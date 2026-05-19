[DayZGame.RegisterLBAdminPage("AdminPermissionMenu", "LBmaster_Core/logo/logo.paa", "Admin Permissions", "core.change.admins")]
class AdminPermissionMenu : LBAdmin_Menu_Page {

	WrapSpacerWidget panel_permissions;
	CheckBoxWidget chckbx_grant_all;
	EditBoxWidget edit_ingamename, edit_comment, edit_perm_name, edit_steamid, edit_search;
	TextListboxWidget list_admins, list_admin_groups, list_permissions;
	ButtonWidget btn_del, btn_add, btn_add_perm, btn_rem_perm, btn_del_admin, btn_add_admin, btn_find_steamid;

	ref LBAdmins config;

	LBWidget_Linked_List listGroups;
	ref array<ref Param2<string, string>> sortedPermissions = new array<ref Param2<string, string>>();
	ref TIntSet importantPerms = new TIntSet();

	override void OnShow() {
		g_Game.RPCSingleParam(null, LBmaster_Core_RPCs.REQUEST_ADMIN_PERMISSIONS, new Param1<bool>(true), true);
	}

	override array<string> GetEditedConfigs() {}

	override void RegisterAllLinkedVars() {
		linked.RegisterLinkedList("config.admins", list_admins, btn_add_admin, btn_del_admin).SetListManager(new LBListManager<LBAdminsPlayer>()).SetColumnCount(2).PreventDuplicating();
		linked.RegisterLinkedVar("config.admins[list_admins].comment", edit_comment, true);
		linked.RegisterLinkedVar("config.admins[list_admins].ingameNameForPermissions", edit_ingamename);
		linked.RegisterLinkedVar("config.admins[list_admins].grantAllPermissions", chckbx_grant_all);
		linked.RegisterLinkedVar("config.admins[list_admins].steamid", edit_steamid, true);
		listGroups = linked.RegisterLinkedList("config.admins[list_admins].permissionGroups", list_admin_groups, null, btn_del);
		linked.RegisterLinkedList("config.groups", list_permissions, btn_add_perm, btn_rem_perm).SetListManager(new LBListManager<LBAdminsGroup>()).SetChangeTrigger("UpdateGrantedPermissions()");
		linked.RegisterLinkedVar("config.groups[list_permissions].name", edit_perm_name, true);
	}

	override void OnRPC(Object target, int rpc_type, ParamsReadContext ctx) {
		if (rpc_type == LBmaster_Core_RPCs.REQUEST_ADMIN_PERMISSIONS) {
			ctx.Read(config);
			ctx.Read(sortedPermissions);
			ctx.Read(importantPerms);
			linked.LoadLinkedVars();
		}
	}

	override bool HasSaveButton() {
		return true;
	}

	override void SaveEditedConfigs() {
		LBLoadingIcon.Show();
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(config);
		rpc.Send(null, LBmaster_Core_RPCs.SAVE_ADMIN_PERMISSIONS, true);
	}

	override bool OnClick(Widget w, int x, int y, int button) {
		if (super.OnClick(w, x, y, button))
			return true;
		if (w == btn_add) {
			int select = list_permissions.GetSelectedRow();
			if (select >= 0 && select < list_permissions.GetNumItems()) {
				string name;
				list_permissions.GetItemText(select, 0, name);
				listGroups.AddEntryStr(name, false);
			}
		} else if (w == btn_find_steamid) {
			LBMenuDataPlayerSelector data = new LBMenuDataPlayerSelector(ScriptCaller.Create(OnSteamidSet));
			LBMenuManager.Get().OpenMenu("LBPlayerSelectorDialog", data, false, g_Game.GetUIManager().GetMenu());
		}
		int index = w.GetUserID() - 1;
		if (index >= 0) {
			bool selected = CheckBoxWidget.Cast(w).IsChecked();
			LBAdminsGroup grp = GetSelectedGroup();
			if (!grp)
				return true;
			grp.Set(sortedPermissions.Get(index).param1, selected);
		}
		return false;
	}

	override bool OnChange(Widget w, int x, int y, bool finished) {
		if (super.OnChange(w, x, y, finished))
			return true;
		if (w == edit_search) {
			LoadGrantedPermissions(true);
			return true;
		}
		return false;
	}

	override bool OnMouseEnter(Widget w, int x, int y) {
		if (super.OnMouseEnter(w, x, y))
			return true;
		int index = w.GetUserID() - 1;
		if (index < 0 && w.GetParent())
			index = w.GetParent().GetUserID() - 1;
		if (index >= 0 && (TextWidget.Cast(w) || CheckBoxWidget.Cast(w))) {
			string tooltip = sortedPermissions.Get(index).param2;
			if (importantPerms.Find(index) != -1) {
				tooltip = tooltip + "\nThis is a very risky permission to enable!";
			}
			LBTooltipManager.Get().DisplayTooltip(w, tooltip);
			return true;
		}
		return false;
	}

	void OnSteamidSet(PlayerNameSave_ player) {
		edit_steamid.SetText(player.GetSteamid());
		linked.OnVarChange(edit_steamid);
	}

	LBAdminsGroup GetSelectedGroup() {
		int index = list_permissions.GetSelectedRow();
		if (index < 0 || index >= list_permissions.GetNumItems())
			return null;
		return config.groups.Get(index);
	}

	void UpdateGrantedPermissions() {
		LoadGrantedPermissions(false);
	}

	void LoadGrantedPermissions(bool resetScroll) {
		LBWidgetUtils.DeleteAllChildren(panel_permissions);
		LBAdminsGroup selected = GetSelectedGroup();
		string search = edit_search.GetText();
		if (!selected)
			return;
		int index = -1;
		foreach (Param2<string, string> permission : sortedPermissions) {
			++index;
			if (search != "" && !LBStringTools.ContainsIgnoreCase(permission.param1, search) && !LBStringTools.ContainsIgnoreCase(permission.param2, search))
				continue;
			CheckBoxWidget chckbx = CheckBoxWidget.Cast(LBLayoutManager.Get().CreateLayout("SimpleCheckbox", panel_permissions));
			TextWidget temp_text = TextWidget.Cast(chckbx.FindAnyWidget("temp_text"));
			temp_text.SetText(permission.param1);
			chckbx.SetChecked(selected.Has(permission.param1));
			chckbx.SetUserID(index + 1);
			if (importantPerms.Find(index) != -1) {
				temp_text.SetColor(ARGB(255, 255, 0, 0));
			}
		}
		if (resetScroll) {
			ScrollWidget parent = ScrollWidget.Cast(panel_permissions.GetParent());
			parent.VScrollToPos(0);
		}
	}

}