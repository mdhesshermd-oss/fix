class LBAPIStatusReport {

	string localTime;
	string uptime;
	int players;
	int maxPlayers;
	string mapName;
	int port;

	void LBAPIStatusReport() {
		localTime = LBDate.Init(false).ToFormattedString();
		uptime = LBDate.Init(g_Game.GetTime() / 1000).ToDiffString();
		array<PlayerIdentity> online = new array<PlayerIdentity>();
		g_Game.GetPlayerIndentities(online);
		players = online.Count();
		g_Game.GetWorldName(mapName);
		maxPlayers = g_Game.ServerConfigGetInt("maxPlayers");
		string portStr;
		if (IsCLIParam("port") && GetCLIParam("port", portStr))
			port = portStr.ToInt();
	}

}