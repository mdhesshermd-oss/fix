class LBConfigBase : Managed {

	void UpdateVersion();
	void LoadDefault();
	bool JsonLoadVar(string path, bool forceValid, out bool overwriteTest) {
		overwriteTest = true;
		return true;
	}
	void JsonSaveVar(string path, out bool overwriteTest) {
		overwriteTest = true;
	}
	bool OnLoad() {
		return false; // Return if the config should be saved after invoking the method
	}
	void OnSave() {} // Always called right before a config is saved
	bool OnLoadMission() {
		return false; // Return if the config should be saved after invoking the method
	}
	void OnReceivedFromRPC(PlayerIdentity sender);
	void OnReloadedFromDisk();

	void WriteToCtx(ParamsWriteContext ctx) {}
	bool ReadFromCtx(ParamsReadContext ctx) {}
	bool UseCustomRPCReadWrite() {
		return false;
	}

	void WriteExtraCtx(ParamsWriteContext ctx) {}
	bool ReadExtraCtx(ParamsReadContext ctx) {
		return true;
	}

	int version = 0;
}