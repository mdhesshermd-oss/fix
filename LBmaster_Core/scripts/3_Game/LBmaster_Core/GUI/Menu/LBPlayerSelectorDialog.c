class LBPlayerSelectorDialog : LBMenuBase {

	TextWidget title, txt_playername, txt_steamid, txt_guid, txt_first, txt_last, txt_first_diff, txt_last_diff;
	TextListboxWidget list_players;
	CheckBoxWidget chckbx_online;
	ButtonWidget btn_select, btn_copy_name, btn_copy_steamid, btn_copy_guid, btn_close;
	Widget panel_info;
	EditBoxWidget edit_search;

	ref LBMenuDataPlayerSelector playerData;
	ref array<ref PlayerNameSave_> players;

	override string GetLayoutName() {
		return "PlayerSelector";
	}

	override void OnShow() {
		super.OnShow();
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(LoadCurrentPlayerInfo, 1000, true);
	}

	override void OnHide() {
		super.OnHide();
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(LoadCurrentPlayerInfo);
	}

	override void OnDataReceived() {
		playerData = LBMenuDataPlayerSelector.Cast(openParameter);
		if (!playerData)
			playerData = new LBMenuDataPlayerSelector();
		chckbx_online.SetChecked(playerData.onlineOnly);
		g_Game.RPCSingleParam(null, LBmaster_Core_RPCs.ADMIN_PLAYERS_REQUEST, new Param1<bool>(true), true);
	}

	override void OnRPC(Object target, int rpc_type, ParamsReadContext ctx) {
		if (rpc_type == LBmaster_Core_RPCs.ADMIN_PLAYERS_REQUEST) {
			int count;
			if (!ctx.Read(players)) {
				LBLogger.Error("Could not read player list from server!", "Core");
				return;
			}
			LBLogger.Info("Received " + players.Count() + " players from Server", "Core");
			LoadPlayers();
		}
	}

	void LoadPlayers() {
		if (!players)
			return;
		TStringArray online = LBPlayerUtils.GetOnlineSteamids();
		string search = edit_search.GetText();
		int selected = list_players.GetSelectedRow();
		list_players.ClearItems();
		foreach (PlayerNameSave_ player : players) {
			if (DisplayPlayer(player, search, online))
				LoadPlayer(player);
		}
		if (selected >= list_players.GetNumItems() || selected < 0)
			list_players.SelectRow(list_players.GetNumItems() - 1);
		else
			list_players.SelectRow(selected);
		LoadCurrentPlayerInfo();
	}

	void LoadPlayer(PlayerNameSave_ player) {
		int row = list_players.AddItem(player.GetName(), player, 0);
		list_players.SetItem(row, player.GetSteamid(), null, 1);
		list_players.SetItem(row, player.GetBohemiaId(), null, 2);
	}

	void LoadCurrentPlayerInfo() {
		PlayerNameSave_ data = GetSelectedPlayerData();
		if (!data)
			return;
		TStringArray online = LBPlayerUtils.GetOnlineSteamids();
		txt_playername.SetText(data.GetName());
		txt_steamid.SetText(data.GetSteamid());
		txt_guid.SetText(data.GetBohemiaId());
		int now = LBDate.Init(true).GetTimestamp();
		int diffFirst = now - data.GetFirstJoinTimestamp();
		int diffLast = now - data.GetLastOnlineTimestamp();
		txt_first.SetText(LBDate.Init(data.GetFirstJoinTimestamp()).ToFormattedString() + " UTC");
		txt_first_diff.SetText(LBDate.Init(diffFirst).ToDiffString() + " ago");
		if (online.Find(data.GetSteamid()) == -1) {
			txt_last.SetText(LBDate.Init(data.GetLastOnlineTimestamp()).ToFormattedString() + " UTC");
			txt_last_diff.SetText(LBDate.Init(diffLast).ToDiffString() + " ago");
		} else {
			txt_last.SetText("Now");
			txt_last_diff.SetText("0s ago");
		}
	}

	void CopyData(int type, Widget w) {
		PlayerNameSave_ player = GetSelectedPlayerData();
		if (!player)
			return;
		string data = "";
		if (type == 0) {
			data = player.GetName();
		} else if (type == 1) {
			data = player.GetSteamid();
		} else if (type == 2) {
			data = player.GetBohemiaId();
		}
		if (data != "") {
			g_Game.CopyToClipboard(data);
			LBCopiedPopup.ShowAtWidget(w);
		}
	}

	PlayerNameSave_ GetSelectedPlayerData() {
		int row = list_players.GetSelectedRow();
		if (row < 0 || row >= list_players.GetNumItems())
			return null;
		PlayerNameSave_ data;
		list_players.GetItemData(row, 0, data);
		return data;
	}

	bool DisplayPlayer(PlayerNameSave_ player, string search, TStringArray online) {
		if (search != "") {
			if (player.GetSteamid().IndexOf(search) == -1 && player.GetBohemiaId().IndexOf(search) == -1 && !LBStringTools.ContainsIgnoreCase(player.GetName(), search))
				return false;
		}
		if (chckbx_online.IsChecked()) {
			if (online.Find(player.GetSteamid()) == -1)
				return false;
		}
		return true;
	}

	override bool OnClick(Widget w, int x, int y, int button) {
		if (super.OnClick(w, x, y, button))
			return true;
		if (w == btn_copy_name) {
			CopyData(0, w);
			return true;
		} else if (w == btn_copy_steamid) {
			CopyData(1, w);
			return true;
		} else if (w == btn_copy_guid) {
			CopyData(2, w);
			return true;
		} else if (w == btn_close) {
			CloseMe();
			return true;
		} else if (w == btn_select) {
			PlayerNameSave_ player = GetSelectedPlayerData();
			playerData.OnSelect(player);
			CloseMe();
			return true;
		}
		return false;
	}

	override bool OnDoubleClick(Widget w, int x, int y, int button) {
		if (super.OnDoubleClick(w, x, y, button))
			return true;
		if (w == list_players) {
			PlayerNameSave_ player = GetSelectedPlayerData();
			playerData.OnSelect(player);
			CloseMe();
			return true;
		}
		return false;
	}

	override bool OnChange(Widget w, int x, int y, bool finished) {
		if (super.OnChange(w, x, y, finished))
			return true;
		if (w == edit_search || w == chckbx_online) {
			LoadPlayers();
			return true;
		}
		return false;
	}

	override bool OnItemSelected(Widget w, int x, int y, int row, int column, int oldRow, int oldColumn) {
		if (super.OnItemSelected(w, x, y, row, column, oldRow, oldColumn))
			return true;
		if (w == list_players) {
			LoadCurrentPlayerInfo();
			return true;
		}
		return false;
	}

}