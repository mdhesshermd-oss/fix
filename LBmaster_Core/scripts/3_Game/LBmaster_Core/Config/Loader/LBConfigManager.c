class LBConfigManager {

	private static ref LBConfigManager g_LBConfigManager;
	private static ref map<LBConfigPriority, ref set<string>> loaders = new map<LBConfigPriority, ref set<string>>();
	private static ref map<string, LBConfigPriority> priorities = new map<string, LBConfigPriority>();
	private ref ScriptInvoker Event_OnConfigReceived = new ScriptInvoker();
	private ref map<string, ref ScriptInvoker> onReceiveInvokers = new map<string, ref ScriptInvoker>();
	private static ref set<string> preloaded = new set<string>();
	private static bool loadedAllConfigs = false, missionInitialized = false;
	static ref map<string, LBConfigLoaderBase> pathLoaders = new map<string, LBConfigLoaderBase>();

	static LBConfigManager Get() {
		if (!g_LBConfigManager)
			g_LBConfigManager = new LBConfigManager();
		return g_LBConfigManager;
	}

	static void Delete() {
		g_LBConfigManager = null;
		loadedAllConfigs = false;
		missionInitialized = false;
	}

	private ref map<string, LBConfigLoaderBase> types = new map<string, LBConfigLoaderBase>();

	void ~LBConfigManager() {
		if (!types)
			return;
		foreach (string name_, LBConfigLoaderBase type : types) {
			if (type)
				type.Delete();
		}
		types.Clear();
		LBAPIManager.UnregisterRoute("v1/Config/list");
	}

	void LBConfigManager() {
		for (int prio = LBConfigPriority.LOWEST; prio <= LBConfigPriority.ULTRA_HIGH; prio++) {
			loaders.Insert(prio, new set<string>());
		}
		LBAPIManager.RegisterRoute("v1/Config/list", new LBAPIParserVoid(ScriptCaller.Create(APIListConfigs)));
	}

	void APIListConfigs(LBAPIResponse resp) {
		TStringArray arr = new TStringArray();
		foreach (string path, LBConfigLoaderBase loader : pathLoaders)
			arr.Insert(path);
		string content;
		LBAPIParser.m_Serial.WriteToString(arr, false, content);
		resp.SetData(content);
		resp.SetOK();
	}

	static LBConfigLoaderBase GetLoaderByPath(string path) {
		return pathLoaders.Get(path);
	}

	ScriptInvoker GetEventOnConfigReceived() {
		return Event_OnConfigReceived;
	}

	ScriptInvoker GetEventOnConfigReceived(string configName) {
		ScriptInvoker other;
		if (onReceiveInvokers.Find(configName, other))
			return other;
		other = new ScriptInvoker();
		onReceiveInvokers.Insert(configName, other);
		return other;
	}

	void OnConfigReceived(typename name) {
		string configName = GetLoaderTypename(name.ToString());
		Event_OnConfigReceived.Invoke(configName);
		ScriptInvoker other;
		if (onReceiveInvokers.Find(configName, other))
			other.Invoke();
	}

	void LoadImmediate(string configName) {
		if (loadedAllConfigs)
			return;
		string loader = GetLoaderTypename(configName);
		if (preloaded.Find(loader) != -1 || !priorities.Contains(loader) || types.Contains(configName))
			return;
		Load(loader);
		preloaded.Insert(loader);
	}

	LBConfigManager RegisterConfigLoader(typename configName) {
		string loaderTypename = GetLoaderTypename(configName.ToString());
		loaders.Get(LBConfigPriority.NORMAL).Insert(loaderTypename);
		priorities.Insert(loaderTypename, LBConfigPriority.NORMAL);
		LBLogger.Verbose("Registering config: " + configName.ToString() + ". Loader Class: " + loaderTypename, "Core");
		return null;
	}

	string GetLoaderTypename(string configName) {
		return configName.Substring(0, configName.Length() - 1);
	}

	void SetConfigPriority(typename configName, LBConfigPriority newPrio) {
		string loaderName = GetLoaderTypename(configName.ToString());
		LBConfigPriority old = priorities.Get(loaderName);
		if (old == newPrio)
			return;
		priorities.Set(loaderName, newPrio);
		loaders.Get(old).RemoveItem(loaderName);
		loaders.Get(newPrio).Insert(loaderName);
		LBLogger.Verbose("Changed Priority of " + loaderName + " from " + EnumTools.EnumToString(LBConfigPriority, old) + " to " + EnumTools.EnumToString(LBConfigPriority, newPrio), "Core");
	}

	LBConfigLoaderBase GetLoaderByName(string name) {
		foreach (string name_, LBConfigLoaderBase type : types) {
			if (name == name_)
				return type;
		}
		return null;
	}

	void LoadAllConfigs() {
		for (int prio = LBConfigPriority.ULTRA_HIGH; prio >= LBConfigPriority.LOWEST; prio--) {
			LoadConfigsOf(prio);
		}
		loadedAllConfigs = true;
		preloaded.Clear();
		LBAdmins.Get().OnRegisterFinished();
		LBLogger.Verbose("Finished Load All Configs", "Core");
	}

	void LoadConfigsOf(LBConfigPriority prio) {
		set<string> loaderList = loaders.Get(prio);
		LBLogger.Verbose("Loading config: " + loaderList.Count() + " configs with Priority: " + EnumTools.EnumToString(LBConfigPriority, prio), "Core");
		foreach (string loader : loaderList) {
			LBConfigLoaderBase initLoader;
			if (types.Find(loader + "_", initLoader)) {
				if (missionInitialized)
					initLoader.InitMission();
				continue;
			}
			Load(loader);
		}
	}

	private void Load(string loader) {
		if (!loader)
			return;
		typename loaderTn = loader.ToType();
		if (!loaderTn)
			return;
		LBConfigPriority prio = priorities.Get(loader);
		LBConfigLoaderBase loaderInstance = LBConfigLoaderBase.Cast(loaderTn.Spawn());
		if (loaderInstance == null) {
			LBLogger.Fatal("Tried to create Config Loader for " + loader + ", but Class does not inherit the LBConfigLoaderBase Class !", "Core");
			return;
		}
		LBLogger.Verbose("Loading config: " + loader + " Instance: " + loaderInstance + " Priority: " + EnumTools.EnumToString(LBConfigPriority, prio), "Core");
		loaderInstance.Load();
		if (!loaderInstance.WasInitialized()) {
			LBLogger.Error("Uninitialized config " + loader + " this config is not saved or loaded to prevent damaging files!", "Core");
		}
		if ("Config//" == loaderInstance.GetFilePathNoBase()) {
			LBLogger.Error("Loader " + loaderInstance + " has no path", "Core");
		}
		pathLoaders.Insert(loaderInstance.GetFilePathNoBase(), loaderInstance);
		LBLogger.Verbose("Finished loading of " + loader, "Core");
		types.Set(loader + "_", loaderInstance);
	}

	void OnMissionInit() {
		foreach (string name, LBConfigLoaderBase loader : types) {
			if (!loader)
				Error("Loader for " + name + " no longer exists!");
			loader.InitMission();
		}
		missionInitialized = true;
	}

}