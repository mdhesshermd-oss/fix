[DayZGame.RegisterLBAdminPage("LBPlayerListAdminMenu", "LBmaster_Core/logo/logo.paa", "Playerlist", "core.read.players")]
class LBPlayerListAdminMenu : LBAdmin_Menu_Page {

	TextWidget title, txt_playername, txt_steamid, txt_guid, txt_first, txt_last, txt_first_diff, txt_last_diff;
	TextListboxWidget list_players;
	CheckBoxWidget chckbx_online;
	Widget panel_info;
	ButtonWidget btn_copy_name, btn_copy_steamid, btn_copy_guid;
	EditBoxWidget edit_search;

	ref array<ref PlayerNameSave_> players = new array<ref PlayerNameSave_>();

	override array<string> GetEditedConfigs() {
		return {};
	}

	override void OnShow() {
		g_Game.RPCSingleParam(null, LBmaster_Core_RPCs.ADMIN_PLAYERS_REQUEST, new Param1<bool>(true), true);
		LBLoadingIcon.Show();
	}

	override void OnRPC(Object target, int rpc_type, ParamsReadContext ctx) {
		if (rpc_type == LBmaster_Core_RPCs.ADMIN_PLAYERS_REQUEST) {
			LBLoadingIcon.Hide();
			if (!ctx.Read(players)) {
				LBLogger.Error("Could not read player list from server", "Core");
				return;
			}
			TStringSet online = LBPlayerUtils.GetOnlineSteamidsSet();
			int onlineP = 0;
			foreach (PlayerNameSave_ player : players) {
				player.InitOnline(online);
				if (player.online)
					++onlineP;
			}
			LBLogger.Info("Received " + players.Count() + " player infos from the server. Online: " + onlineP + " (" + online.Count() + ")", "Core");
			linked.LoadLinkedVars();
		}
	}

	override void RegisterAllLinkedVars() {
		linked.RegisterLinkedList("players", list_players).SetSearchBar(edit_search, {chckbx_online}).SetColumnCount(2).SetListManager(new LBListManagerSearch<PlayerNameSave_>());
		linked.RegisterLinkedVar("players[list_players].lastName", txt_playername);
		linked.RegisterLinkedVar("players[list_players].steamid", txt_steamid);
		linked.RegisterLinkedVar("players[list_players].bohemiaId", txt_guid);
		linked.RegisterLinkedVar("players[list_players].GetFirstOnlineString()", txt_first);
		linked.RegisterLinkedVar("players[list_players].GetFirstOnlineDiffString()", txt_first_diff);
		linked.RegisterLinkedVar("players[list_players].GetLastOnlineString()", txt_last);
		linked.RegisterLinkedVar("players[list_players].GetLastOnlineDiffString()", txt_last_diff);
	}

}