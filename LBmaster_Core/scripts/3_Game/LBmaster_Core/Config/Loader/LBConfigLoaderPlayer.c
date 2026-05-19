class LBConfigLoaderPlayer<Class T1> : LBConfigLoaderBase {

	typename TN_T1 = LBStringTools.FixTypename(T1);

	ref map<string, ref T1> cachedForLater = new map<string, ref T1>();

	[LBConfigManager.Get().RegisterConfigLoader(T1)]
	static ref LBConfigLoaderPlayer<T1> Loader;

	ScriptCaller receiveCallback = null;

	void LBConfigLoaderPlayer() {
		Loader = this;
		LBLogger.Verbose("Created Loader: " + this, "Core");
	}

	void ~LBConfigLoaderPlayer() {
		LBLogger.Verbose("Deleted Loader: " + this, "Core");
		Delete();
	}

	static Class SetPriority(LBConfigPriority prio) {
		LBConfigManager.Get().SetConfigPriority(T1, prio);
		return null;
	}

	string GetReceivePermissionOthers() {
		return "";
	}

	override bool IsForceValid() {
		return false;
	}

	override string GetName() {
		return TN_T1.ToString();
	}

	bool KeepCached() {
		return false;
	}

	override void Load() {
		InitVars();
	}

	bool CanLoadFileOf(string filename, PlayerIdentity sender) {
#ifdef SERVER
		string steamid = sender.GetPlainId();
#else
		string steamid = LBAdmins.Get().GetMySteamid();
#endif
		if (filename == steamid + ".json" || filename == steamid)
			return true;
		return GetReceivePermissionOthers() == "" || LBAdmins.Get().HasPermission(GetReceivePermissionOthers(), sender);
	}

	override void InitVarsInternal(string modName, string filename, LBConfigType type = LBConfigType.CONFIG, bool syncedToClients = true, string permissionChange = "", string permissionReceive = "", bool registerToAPI = true) {
		super.InitVarsInternal(modName, GetFilename(), type, syncedToClients, permissionChange, permissionReceive, false);
		if (registerToAPI) {
			LBAPIManager.RegisterRoute("v1/" + GetFilePathNoBaseNoFilename() + "/list", new LBAPIParserVoid(ScriptCaller.Create(APIGetConfigPlayerList)));
			LBAPIManager.RegisterRoute("v1/" + GetFilePathNoBaseNoFilename() + "/get", new LBAPIParser<LBAPIPath>(ScriptCaller.Create(APIGetConfigPlayer)));
			//LBAPIManager.RegisterRoute("v1/" + GetFilePathNoBaseNoFilename() + "/set", new LBAPIParserRaw(ScriptCaller.Create(APISetConfigPlayer)));
		}
	}

	void APIGetConfigPlayerList(LBAPIResponse resp) {
		TStringArray files = new TStringArray();
		ExploreFolder("", files, ".json");
		string content;
		LBAPIParser.m_Serial.WriteToString(files, true, content);
		resp.SetData(content);
		resp.SetOK();
	}

	void ExploreFolder(string folder, TStringArray output, string ending) {
		string fullFolder = GetFileFolder() + "/" + folder;
		string filename;
		FileAttr attr;
		FindFileHandle handle = FindFile(fullFolder + "*", filename, attr, FindFileFlags.ALL);
		if (handle == 0)
			return;
		if ((attr & FileAttr.DIRECTORY) == 0) {
			if (filename != "" && LBStringTools.EndsWith(filename, ending)) {
				output.Insert(folder + filename);
			}
		} else if (filename != "") {
			ExploreFolder(folder + filename + "/", output, ending);
		}
		while (FindNextFile(handle, filename, attr)) {
			if ((attr & FileAttr.DIRECTORY) == 0) {
				if (filename != "" && LBStringTools.EndsWith(filename, ending)) {
					output.Insert(folder + filename);
				}
			} else if (filename != "") {
				ExploreFolder(folder + filename + "/", output, ending);
			}
		}
		CloseFindFile(handle);
	}

	void DeleteAllPlayers() {
		DeletePlayerFiles(".json");
		DeletePlayerFiles(".backup");
		DeletePlayerFiles(".backup2");
		cachedForLater.Clear();
	}

	private void DeletePlayerFiles(string ending) {
		TStringArray files = new TStringArray();
		ExploreFolder("", files, ending);
		foreach (string file : files) {
			DeleteFile(file);
		}
	}

	void RenamePlayer(T1 data, string newName) {
		if (newName.LastIndexOf(".json") != newName.Length() - 5 || newName.Length() < 5)
			newName = newName + ".json";
		_filename = newName;
		string oldFile = data._filename;
		data._filename = _filename;
		if (cachedForLater.Contains(oldFile)) {
			cachedForLater.Remove(oldFile);
			cachedForLater.Insert(_filename, data);
		}
		SavePlayer(data);
		DeletePlayer(oldFile);
	}

	void DeletePlayer(LBPlayer_ player) {
		DeletePlayer(player.GetSteamId());
	}

	void DeletePlayer(string name) {
		string baseFile = GetFileFolder() + "/" + name;
		if (FileExist(baseFile))
			DeleteFile(baseFile);
		if (FileExist(baseFile + ".backup"))
			DeleteFile(baseFile + ".backup");
		if (FileExist(baseFile + ".backup2"))
			DeleteFile(baseFile + ".backup2");
	}

	array<ref T1> LoadAllPlayers() {
		TStringArray files = new TStringArray();
		ExploreFolder("", files, ".json");
		array<ref T1> arr = new array<ref T1>();
		foreach (string file : files) {
			T1 player = LoadPlayer(file);
			if (player)
				arr.Insert(player);
		}
		return arr;
	}

	void APIGetConfigPlayer(LBAPIResponse resp, LBAPIPath path) {
		T1 player = LoadPlayer(path.GetSanitized(), null, true);
		if (!player) {
			resp.Set(404, "Not found");
			return;
		}
		string content;
		LBAPIParser.m_Serial.WriteToString(player, true, content);
		resp.SetData(content);
		resp.SetOK();
	}

	void LoadMyFile(ScriptCaller receiveCallback_ = null) {
		LoadPlayer("", receiveCallback_);
	}

	bool ReloadPlayer(T1 data, ScriptCaller receiveCallback_ = null) {
		this.receiveCallback = receiveCallback_;
		if (!data)
			return false;
		LBLogger.Verbose("Relooading: " + GetName() + " (" + data._filename + ")", "Core");
		InitVars();
		_filename = data._filename;
#ifndef SERVER
		if (!IsClientSideConfig()) {
			RequestFromServer();
			return true;
		}
#else
		if (IsClientSideConfig()) {
			return false;
		}
#endif
		string path = GetFilePath();
		LBLogger.Verbose("Reloading: Filename: " + _filename + " From " + path, "Core");
		if (!LBmasterConfigMover.JsonFileExist(path)) {
			return false;
		} else {
			bool forceValid = IsForceValid();
			bool loadFull;
			if (!data.JsonLoadVar(path, forceValid, loadFull) && !loadFull) {
				return false;
			}
			if (loadFull && !LBJsonLoader<T1>.JsonLoadFile(path, data, forceValid, false, KeepBackups())) {
				return false;
			}
		}
		if (data.version != data.CURRENT_VERSION) {
			LBLogger.Debug("Updating Config " + GetName() + " from Version " + data.version + " to " + data.CURRENT_VERSION, "Core");
			data.UpdateVersion();
			data.version = data.CURRENT_VERSION;
			SavePlayer(data);
		}
		if (data.OnLoad()) {
			LBLogger.Debug("OnLoad triggered Saving of Config " + GetName(), "Core");
			SavePlayer(data);
		}
		data._filename = _filename;
		if (KeepCached())
			cachedForLater.Insert(data._filename, data);
		return true;
	}

	T1 LoadPlayer(LBPlayer_ player, ScriptCaller receiveCallback_ = null, bool nullIfNotFound = false) {
		T1 data = LoadPlayer(player.GetSteamId(), receiveCallback_, nullIfNotFound);
		if (data && player.GetIdentity())
			data.InitIdentity(player.GetIdentity());
		return data;
	}

	T1 LoadPlayer(PlayerIdentity identity, ScriptCaller receiveCallback_ = null, bool nullIfNotFound = false) {
		if (!identity)
			return null;
		T1 player = LoadPlayer(identity.GetPlainId() + ".json", receiveCallback_, nullIfNotFound);
		player.InitIdentity(identity);
		return player;
	}

	T1 LoadPlayer(string name, ScriptCaller receiveCallback_ = null, bool nullIfNotFound = false) {
		this.receiveCallback = receiveCallback_;
		LBLogger.Verbose("Loading: " + GetName() + " (" + name + ")", "Core");
		InitVars();
		if (name.LastIndexOf(".json") != name.Length() - 5 || name.Length() < 5)
			name = name + ".json";
		_filename = name;
#ifndef SERVER
		if (!IsClientSideConfig()) {
			RequestFromServer();
			return null;
		}
#else
		if (IsClientSideConfig()) {
			return null;
		}
#endif
		T1 Get;
		if (IsCLIParam("lbTest") || !WasInitialized()) {
			Get = InitDefault(false);
			Get.OnLoad();
			Get._filename = _filename;
			return Get;
		}
		if (cachedForLater.Find(_filename, Get)) {
			LBLogger.Verbose("Found Cached: " + name + " Filename: " + _filename, "Core");
			return Get;
		}
		Class.CastTo(Get, TN_T1.Spawn());
		Get._filename = _filename;
		string path = GetFilePath();
		LBLogger.Verbose("Loading: " + name + " Filename: " + _filename + " From " + path, "Core");
		if (!LBmasterConfigMover.JsonFileExist(path)) {
			LBmasterConfigMover.CreateParentFolders(path);
			if (nullIfNotFound)
				return null;
			Get = InitDefault();
		} else {
			bool forceValid = IsForceValid();
			bool loadFull;
			if (!Get.JsonLoadVar(path, forceValid, loadFull) && !loadFull) {
				Get = InitDefault();
			}
			if (loadFull && !LBJsonLoader<T1>.JsonLoadFile(path, Get, forceValid, false, KeepBackups())) {
				Get = InitDefault();
			}
		}
		if (Get.version != Get.CURRENT_VERSION) {
			LBLogger.Debug("Updating Config " + GetName() + " from Version " + Get.version + " to " + Get.CURRENT_VERSION, "Core");
			Get.UpdateVersion();
			Get.version = Get.CURRENT_VERSION;
			SavePlayer(Get);
		}
		if (Get.OnLoad()) {
			LBLogger.Debug("OnLoad triggered Saving of Config " + GetName(), "Core");
			SavePlayer(Get);
		}
		Get._filename = _filename;
		if (KeepCached())
			cachedForLater.Insert(Get._filename, Get);
		return Get;
	}

	T1 InitDefault(bool save = true) {
		T1 Get;
		Class.CastTo(Get, TN_T1.Spawn());
		Get._filename = _filename;
		Get.LoadDefault();
		if (save)
			SavePlayer(Get);
		return Get;
	}

	override void InitMission() {}
	override void Delete() {}
	override void Reload() {}

	void SavePlayer(T1 config) {
		if (!config)
			return;
#ifdef SERVER
		if (!WasInitialized())
			return;
#endif
		if (config.saveLater) {
			config.saveLater = false;
			if (!KeepCached())
				cachedForLater.Remove(config._filename);
			LBLogger.Debug("Saving config " + config._filename + " later", "Core");
		}
		_filename = config._filename;
#ifdef SERVER
		if (IsClientSideConfig())
			return;
#else
		if (!IsClientSideConfig()) {
			if (IsSyncedToClient()) {
				config.OnSave();
				SendToClient(null, null, false);
			}
			return;
		}
#endif
		config.OnSave();
		bool saveFull = false;
		config.JsonSaveVar(GetFilePath(), saveFull);
		if (saveFull)
			LBJsonLoader<T1>.JsonSaveFile(GetFilePath(), config, KeepBackups());
	}

	void SavePlayerLater(T1 config, int timeout = 30000) {
		cachedForLater.Insert(config._filename, config);
		if (!config.saveLater) {
			g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SavePlayer, timeout, false, config);
			LBLogger.Debug("Saving file " + config._filename + " with a delay of " + timeout + "ms", "Core");
		}
		config.saveLater = true;
	}

	override void SendToClient(ParamsReadContext ctx, PlayerIdentity target, bool sendConfirmationMessage = true) {
		if (!IsSyncedToClient())
			return;
		if (ctx && !ctx.Read(_filename))
			return;
		if (_filename == ".json")
			_filename = target.GetPlainId() + ".json";

		LBLogger.Verbose("client requested config " + GetName() + " (" + _filename + ") to " + LBLogger.FormatPlayerIdentity(target), "Core");
		if (!CanLoadFileOf(_filename, target))
			return;
		T1 config = LoadPlayer(_filename + "");
		LBLogger.Verbose("sending config " + GetName() + " (" + _filename + ") to " + LBLogger.FormatPlayerIdentity(target), "Core");
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(GetName());
		rpc.Write(_filename);
		if (config.UseCustomRPCReadWrite())
			config.WriteToCtx(ctx);
		else
			rpc.Write(config);
		config.WriteExtraCtx(rpc);
		rpc.Write(sendConfirmationMessage);
		if (!target && GetReceivePermission() != "" && g_Game.IsServer()) {
			LBAdmins.Get().SendRPCToAdminsWithPermission(GetReceivePermission(), rpc, LBmaster_Core_RPCs.CONFIG_SYNC);
		} else {
			rpc.Send(null, LBmaster_Core_RPCs.CONFIG_SYNC, true, target);
		}
	}

	override bool ReadFromCtx(ParamsReadContext ctx, PlayerIdentity sender) {
		if (!ctx.Read(_filename))
			return false;
		if (!CanLoadFileOf(_filename, sender))
			return false;
#ifdef SERVER
		string permission = GetChangePermission();
		if (GetChangePermission() != "" && !LBAdmins.Get().HasPermission(permission, sender)) {
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
#ifdef SERVER
		LBLogger.Info("received config " + GetName() + " (" + _filename + ") from " + LBLogger.FormatPlayerIdentity(sender), "Core");
#else
		LBLogger.Info("received config " + GetName() + " (" + _filename + ") from Server. Callback: " + this.receiveCallback, "Core");
#endif
		newInstance.OnReceivedFromRPC(sender);
		LBConfigManager.Get().OnConfigReceived(TN_T1);
#ifdef SERVER
		SavePlayer(newInstance);
		SendToClient(null, sender);
#endif
		if (this.receiveCallback)
			this.receiveCallback.Invoke(this, newInstance);

		return true;
	}

	private void RequestFromServer() {
		if (!IsSyncedToClient())
			return;
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(GetName());
		rpc.Write(_filename);
		rpc.Send(null, LBmaster_Core_RPCs.CONFIG_REQUEST, true);
		LBLogger.Info("requested config " + GetName() + " (" + _filename + ") from Server", "Core");
	}

}