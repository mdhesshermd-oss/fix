class LBTip4ServConfig {

	string key;

	static string LoadKey() {
		if (!FileExist("$profile:LBmaster/tip4serv_config.json"))
			return "";
		LBTip4ServConfig cfg;
		string error;
		if (!JsonFileLoader<LBTip4ServConfig>.LoadFile("$profile:LBmaster/tip4serv_config.json", cfg, error) || !cfg)
			return "";
		return cfg.key;
	}

	static void RegisterHandler() {
		string apiKey = LoadKey();
		if (apiKey == "")
			return;
		string pollUrl = "https://api.tip4serv.com/lbmaster/poll?";
		string responseUrl = "https://api.tip4serv.com/lbmaster/update?";
		LBAPIManager.StartHandler("Tip4Serv", pollUrl, responseUrl, false, "\r\nAuthorization: Bearer " + apiKey + "\r\nContent-Type: application/json", 60);
	}

}