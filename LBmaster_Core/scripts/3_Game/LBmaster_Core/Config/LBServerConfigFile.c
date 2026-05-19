class LBServerConfigFile {

	static ref LBServerConfigFile g_LBServerConfigFile;

	static LBServerConfigFile Get() {
		if (!g_LBServerConfigFile)
			g_LBServerConfigFile = new LBServerConfigFile();
		return g_LBServerConfigFile;
	}

	string hostname = "CONFIG FILE NOT FOUND";
	bool hostnameRead = false;
	string configPath;
	bool configExists = false;

	void LBServerConfigFile() {
		configPath = GetConfigPath();
		string value;
		if (FileExist(configPath)) {
			map<string, string> vars = ParseConfig(configPath);
			configExists = true;
			if (vars.Find("hostname", value)) {
				hostnameRead = true;
				hostname = value;
			}
		}
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