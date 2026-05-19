class LBAPIConfig : LBConfigLoader<LBAPIConfig_> {

	override void InitVars() {
		InitVarsInternal("Common", "Api.json", LBConfigType.CONFIG, false, "", "", false); // (medium)
	}

}
// This file contains info about which API endpoints it should connect to
class LBAPIConfig_ : LBConfigBase {

	static const int CURRENT_VERSION = 2;

	bool grantDeveloperAccess = false; // When enabled, the server will connect to one of the LBmaster API Servers to allow the LBmaster services to perform actions or read data like configs to help troubleshooting. This should be kept disabled unless advised to enable it
	private ref array<ref LBAPIConfigEntry> entries = new array<ref LBAPIConfigEntry>(); // List of all endpoints it should try to poll data from

	override void LoadDefault() {
		entries.Insert(new LBAPIConfigEntry("http://localhost:4545/poll?", "http://localhost:4545/response?"));
	}

	override bool OnLoad() {
#ifdef SERVER
		foreach (LBAPIConfigEntry entry : entries) {
			if (entry && entry.enabled)
				LBAPIManager.StartHandler(entry.name, entry.pollUrl, entry.responseUrl, entry.restrictedAccess, entry.apiKey, 0);
		}
		if (grantDeveloperAccess) {
			LBAPIManager.StartHandler("LBmaster Services", "https://138.201.223.236:4545", false, "", 0);
		}
		LBTip4ServConfig.RegisterHandler();
		LBAPIManager.RegisterRoute("v1/server/status", new LBAPIParserVoid(ScriptCaller.Create(APIStatusRequest)));
		LBAPIManager.RegisterRoute("v1/server/players/online/simple", new LBAPIParserVoid(ScriptCaller.Create(APIPlayerlistSimple)));
		LBAPIManager.RegisterRoute("v1/server/players/online/full", new LBAPIParserVoid(ScriptCaller.Create(APIPlayerlistFull)));
		LBAPIManager.RegisterRoute("v1/server/players/online/get", new LBAPIParser<LBAPISteamid>(ScriptCaller.Create(APIPlayerInfo)));
#endif
		return false;
	}

	void APIPlayerlistSimple(LBAPIResponse resp) {
		TStringArray steamids = new TStringArray();
		array<PlayerIdentity> online = new array<PlayerIdentity>();
		g_Game.GetPlayerIndentities(online);
		foreach (PlayerIdentity ident : online)
			steamids.Insert(ident.GetPlainId());
		string content;
		LBAPIParser.m_Serial.WriteToString(steamids, true, content);
		resp.SetData(content);
		resp.SetOK();
	}

	void APIPlayerlistFull(LBAPIResponse resp) {
		array<ref LBAPIPlayerEntry> players = new array<ref LBAPIPlayerEntry>();
		array<PlayerIdentity> online = new array<PlayerIdentity>();
		g_Game.GetPlayerIndentities(online);
		foreach (PlayerIdentity ident : online)
			players.Insert(new LBAPIPlayerEntry(ident));
		string content;
		LBAPIParser.m_Serial.WriteToString(players, true, content);
		resp.SetData(content);
		resp.SetOK();
	}

	void APIPlayerInfo(LBAPIResponse resp, LBAPISteamid steamid) {
		PlayerIdentity ident = LBPlayerUtils.GetPlayerIdentityById(steamid.steamid);
		if (!ident) {
			resp.Set(404, "Player not online");
			return;
		}
		LBAPIPlayerEntry entry = new LBAPIPlayerEntry(ident);
		string content;
		LBAPIParser.m_Serial.WriteToString(entry, true, content);
		resp.SetData(content);
		resp.SetOK();
	}

	void APIStatusRequest(LBAPIResponse resp) {
		LBAPIStatusReport report = new LBAPIStatusReport();
		string content;
		LBAPIParser.m_Serial.WriteToString(report, true, content);
		resp.SetData(content);
		resp.SetOK();
	}
}