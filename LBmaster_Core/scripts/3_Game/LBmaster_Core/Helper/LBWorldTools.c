class LBWorldTools {

	static ref TStringArray lastBlockedByItems;

	static bool IsAreaBlockedByObjects(vector pos, float radius = 1.0, float height = 3.0, bool allowWater = false, PhxInteractionLayers layer = -1) {
		vector transform[4];
		Math3D.MatrixIdentity4(transform);
		transform[3] = pos;
		CollisionOverlapCallbackAnyLBCore callback = new CollisionOverlapCallbackAnyLBCore();
		lastBlockedByItems = callback.blockingObjects;
		if (DayZPhysics.CapsuleOverlapBullet(transform, radius, height, layer, callback) && callback.isBlocked)
			return true;
		if (allowWater)
			return false;
		return g_Game.SurfaceIsSea(pos[0], pos[2]) || g_Game.SurfaceIsPond(pos[0], pos[2]);
	}

	static string GetServerString() {
		string ip;
		int port;
		if (GetServerInfoLB(ip, port)) {
			return ip + ":" + port;
		}
		return "";
	}

	static bool GetServerInfoLB(out string ip, out int port) {
		MenuData menu_data = g_Game.GetMenuData();
		GetServersResultRow info = OnlineServices.GetCurrentServerInfo();
		if (g_Game.GetHostAddress(ip, port))
			return true;

		if (info) {
			ip = info.m_HostIp;
			port = info.m_HostPort;
			return true;
		} else if (menu_data && menu_data.GetLastPlayedCharacter() != GameConstants.DEFAULT_CHARACTER_MENU_ID) {
			int char_id = menu_data.GetLastPlayedCharacter();
			string address, name;

			menu_data.GetLastServerAddress(char_id, address);
			port = menu_data.GetLastServerPort(char_id);
			ip = address;
			return true;
		}
		return false;
	}

}