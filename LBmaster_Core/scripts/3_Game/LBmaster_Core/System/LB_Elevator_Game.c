class LB_Elevator_Game : LB_Elevator {

	LB_ATM_PlayerbaseBase LoadATMPlayer(Man player);
	void ShowIngameHud(bool show);
	bool AddAdminPlacedItem(vector position, vector direction, string itemname, PlayerIdentity sender, string placementName) {
		return false;
	}
	string GetPlayerGroupName(string steamid, string bohemiaId, out bool visible) {}
	string GetPlayerTag(string steamid, string bohemiaId) {}
	bool GetPlayerGroupColor(string steamid, string bohemiaId, inout LBColorConfig cfg) {
		return false;
	}
	bool GetPlayerTagColor(string steamid, string bohemiaId, inout LBColorConfig cfg) {
		return false;
	}
	string GetLicenseServerConnection() {
		return "";
	}

	void OnGroupDeleted(string shortname, string groupname, bool empty, bool admin, bool inactive);
	void OnGroupRenamed(string newShortname, string newGroupname, string oldShortname, string oldGroupname);

	LBUniversalServerMarker CreateServerMarker(string name, vector position, int color, string icon, bool displayMap = true, bool display3D = true, float radius = 0, int radiusColor = -1, bool radiusStriked = false) {}

}
int lb_elevator_init_game = LB_ElevatorInitGame();
int LB_ElevatorInitGame() {
	LB_Elevator.Set(new LB_Elevator_Game());
	return 0;
}
LB_Elevator_Game GetElevatorGame() {
	return LB_Elevator_Game.Cast(LB_Elevator.Get());
}