class ServerUpdateRequestPacket {

	string hostname = "CONFIG FILE NOT FOUND";
	bool hostnameRead = false;
	int maxPlayers = -1;
	string configPath;
	bool configExists = false;
	string profilePath;
	bool profilePathSet = false;
	int gamePort = 2302;
	string ip;
	string modsInstalled;
	ref TStringArray cfgMods = new TStringArray();

	string GetLastLicenseServerIP() {
		return "213.109.160.200";
	}
	int GetLicenseServerPort() {
		return 2118;
	}
	string GetModsInstalled() {
		return "";
	}

	ServerUpdateRequestPacket Load() {
		LBLogger.Verbose("LB Update Check start...", "Core");
		LoadServerConfig();
		LoadCLIParams();
		LoadInstalledMods();
		return this;
	}

	int Send() {
		LBLogger.Verbose("Sending Mod version requests", "Core");
		RestApi api = GetRestApi();
		if (!api)
			api = CreateRestApi();
		string ipTo = "https://" + GetLastLicenseServerIP() + ":" + GetLicenseServerPort() + "/updates?format=new";
		RestContext ctx = api.GetRestContext(ipTo);
		ctx.POST(new LBVersionUpdateCheckCallback(ipTo), "", ToJson());
		LBLogger.Verbose("Finished Mod update analysis", "Core");
		return 1;
	}

	void LoadServerConfig() {
		configPath = GetConfigPath();
		string value;
		if (FileExist(configPath)) {
			map<string, string> vars = ParseConfig(configPath);
			configExists = true;
			if (vars.Find("hostname", value)) {
				hostnameRead = true;
				hostname = value;
			}
			if (vars.Find("maxplayers", value)) {
				maxPlayers = value.ToInt();
			}
		}
	}

	void LoadCLIParams() {
		string value;
		if (GetCLIParam("port", value))
			gamePort = value.ToInt();
		if (GetCLIParam("ip", value))
			ip = value;
		if (GetCLIParam("profiles", value)) {
			profilePath = value;
			profilePathSet = true;
		}
	}

	void LoadInstalledMods() {
		modsInstalled = GetModsInstalled();
		int children = g_Game.ConfigGetChildrenCount("CfgMods");
		for (int i = 0; i < children; i++) {
			string child;
			if (!g_Game.ConfigGetChildName("CfgMods", i, child))
				continue;
			cfgMods.Insert(child);
		}
	}

	string ToJson() {
		JsonSerializer serial = new JsonSerializer();
		string output;
		ServerUpdateRequestPacket packet = this;
		serial.WriteToString(packet, false, output);
		LBLogger.Verbose("LBUpdate Packet is " + output.Length() + " bytes long", "Core");
		return output;
	}

	string GetConfigPath() {
		string cliParam;
		string path = "serverDZ.cfg";
		if (GetCLIParam("config", cliParam))
			path = cliParam;
		if (!FileExist(path) && (path.Contains("/") || path.Contains("\\"))) {
			string folder = path;
			while (folder != "") {
				folder = GetParentFolder(folder);
				string fromFile = path.Substring(folder.Length() + 1, path.Length() - 1 - folder.Length());
				if (FileExist(fromFile)) {
					path = fromFile;
					break;
				}
			}
		}
		return "$CurrentDir:" + path;
	}

	static string GetParentFolder(string path) {
		int length = path.Length();
		int index = length - 1;
		while (index >= 0) {
			string part = path.Substring(index, length - index);
			if (part.Contains("/") || part.Contains("\\"))
				break;
			index--;
		}
		if (index >= 0)
			return path.Substring(0, index);
		return "";
	}

	map<string, string> ParseConfig(string path) {
		LBLogger.Verbose("Parsing Server config at " + path, "Core");
		ParseHandle handle = BeginParse(path);
		if (handle == 0)
			return new map<string, string>();
		const int count = 100;
		string str[count];
		int index = 0;
		map<string, string> variables = new map<string, string>();
		int variableStatus = 0;
		string varName = "";
		while (true) {
			int parsed = ParseLine(handle, index, str);
			index++;
			if (parsed <= 0)
				break;
			for (int i = 0; i < parsed; i++) {
				string lower = str[i];
				lower.ToLower();
				int oldStatus = variableStatus;
				if (lower == "=" && variableStatus == 1) {
					variableStatus = 2;
				} else if (lower == "{" || lower == "}") {
					variableStatus = 0;
				} else if (lower == ";") {
				} else if (variableStatus == 2) {
					variables.Insert(varName, str[i]);
					variableStatus = 0;
				} else if (variableStatus == 0 && str[i].Length() > 2) {
					varName = str[i];
					varName.ToLower();
					variableStatus = 1;
				}
			}
		}
		EndParse(handle);
		return variables;
	}

}