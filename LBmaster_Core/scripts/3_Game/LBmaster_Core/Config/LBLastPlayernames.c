class LBLastPlayernames : LBConfigLoader<LBLastPlayernames_> {

	[SetPriority(LBConfigPriority.LOW)]
	override void InitVars() {
		InitVarsInternal("Core", "playernames.json", LBConfigType.DATA, true, "core.read.players", "core.read.players"); // (easy)
	}

}
// Save all last playernames to display them when the player is offline and makes a trap kill setup by them and for more things
// This file should not be edited manually
class LBLastPlayernames_ : LBConfigBase {

	static const int CURRENT_VERSION = 1;

	[NonSerialized()]
	private ref map<string, PlayerNameSave_> playernamesSteam = new map<string, PlayerNameSave_>();
	[NonSerialized()]
	private ref map<string, PlayerNameSave_> playernamesBI = new map<string, PlayerNameSave_>();
	[NonSerialized()]
	ref array<ref PlayerNameSave_> loadedPlayers = new array<ref PlayerNameSave_>();

	ref TStringSet players = new TStringSet(); // This is just a list of all steamids ever joined the server

	override bool OnLoad() {
		bool changed = false;
		foreach (string steamid : players) {
			PlayerNameSave_ player = PlayerNameSave.Loader.LoadPlayer(LBPlayer(steamid));
			loadedPlayers.Insert(player);
			AddToCache(player);
		}
		LBAPIManager.RegisterRoute("/v1/server/players/offline", new LBAPIParserVoid(ScriptCaller.Create(APIOnPlayerList)));
		LBAPIManager.RegisterRoute("/v1/server/players/offline/get", new LBAPIParser<LBAPISteamid>(ScriptCaller.Create(APIOnPlayerInfo)));
		return false;
	}

	override bool UseCustomRPCReadWrite() {
		return true;
	}

	override void WriteToCtx(ParamsWriteContext ctx) {
		int count = loadedPlayers.Count();
		ctx.Write(count);
		foreach (PlayerNameSave_ player : loadedPlayers) {
			if (!player)
				ctx.Write(0);
			else
				player.WriteToCtxSimple(ctx);
		}
	}

	override bool ReadFromCtx(ParamsReadContext ctx) {
		int count;
		if (!ctx.Read(count))
			return false;
		loadedPlayers = new array<ref PlayerNameSave_>();
		for (int i = 0; i < count; ++i) {
			PlayerNameSave_ player = PlayerNameSave_.ReadFromCtxSimple(ctx);
			if (player)
				loadedPlayers.Insert(player);
		}
		playernamesSteam = new map<string, PlayerNameSave_>();
		playernamesBI = new map<string, PlayerNameSave_>();
		foreach (PlayerNameSave_ name : loadedPlayers) {
			AddToCache(name);
		}
		return true;
	}

	void APIOnPlayerInfo(LBAPIResponse resp, LBAPISteamid steamid) {
		PlayerNameSave_ info = GetPlayerInfo(steamid.steamid);
		string content;
		LBAPIParser.m_Serial.WriteToString(info, true, content);
		resp.SetData(content);
		resp.SetOK();
	}

	void APIOnPlayerList(LBAPIResponse resp) {
		string content;
		LBAPIParser.m_Serial.WriteToString(players, false, content);
		resp.SetData(content);
		resp.SetOK();
	}

	void AddToCache(PlayerNameSave_ name) {
		playernamesSteam.Insert(name.GetSteamid(), name);
		playernamesBI.Insert(name.GetBohemiaId(), name);
	}

	void AddName(PlayerIdentity ident) {
		if (!ident)
			return;

		string name = ident.GetName();
		PlayerNameSave_ found;
		if (playernamesSteam.Find(ident.GetPlainId(), found) && found) {
			found.UpdateName(ident);
		} else {
			found = PlayerNameSave.Loader.LoadPlayer(ident);
			players.Insert(found.GetSteamid());
			loadedPlayers.Insert(found);
			AddToCache(found);
			LBLastPlayernames.Loader.SaveNoSync();
		}
		found.UpdateLastOnlineTimestamp();
	}

	void OnPlayerOffline(string guid) {
		PlayerNameSave_ info = GetByBohemiaId(guid);
		if (!info)
			return;
		info.UpdateLastOnlineTimestamp();
		PlayerNameSave.Loader.SavePlayer(info);
	}

	array<PlayerNameSave_> GetPlayersOfflineSince(int timestamp) {
		array<PlayerNameSave_> offline = new array<PlayerNameSave_>();
		foreach (string steamid, PlayerNameSave_ player : playernamesSteam) {
			if (player.GetLastOnlineTimestamp() < timestamp)
				offline.Insert(player);
		}
		return offline;
	}

	TStringArray GetPlayersOfflineSinceSteamids(int timestamp) {
		TStringArray offline = new TStringArray();
		foreach (string steamid, PlayerNameSave_ player : playernamesSteam) {
			if (player.GetLastOnlineTimestamp() < timestamp)
				offline.Insert(player.GetSteamid());
		}
		return offline;
	}

	array<PlayerNameSave_> GetAllTrackedPlayers() {
		array<PlayerNameSave_> copy = new array<PlayerNameSave_>();
		foreach (string steamid, PlayerNameSave_ player : playernamesSteam) {
			copy.Insert(player);
		}
		return copy;
	}

	PlayerNameSave_ GetPlayerInfo(string steamid) {
		return playernamesSteam.Get(steamid);
	}

	PlayerNameSave_ GetByBohemiaId(string bohemiaId) {
		return playernamesBI.Get(bohemiaId);
	}

	string GetFullPlayerName(string steamid) {
		if (steamid == "Unknown")
			return "BOT";
		PlayerNameSave_ name;
		if (playernamesSteam.Find(steamid, name)) {
			return name.GetPlayerGroupTag(true) + " " + name.GetName();
		}
		return "Unknown";
	}

	PlayerNameSave_ GetName(string steamid) {
		PlayerNameSave_ name;
		if (playernamesSteam.Find(steamid, name)) {
			return name;
		}
		return null;
	}

	string GetPlayerName(string steamid) {
		if (steamid == "Unknown")
			return "BOT";
		PlayerNameSave_ name;
		if (playernamesSteam.Find(steamid, name)) {
			return name.GetName();
		}
		return "Unknown";
	}

	string GetPlayerTag(string steamid) {
		if (steamid == "Unknown")
			return "";
		PlayerNameSave_ name;
		if (playernamesSteam.Find(steamid, name)) {
			return name.GetTag();
		}
		return "Unknown";
	}

	string GetPlayerGroup(string steamid) {
		if (steamid == "Unknown")
			return "";
		PlayerNameSave_ name;
		if (playernamesSteam.Find(steamid, name)) {
			return name.GetPlayerGroupTag(false);
		}
		return "";
	}

}