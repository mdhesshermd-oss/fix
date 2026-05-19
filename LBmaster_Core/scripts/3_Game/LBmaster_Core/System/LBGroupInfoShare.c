class LBGroupInfoShare {

	static ref map<string, ref Param2<string, string>> playerGroups = new map<string, ref Param2<string, string>>();
	static int lastChange = 0;

	static void SetPlayerGroup(string steamid, string groupName, string groupShortname) {
		string oldname, oldshortname;
		if (GetPlayerGroup(steamid, oldname, oldshortname) && oldname == groupName && oldshortname == groupShortname)
			return;
		playerGroups.Set(steamid, new Param2<string, string>(groupName, groupShortname));
		ScriptRPC rpc = CreateRPC(2);
		rpc.Write(steamid);
		rpc.Write(groupName);
		rpc.Write(groupShortname);
		SendRPC(rpc);
	}

	static void UnsetPlayerGroup(string steamid) {
		if (!playerGroups.Contains(steamid))
			return;
		playerGroups.Remove(steamid);
		ScriptRPC rpc = CreateRPC(1);
		rpc.Write(steamid);
		SendRPC(rpc);
	}

	static void OnPlayerOffline(string steamid) {
		UnsetPlayerGroup(steamid);
	}

	static bool GetPlayerGroup(string steamid, out string groupname, out string groupshortname) {
		Param2<string, string> param;
		if (playerGroups.Find(steamid, param)) {
			groupname = param.param1;
			groupshortname = param.param2;
			return true;
		}
		return false;
	}

	static void OnPlayerOnline(PlayerIdentity identity) {
		if (!LBAdmins.Get().HasPermission("admin.view.playerinfo", identity))
			return;
		ScriptRPC rpc = CreateRPC(0);
		rpc.Write(playerGroups.Count());
		foreach (string steamid, Param2<string, string> groupInfo : playerGroups) {
			rpc.Write(steamid);
			rpc.Write(groupInfo.param1);
			rpc.Write(groupInfo.param2);
		}
		rpc.Send(null, LBmaster_Core_RPCs.SHARE_PLAYER_GROUP_INFO, true, identity);
	}

	static void OnRPC(ParamsReadContext ctx) {
#ifndef SERVER
		int type;
		string steamid, group, shortname;
		if (!ctx.Read(type))
			return;
		switch (type) {
			case 0:
				playerGroups = new map<string, ref Param2<string, string>>();
				int count;
				if (!ctx.Read(count))
					return;
				for (int i = 0; i < count; ++i) {
					if (!ctx.Read(steamid) || !ctx.Read(group) || !ctx.Read(shortname))
						return;
					playerGroups.Insert(steamid, new Param2<string, string>(group, shortname));
				}
				break;
			case 1:
				if (!ctx.Read(steamid))
					return;
				playerGroups.Remove(steamid);
				break;
			case 2:
				if (!ctx.Read(steamid) || !ctx.Read(group) || !ctx.Read(shortname))
					return;
				playerGroups.Set(steamid, new Param2<string, string>(group, shortname));
				break;
		}
		lastChange = g_Game.GetTime();
#endif
	}

	private static void SendRPC(ScriptRPC rpc) {
		LBAdmins.Get().SendRPCToAdminsWithPermission("admin.view.playerinfo", rpc, LBmaster_Core_RPCs.SHARE_PLAYER_GROUP_INFO);
	}

	private static ScriptRPC CreateRPC(int type) {
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(type);
		return rpc;
	}

}