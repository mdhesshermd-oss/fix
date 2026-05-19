class LBConfigLoaderBase {

	private string _modName, _filename, _permissionChange = "", _permissionReceive = "";
	private LBConfigType _type = LBConfigType.CONFIG;
	private bool _syncedToClients = true;
	private bool wasInitialized = false;

	void Load();
	bool LoadFromJson(string json, out string error);
	void InitMission();
	void Reload();
	bool ReloadFromDisk(out string error);
	void Delete();
	void SaveNoSync();
	void Save(bool sendConfirmationMessage = true);
	string SaveToString();
	void SendToClient(ParamsReadContext ctx, PlayerIdentity target, bool sendConfirmationMessage = true);
	void SyncEveryone(bool sendConfirmationMessage = false);
	bool ReadFromCtx(ParamsReadContext ctx, PlayerIdentity sender);
	void PrepareRPC();
	string GetName();
	string GetRequiredVersion() {
		return "";
	}
	bool KeepBackups() {
		return false;
	}

	void InitVars() {
		InitVarsInternal(GetModName(), GetFilename(), GetType(), IsSyncedToClient(), GetChangePermission(), GetReceivePermission(), false);
	}

	void InitVarsInternal(string modName, string filename, LBConfigType type = LBConfigType.CONFIG, bool syncedToClients = true, string permissionChange = "", string permissionReceive = "", bool registerToAPI = true) {
		this._modName = modName;
		this._filename = filename;
		this._permissionChange = permissionChange;
		this._permissionReceive = permissionReceive;
		this._type = type;
		this._syncedToClients = syncedToClients;
		wasInitialized = IsVersionInstalled(GetRequiredVersion());
	}

	static bool IsVersionInstalled(string name) {
#ifdef DIAG
		return true;
#endif
		if (name != "") {
#ifndef SERVER
			return true;
#else
			if (g_Game.ConfigIsExisting("LBVersions " + name + "_Server"))
				return true;
#endif
		} else {
			return true;
		}
		return false;
	}

	bool WasInitialized() {
		return wasInitialized;
	}

	LBConfigType GetType() {
		return _type;
	}
	string GetModName() {
		return _modName;
	}
	string GetBaseFolder() {
		return "$profile:LBmaster/";
	}
	void DeleteConfigFile() {
		if (GetDeletedFileFolder() != "") {
			LBmasterConfigMover.MoveFileAndBackups(GetFilePath(), GetDeletedFileFolder());
		}
		LBmasterConfigMover.DeleteFileAndBackups(GetFilePath());
	}
	string GetDeletedFileFolder() {
		return "";
	}
	string GetFilename() {
		return _filename;
	}
	string GetOldConfigPath() {
		return "";
	}
	string GetChangePermission() {
		return _permissionChange;
	}
	string GetReceivePermission() {
		return _permissionReceive;
	}
	bool IsForceValid() {
		return false;
	}
	bool IsSyncedOnJoin() {
		return true;
	}
	bool IsSyncedToClient() {
		return _syncedToClients;
	}
	bool IsClientSideConfig() {
		return false;
	}
	string GetFilePathNoBaseNoFilename() {
		string path = "";
		if (GetType() == LBConfigType.CONFIG)
			path += "Config/";
		else
			path += "Data/";
		path += GetModName();
		return path;
	}
	string GetFilePathNoBase() {
		string path = "";
		if (GetType() == LBConfigType.CONFIG)
			path += "Config/";
		else
			path += "Data/";
		path += GetModName() + "/" + GetFilename();
		return path;
	}

	string GetFilePath() {
		string path = GetBaseFolder();
		if (GetType() == LBConfigType.CONFIG)
			path += "Config/";
		else
			path += "Data/";
		path += GetModName() + "/" + GetFilename();
		return path;
	}

	string GetFileFolder() {
		string path = GetBaseFolder();
		if (GetType() == LBConfigType.CONFIG)
			path += "Config/";
		else
			path += "Data/";
		path += GetModName();
		return path;
	}

}