class LBConfigLoader<Class T1> : LBConfigLoaderBase {

	typename TN_T1 = LBStringTools.FixTypename(T1);

	[LBConfigManager.Get().RegisterConfigLoader(T1)]
	static ref T1 Get;
	static ref LBConfigLoaderBase Loader;
	static ref ScriptRPC lastRPC;

	void LBConfigLoader() {
		Loader = this;
		LBLogger.Verbose("Created Loader: " + this, "Core");
	}

	void ~LBConfigLoader() {
		LBLogger.Verbose("Deleted Loader: " + this, "Core");
		Delete();
	}

	static Class SetPriority(LBConfigPriority prio) {
		LBConfigManager.Get().SetConfigPriority(T1, prio);
		return null;
	}

	override void InitVarsInternal(string modName, string filename, LBConfigType type = LBConfigType.CONFIG, bool syncedToClients = true, string permissionChange = "", string permissionReceive = "", bool registerToAPI = true) {
		super.InitVarsInternal(modName, filename, type, syncedToClients, permissionChange, permissionReceive, registerToAPI);
		if (registerToAPI) {
			LBAPIManager.RegisterRoute("v1/" + GetFilePathNoBase() + "/get", new LBAPIParserVoid(ScriptCaller.Create(APIGetConfig)));
			LBAPIManager.RegisterRoute("v1/" + GetFilePathNoBase() + "/set", new LBAPIParserRaw(ScriptCaller.Create(APISetConfig)));
		}
	}

	void APISetConfig(LBAPIResponse resp, string content) {
		LBLogger.Debug("Content Set: " + content + " for " + this, "Core");
		bool loaded = false;
		string error;
		if (LoadFromJson(content, error)) {
			Save();
			resp.SetOK();
		} else {
			LBLogger.Error("Error loading Json from API: " + error, "Core");
			resp.Set(501, error);
		}
	}

	void APIGetConfig(LBAPIResponse resp) {
		resp.SetData(SaveToString());
		resp.SetOK();
	}

	override string GetName() {
		return TN_T1.ToString();
	}

	override void Load() {
		LBLogger.Verbose("Loading: " + GetName(), "Core");
		InitVars();
#ifndef SERVER
		if (!IsClientSideConfig()) {
			RequestFromServer();
			return;
		}
#else
		if (IsClientSideConfig()) {
			return;
		}
#endif
		Class.CastTo(Get, TN_T1.Spawn());
		if (IsCLIParam("lbTest") || !WasInitialized()) {
			InitDefault(false);
			Get.OnLoad();
			return;
		}
		string path = GetFilePath();
		string oldPath = GetOldConfigPath();
		if (oldPath != "" && !FileExist(path) && FileExist(oldPath)) {
			LBmasterConfigMover.MoveFileAndBackups(oldPath, LBmasterConfigMover.GetParentFolder(path));
		}
		if (!LBmasterConfigMover.JsonFileExist(path)) {
			LBmasterConfigMover.CreateParentFolders(path);
			InitDefault();
		} else {
			bool forceValid = IsForceValid();
			bool loadFull = false;
			if (!Get.JsonLoadVar(path, forceValid, loadFull) && !loadFull) {
				InitDefault();
			}
			if (loadFull && !LBJsonLoader<T1>.JsonLoadFile(path, Get, forceValid, false, KeepBackups())) {
				InitDefault();
			}
		}
		if (Get.version != Get.CURRENT_VERSION) {
			LBLogger.Debug("Updating Config " + GetName() + " from Version " + Get.version + " to " + Get.CURRENT_VERSION, "Core");
			Get.UpdateVersion();
			Get.version = Get.CURRENT_VERSION;
			Save();
		}
		if (Get.OnLoad()) {
			LBLogger.Debug("OnLoad triggered Saving of Config " + GetName(), "Core");
			Save();
		}
		lastRPC = null;
	}

	void InitDefault(bool save = true) {
		Class.CastTo(Get, TN_T1.Spawn());
		Get.version = Get.CURRENT_VERSION;
		Get.LoadDefault();
		if (save)
			Save();
	}

	override bool ReloadFromDisk(out string error) {
		string path = GetFilePath();
		if (!FileExist(path)) {
			Load();
			return true;
		}
		string content;
		if (!LBJsonLoader<T1>.ReadContent(path, false, false, content)) {
			error = "Could not open " + path + " to read from!";
			return false;
		}
		if (TryLoadFromFile(path, error)) {
			Get.OnReloadedFromDisk();
			return true;
		}
		return false;
	}

	override bool LoadFromJson(string json, out string error) {
		string tempPath = "$profile:LBmaster/temp.txt";
		FileHandle handle = OpenFile(tempPath, FileMode.WRITE);
		if (handle == 0) {
			LBLogger.Error("Could not open file handle for " + tempPath, "Core");
			return false;
		}
		FPrint(handle, json);
		CloseFile(handle);
		bool ok = TryLoadFromFile(tempPath, error);
		DeleteFile(tempPath);
		return ok;
	}

	bool TryLoadFromFile(string path, out string error) {
		if (!WasInitialized()) {
			error = "Missing Server Mod!";
			return false;
		}
		T1 instance;
		Class.CastTo(instance, TN_T1.Spawn());
		bool loadFull = false;
		if (!instance.JsonLoadVar(path, false, loadFull) && !loadFull) {
			error = lb_last_JsonLoaderError;
			return false;
		}
		if (loadFull && !LBJsonLoader<T1>.JsonLoadFile(path, instance, false, false, KeepBackups())) {
			error = lb_last_JsonLoaderError;
			return false;
		}
		error = "";
		Get = instance;
		if (Get.version != Get.CURRENT_VERSION) {
			LBLogger.Debug("Updating Config " + GetName() + " from Version " + Get.version + " to " + Get.CURRENT_VERSION, "Core");
			Get.UpdateVersion();
			Get.version = Get.CURRENT_VERSION;
			Save();
		}
		Get.OnLoad();
		lastRPC = null;
		return true;
	}

	override void InitMission() {
		if (Get && Get.OnLoadMission()) {
			LBLogger.Debug("OnLoad triggered Saving of Config " + GetName(), "Core");
			Save();
		}
	}

	override void Delete() {
		Get = null;
	}

	override void Reload() {
		Load();
	}

	override void SyncEveryone(bool sendConfirmationMessage = false) {
#ifdef SERVER
		if (IsClientSideConfig())
			return;
		SendToClient(null, null);
#else
		if (!IsClientSideConfig()) {
			if (IsSyncedToClient())
				SendToClient(null, null, sendConfirmationMessage);
			return;
		}
#endif
	}

	override void SaveNoSync() {
		bool enabled = _syncedToClients;
		_syncedToClients = false;
		Save();
		_syncedToClients = enabled;
	}

	override void Save(bool sendConfirmationMessage = true) {
		Get.OnSave();
		lastRPC = null;
		SyncEveryone(sendConfirmationMessage);
#ifdef SERVER
		if (!WasInitialized())
			return;
#endif
		int start = TickCount(0);
		bool saveFull = false;
		Get.JsonSaveVar(GetFilePath(), saveFull);
		if (saveFull)
			LBJsonLoader<T1>.JsonSaveFile(GetFilePath(), Get, KeepBackups());
		LBLogger.Debug("Took " + (TickCount(start) / 10 / 1000.0) + "ms to save " + GetFilePath(), "Core");
	}

	override string SaveToString() {
		Get.OnSave();
		string path = GetBaseFolder() + "temp.txt";
		bool saveFull = false;
		Get.JsonSaveVar(path, saveFull);
		if (saveFull)
			LBJsonLoader<T1>.JsonSaveFile(path, Get, false);
		string content;
		LBJsonLoader<T1>.ReadContent(path, false, true, content);
		return content;
	}

	override void SendToClient(ParamsReadContext ctx, PlayerIdentity target, bool sendConfirmationMessage = true) {
		if (!IsSyncedToClient())
			return;
		int start = TickCount(0);
		if (lastRPC == null) {
			PrepareRPC();
		} else {
			LBLogger.Debug("Using cached RPC data for " + TN_T1.ToString(), "Core");
		}
		int rpc_type = LBmaster_Core_RPCs.CONFIG_SYNC;
		if (sendConfirmationMessage)
			rpc_type = LBmaster_Core_RPCs.CONFIG_SYNC_CONFIRM;
		if (!target && GetReceivePermission() != "" && g_Game.IsServer()) {
			LBAdmins.Get().SendRPCToAdminsWithPermission(GetReceivePermission(), lastRPC, rpc_type);
		} else if (!target || g_Game.IsClient() || GetReceivePermission() == "" || LBAdmins.Get().HasPermission(GetReceivePermission(), target)) {
			lastRPC.Send(null, rpc_type, true, target);
		} else {
			return;
		}
		LBLogger.Verbose("sending config " + GetName() + " to " + LBLogger.FormatPlayerIdentity(target) + " Took: " + (TickCount(start) / 10000) + "ms", "Core");
	}

	override void PrepareRPC() {
		LBLogger.Debug("Generating RPC for config " + TN_T1.ToString(), "Core");
		lastRPC = new ScriptRPC();
		lastRPC.Write(GetName());
		if (Get.UseCustomRPCReadWrite())
			Get.WriteToCtx(lastRPC);
		else
			lastRPC.Write(Get);
		Get.WriteExtraCtx(lastRPC);
	}

	override bool ReadFromCtx(ParamsReadContext ctx, PlayerIdentity sender) {
		int start = TickCount(0);
		lastRPC = null;
#ifdef SERVER
		string permission = GetChangePermission();
		if (!LBAdmins.Get().HasPermission(permission, sender)) {
			LBLogger.Error("Player " + LBLogger.FormatPlayerIdentity(sender) + " tried to update Config " + TN_T1.ToString(), "Core");
			return false;
		}
#endif

		T1 newInstance;
		Class.CastTo(newInstance, TN_T1.Spawn());
		if (newInstance.UseCustomRPCReadWrite()) {
			if (!newInstance.ReadFromCtx(ctx)) {
#ifdef SERVER
				LBLogger.Error("Could not read config " + GetName() + " from " + LBLogger.FormatPlayerIdentity(sender), "Core");
#else
				LBLogger.Error("Could not read config " + GetName() + " from Server", "Core");
#endif
				return false;
			}
		} else {
			if (!ctx.Read(newInstance)) {
#ifdef SERVER
				LBLogger.Error("Could not read config " + GetName() + " from " + LBLogger.FormatPlayerIdentity(sender), "Core");
#else
				LBLogger.Error("Could not read config " + GetName() + " from Server", "Core");
#endif
				return false;
			}
		}
		if (!newInstance.ReadExtraCtx(ctx)) {
#ifdef SERVER
			LBLogger.Error("Could not read config " + GetName() + " from " + LBLogger.FormatPlayerIdentity(sender), "Core");
#else
			LBLogger.Error("Could not read config " + GetName() + " from Server", "Core");
#endif
			return false;
		}
		Get = newInstance;
		Get.OnReceivedFromRPC(sender);
		LBConfigManager.Get().OnConfigReceived(TN_T1);
#ifdef SERVER
		Save();
		LBAdminAuditLog.Get.LogAction(sender, "ConfigSave", "Config " + GetName() + " was saved");
		LBLogger.Info("received config " + GetName() + " from " + LBLogger.FormatPlayerIdentity(sender) + " in " + (TickCount(start) / 10000) + "ms", "Core");
#else
		LBLogger.Info("received config " + GetName() + " from Server in " + (TickCount(start) / 10000) + "ms", "Core");
#endif
		return true;
	}

	private void RequestFromServer() {
		if (!IsSyncedToClient())
			return;
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(GetName());
		rpc.Send(null, LBmaster_Core_RPCs.CONFIG_REQUEST, true);
		LBLogger.Info("requested config " + GetName() + " from Server", "Core");
	}

}
string GetLBMessageInfoIcon() {
	return LBIconConfig.Get.info;
}