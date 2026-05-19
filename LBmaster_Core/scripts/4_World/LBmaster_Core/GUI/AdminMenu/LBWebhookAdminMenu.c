[DayZGame.RegisterLBAdminPage("LBWebhookAdminMenu", "LBmaster_Core/logo/logo.paa", "Webhooks", "core.change.webhooks")]
class LBWebhookAdminMenu : LBAdmin_Menu_Page {

	TextWidget txt_message;
	CheckBoxWidget chckbx_enabled;
	XComboBoxWidget combo_webhooks;
	EditBoxWidget edit_name, edit_url, edit_search;
	TextListboxWidget list_enabled, list_available, list_steamids;
	Widget panel_received, panel_other;
	ButtonWidget btn_add, btn_del, btn_add_message, btn_del_message, btn_add_all, btn_del_all, btn_add_steamid, btn_remove_steamid;

	ref TStringArray allNames, available;
	ref LB_Webhook_Manager_ config = new LB_Webhook_Manager_();
	string search = "";

	override void OnPageShown() {
		super.OnPageShown();
		SetText("Waiting for the config...");
		if (LBAdmins.Get().HasPermission("core.change.webhooks", g_Game.GetPlayer(), true)) {
			g_Game.RPCSingleParam(null, LBmaster_Core_RPCs.ADMIN_WEBHOOK_CONFIG, new Param1<bool>(true), true);
			LBLoadingIcon.Show();
		} else {
			SetText("You don't have the permission to view this!");
		}
	}

	override array<string> GetEditedConfigs() {}

	override bool HasSaveButton() {
		return true;
	}

	override Widget ClickOnSpacePressed() {
		Widget focus = GetFocus();
		if (focus == list_available)
			return btn_add_message;
		else if (focus == list_enabled)
			return btn_del_message;
		return null;
	}
	override XComboBoxWidget UseArrowKeysWidget() {
		return combo_webhooks;
	}

	override void OnRPC(Object target, int rpc_type, ParamsReadContext ctx) {
		if (rpc_type == LBmaster_Core_RPCs.ADMIN_WEBHOOK_CONFIG) {
			LBLoadingIcon.Hide();
			if (!config.ReadFromCtx(ctx))
				return;
			LBLogger.Info("Received Webhooks config from server", "Core");
			allNames = new TStringArray();
			foreach (string name, LB_Webhook_Message webhook : config.webhookMessages)
				allNames.Insert(name);
			allNames.Sort();
			linked.LoadLinkedVars();
			ShowReceived();
			oldConfigContent = GetConfigContent();
		}
	}

	override void RegisterAllLinkedVars() {
		linked.RegisterLinkedList("config.webhooks", combo_webhooks, btn_add, btn_del).SetListManager(new LBListManager<ref LB_Webhook>()).SetChangeTrigger("LoadAvailable()");
		linked.RegisterLinkedVar("config.webhooks[combo_webhooks].description", edit_name);
		linked.RegisterLinkedVar("config.webhooks[combo_webhooks].webhookURL", edit_url);
		linked.RegisterLinkedVar("config.webhooks[combo_webhooks].enabled", chckbx_enabled);
		linked.RegisterLinkedList("config.webhooks[combo_webhooks].webhookMessages", list_enabled, null, btn_del_message);
		linked.RegisterLinkedList("config.webhooks[combo_webhooks].steamidFilter", list_steamids, null, btn_remove_steamid).SetListManager(new LBListManagerString()).SetChangeTrigger("LoadSteamidNames()");
		LoadAvailable();
	}

	void LoadSteamidNames() {
		int row;
		if (!LBWidgetUtils.CheckSelectedInRange(combo_webhooks, row))
			return;
		LB_Webhook webhook = config.webhooks.Get(row);
		int i = 0;
		foreach (string steamid : webhook.steamidFilter) {
			string name = LBLastPlayernames.Get.GetPlayerName(steamid);
			list_steamids.SetItem(i, name, null, 1);
			++i;
		}
	}

	override string GetConfigContent() {
		JsonSerializer serial = new JsonSerializer();
		string output;
		serial.WriteToString(config, false, output);
		return output;
	}

	TStringArray GetAllWebhookNames() {
		return allNames;
	}

	void LoadAvailable() {
		int selected = combo_webhooks.GetCurrentItem();
		if (selected < 0 || selected >= config.webhooks.Count())
			return;
		available = new TStringArray();
		list_available.ClearItems();
		LB_Webhook webhook = config.webhooks.Get(selected);
		foreach (string name : allNames) {
			if (webhook.webhookMessages.Find(name) == -1 && LBStringTools.ContainsIgnoreCase(name, search)) {
				available.Insert(name);
				list_available.AddItem(name, null, 0);
			}
		}
		selected = list_available.GetSelectedRow();
		if (selected >= list_available.GetNumItems())
			list_available.SelectRow(list_available.GetNumItems() - 1);
	}

	override void SaveEditedConfigs() {
		LBLoadingIcon.Show();
		ScriptRPC rpc = new ScriptRPC();
		config.WriteToCtx(rpc);
		rpc.Send(null, LBmaster_Core_RPCs.ADMIN_WEBHOOK_CONFIG_SAVE, true);
		oldConfigContent = GetConfigContent();
	}

	override bool OnClick(Widget w, int x, int y, int button) {
		bool ok = super.OnClick(w, x, y, button);
		if (w == btn_add_message) {
			int selected = combo_webhooks.GetCurrentItem();
			if (selected < 0 || selected >= config.webhooks.Count())
				return true;
			LB_Webhook webhook = config.webhooks.Get(selected);
			int index = list_available.GetSelectedRow();
			if (index < 0 || index >= list_available.GetNumItems())
				return true;
			string found = available.Get(index);
			webhook.webhookMessages.Insert(found);
			available.RemoveOrdered(index);
			list_available.RemoveRow(index);
			linked.TriggerLoad("config.webhooks[combo_webhooks].webhookMessages");
			return true;
		} else if (w == btn_add_all) {
			selected = combo_webhooks.GetCurrentItem();
			if (selected < 0 || selected >= config.webhooks.Count())
				return true;
			webhook = config.webhooks.Get(selected);
			foreach (string avail : available) {
				if (webhook.webhookMessages.Find(avail) == -1) {
					webhook.webhookMessages.Insert(avail);
				}
			}
			edit_search.SetText("");
			search = "";
			LoadAvailable();
			linked.TriggerLoad("config.webhooks[combo_webhooks].webhookMessages");
			return true;
		} else if (w == btn_del_all) {
			selected = combo_webhooks.GetCurrentItem();
			if (selected < 0 || selected >= config.webhooks.Count())
				return true;
			webhook = config.webhooks.Get(selected);
			webhook.webhookMessages.Clear();
			LoadAvailable();
			linked.TriggerLoad("config.webhooks[combo_webhooks].webhookMessages");
			return true;
		} else if (w == btn_del_message) {
			LoadAvailable();
			return true;
		} else if (w == btn_add_steamid) {
			LBMenuManager.Get().OpenMenu("LBPlayerSelectorDialog", new LBMenuDataPlayerSelector(ScriptCaller.Create(OnPlayerSelected)), false, LBMenuManager.Get().GetOpenLBMenu());
			return true;
		}
		return ok;
	}

	void OnPlayerSelected(PlayerNameSave_ player) {
		int row;
		if (!LBWidgetUtils.CheckSelectedInRange(combo_webhooks, row))
			return;
		LB_Webhook webhook = config.webhooks.Get(row);
		if (webhook.steamidFilter.Find(player.GetSteamid()) == -1) {
			webhook.steamidFilter.Insert(player.GetSteamid());
			linked.TriggerLoad("config.webhooks[combo_webhooks].steamidFilter");
		}
	}

	override bool OnChange(Widget w, int x, int y, bool finished) {
		if (super.OnChange(w, x, y, finished))
			return true;
		if (w == edit_search) {
			search = edit_search.GetText();
			LoadAvailable();
			return true;
		}
		return false;
	}

	void SetText(string text) {
		panel_received.Show(false);
		panel_other.Show(true);
		txt_message.SetText(text);
	}

	void ShowReceived() {
		panel_received.Show(true);
		panel_other.Show(false);
	}
}