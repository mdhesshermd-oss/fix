class LB_Elevator_Mission : LB_Elevator_World {

	override void ShowIngameHud(bool show) {
		Mission mission = g_Game.GetMission();
		if (!mission)
			return;
		IngameHud hud = IngameHud.Cast(mission.GetHud());
		if (hud) {
			hud.ShowQuickbarUI(g_Game.GetProfileOption(EDayZProfilesOptions.QUICKBAR) && show);
			hud.ShowHudUI(g_Game.GetProfileOption(EDayZProfilesOptions.HUD) && show);
		}
	}

	override string GetPlayerTag(string steamid, string bohemiaId) {
#ifdef SERVER
#ifdef LBGROUP_SYSTEM
#ifndef LB_DISABLE_CHAT
		PrefixGroup prefix = ChatConfig.Get.GetPrefixForSteamid(steamid, false);
		if (prefix) {
			return prefix.prefix;
		}
#endif
#endif
#endif
		return "";

	}
	override bool GetPlayerTagColor(string steamid, string bohemiaId, inout LBColorConfig cfg) {
#ifdef SERVER
#ifdef LBGROUP_SYSTEM
#ifndef LB_DISABLE_CHAT
		PrefixGroup prefix = ChatConfig.Get.GetPrefixForSteamid(steamid, false);
		if (prefix) {
			cfg = LBColorConfig.Init(255, prefix.colorR, prefix.colorG, prefix.colorB);
			return true;
		}
#endif
#endif
#endif
		return false;
	}

	override string GetLicenseServerConnection() {
		ServerUpdateRequestPacket packet = new ServerUpdateRequestPacket();
		return packet.GetLastLicenseServerIP() + ":" + packet.GetLicenseServerPort();
	}

	override LBUniversalServerMarker CreateServerMarker(string name, vector position, int color, string icon, bool displayMap = true, bool display3D = true, float radius = 0, int radiusColor = -1, bool radiusStriked = false) {
		return new LBUniversalServerMarkerImpl(name, position, color, icon, displayMap, display3D, radius, radiusColor, radiusStriked);
	}
}
int lb_elevator_init_mission = LB_ElevatorInitMission();
int LB_ElevatorInitMission() {
	LB_Elevator.Set(new LB_Elevator_Mission());
	return 0;
}
LB_Elevator_Mission GetElevatorMission() {
	return LB_Elevator_Mission.Cast(LB_Elevator.Get());
}