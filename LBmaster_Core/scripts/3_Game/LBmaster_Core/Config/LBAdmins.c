/*
class LBAdmins : LBConfigLoader<LBAdmins_> {
	override void InitVars() {
		InitVarsInternal("Common", "Admins.json", LBConfigType.CONFIG, true, "core.change.admins"); // (easy - medium)
	}
}
*/
// This file is used to define the admins for all LBmaster mods.
// After adding your Steamid to the file, you can enable Admin mode by pressing `U`
// This enabled some interaction options to configure items like ATMs or Garages
// To access the Admin Menu, you need to have admin mode enabled and press `I`
// These are the default buttons used, which can be changed in the DayZ Controls
class LBAdmins { /*: LBConfigBase*/

	static const string GROUP_OWNER = "Owner";
	static const string GROUP_ADMIN = "Admin";
	static const string GROUP_MODERATOR = "Moderator";
	static const string GROUP_SUPPORT = "Support";
	private static ref LBAdmins g_LBAdmins;
	private static const string CFG_PATH = "$profile:LBmaster/Config/Common/Admins.json";

	const int CURRENT_VERSION = 2;

	private int version = CURRENT_VERSION; // Version for internal Version tracking

	ref array<ref LBAdminsPlayer> admins = new array<ref LBAdminsPlayer>(); // List of Admins. Each admin needs their own Entry (`{... steamid ...}`)
	ref array<ref LBAdminsGroup> groups = new array<ref LBAdminsGroup>(); // List of different Permission Groups / Ranks (Owner / Admin / Moderator / Supporter etc.)
	
	[NonSerialized()]
	ref ScriptInvoker onReceivedCall = new ScriptInvoker();
	[NonSerialized()]
	bool received = false;
	[NonSerialized()]
	private string mySteamid;
	[NonSerialized()]
	private bool active = false;
	[NonSerialized()]
	private ref TStringSet activeSteamids = new TStringSet();
	[NonSerialized()]
	bool needSave = false;
	[NonSerialized()]
	private ref ScriptInvoker activeToggle = new ScriptInvoker();
	[NonSerialized()]
	private ref array<PlayerIdentity> activeAdmins = new array<PlayerIdentity>();
	[NonSerialized()]
	private ref set<string> disabledNamesSteamids = new set<string>();
	[NonSerialized()]
	ref map<string, string> permissionComments = new map<string, string>();
	[NonSerialized()]
	ref set<string> importantPermissions = new set<string>();

	static LBAdmins Get() {
		if (!g_LBAdmins)
			g_LBAdmins = Load();
		return g_LBAdmins;
	}

	static bool Loaded() {
		return g_LBAdmins != null && g_LBAdmins.mySteamid != "";
	}

	static LBAdmins Load() {
		LBAdmins cfg = new LBAdmins();
#ifndef NO_GUI
		return cfg;
#endif
		LBmasterConfigMover.CreateParentFolders(CFG_PATH);
		if (FileExist(CFG_PATH)) {
			string error;
			if (!JsonFileLoader<LBAdmins>.LoadFile(CFG_PATH, cfg, error))
				LBLogger.Fatal("Could not Load Admins file. Error: " + error, "Core");
		} else {
			cfg = LoadDefault();
			cfg.Save();
		}
		cfg.Update();
		cfg.OnLoad();
		return cfg;
	}

	static LBAdmins LoadDefault() {
		LBAdmins cfg = new LBAdmins();
		cfg.InsertAdmin("12312312312312312", "", "Remove Me", true, {GROUP_OWNER});
		cfg.InsertAdmin("SteamiID Here", "", "Another Admin", false, {GROUP_ADMIN});
		cfg.InsertAdmin("I also need a Steamid", "Peter", "And a third one", false, {GROUP_SUPPORT});
		return cfg;
	}

	static void Delete() {
		g_LBAdmins = null;
	}

	void LBAdmins() {
#ifdef SERVER
		LBAPIManager.RegisterRoute("/v1/admins/players/list", new LBAPIParserVoid(ScriptCaller.Create(OnAdminsList), true));
		LBAPIManager.RegisterRoute("/v1/admins/players/listfull", new LBAPIParserVoid(ScriptCaller.Create(OnAdminsListFull), true));
		LBAPIManager.RegisterRoute("/v1/admins/players/get", new LBAPIParserKey(ScriptCaller.Create(OnAdminDetail), true));
		LBAPIManager.RegisterRoute("/v1/admins/players/set", new LBAPIParser<LBAdminsPlayer>(ScriptCaller.Create(OnAdminChange), true));
		LBAPIManager.RegisterRoute("/v1/admins/players/delete", new LBAPIParserKey(ScriptCaller.Create(OnAdminDelete), true));
		LBAPIManager.RegisterRoute("/v1/admins/ranks/list", new LBAPIParserVoid(ScriptCaller.Create(OnRanksList), true));
		LBAPIManager.RegisterRoute("/v1/admins/ranks/listfull", new LBAPIParserVoid(ScriptCaller.Create(OnRanksListFull), true));
		LBAPIManager.RegisterRoute("/v1/admins/ranks/get", new LBAPIParserKey(ScriptCaller.Create(OnRankDetail), true));
#endif
	}

	void EnableAdminModeFor(PlayerIdentity identity) {
		activeSteamids.Insert(identity.GetPlainId());
		activeAdmins.Insert(identity);
	}

	void DisableAdminModeFor(PlayerIdentity identity) {
		int index = activeSteamids.Find(identity.GetPlainId());
		if (index >= 0)
			activeSteamids.Remove(index);
		activeAdmins.RemoveItem(identity);
	}

	void SendToActiveAdmins(ScriptRPC rpc, int type, Object obj = null) {
		foreach (PlayerIdentity ident : activeAdmins) {
			if (ident)
				rpc.Send(obj, type, true, ident);
		}
	}

	bool IsActive(PlayerIdentity identity) {
		if (!identity)
			return IsActive();
		return IsActive(identity.GetPlainId());
	}

	bool IsActive(string steamid = "") {
		return active || (activeSteamids.Find(steamid) != -1 && steamid != "");
	}

	bool IsActive(Man player) {
		if (!player)
			return false;
		return IsActive(player.GetIdentity());
	}

	bool CanEnableAdminMenu(string steamid = "") {
		return HasPermission("core.toggle.adminmode", steamid);
	}

	void ToggleActive() {
		active = !active;
		LBLogger.Debug("Toggle Admin mode active: " + active, "Core");
		activeToggle.Invoke();
		g_Game.RPCSingleParam(null, LBmaster_Core_RPCs.ADMIN_TOGGLE, new Param1<bool>(active), true);
	}

	ScriptInvoker GetActiveToggleEvent() {
		return activeToggle;
	}

	LBAdminsGroup RegisterGroup(string group) {
		LBAdminsGroup set_;
		if (FindGroup(group, set_))
			return set_;
		set_ = new LBAdminsGroup();
		set_.name = group;
		groups.Insert(set_);
		Save();
		return set_;
	}

	private bool FindGroup(string name, out LBAdminsGroup found) {
		foreach (LBAdminsGroup group : groups) {
			if (group && group.name == name) {
				found = group;
				return true;
			}
		}
		return false;
	}

	private void RegisterPermissionInternal(string permission, int level, bool support, bool moderator, bool admin, bool owner, string comment) {
		permissionComments.Insert(permission, comment);
		RegisterPermissionIf(GROUP_OWNER, permission, level, owner);
		RegisterPermissionIf(GROUP_ADMIN, permission, level, admin);
		RegisterPermissionIf(GROUP_MODERATOR, permission, level, moderator);
		RegisterPermissionIf(GROUP_SUPPORT, permission, level, support);
	}

	private void RegisterPermissionIf(string group, string permission, int level, bool checked) {
		if (checked)
			RegisterPermissionInternal(group, permission, level);
		else
			RegisterPermissionInternal(group, permission, 0);
	}

	private void RegisterPermissionInternal(string group, string permission, int level) {
		LBAdminsGroup groupPerms = RegisterGroup(group);
		if (!groupPerms.Contains(permission)) {
			groupPerms.Insert(permission, level);
			needSave = true;
		}
	}

	void RegisterPermission(string permission, string comment) {
		RegisterPermissionInternal(permission, 1, true, true, true, true, comment);
	}

	void RegisterPermission(string permission, bool support, string comment) {
		RegisterPermissionInternal(permission, 1, support, true, true, true, comment);
	}

	void RegisterPermission(string permission, bool support, bool moderator, string comment) {
		RegisterPermissionInternal(permission, 1, support, moderator, true, true, comment);
	}

	void RegisterPermission(string permission, bool support, bool moderator, bool admin, string comment) {
		RegisterPermissionInternal(permission, 1, support, moderator, admin, true, comment);
	}

	void RegisterPermission(string permission, bool support, bool moderator, bool admin, bool owner, string comment) {
		RegisterPermissionInternal(permission, 1, support, moderator, admin, owner, comment);
	}

	void RegisterPermissionImportant(string permission, string comment) {
		RegisterPermissionInternal(permission, 1, true, true, true, true, comment);
		importantPermissions.Insert(permission);
	}

	void RegisterPermissionImportant(string permission, bool support, string comment) {
		RegisterPermissionInternal(permission, 1, support, true, true, true, comment);
		importantPermissions.Insert(permission);
	}

	void RegisterPermissionImportant(string permission, bool support, bool moderator, string comment) {
		RegisterPermissionInternal(permission, 1, support, moderator, true, true, comment);
		importantPermissions.Insert(permission);
	}

	void RegisterPermissionImportant(string permission, bool support, bool moderator, bool admin, string comment) {
		RegisterPermissionInternal(permission, 1, support, moderator, admin, true, comment);
		importantPermissions.Insert(permission);
	}

	void RegisterPermissionImportant(string permission, bool support, bool moderator, bool admin, bool owner, string comment) {
		RegisterPermissionInternal(permission, 1, support, moderator, admin, owner, comment);
		importantPermissions.Insert(permission);
	}

	void InsertAdmin(string steamid, string name, string comment, bool grantAllPermissions, TStringArray permissionGroups = null) {
		LBAdminsPlayer player = new LBAdminsPlayer();
		player.comment = comment;
		player.ingameNameForPermissions = name;
		player.steamid = steamid;
		player.grantAllPermissions = grantAllPermissions;
		if (permissionGroups) {
			foreach (string str : permissionGroups)
				player.permissionGroups.Insert(str);
		}
		player.InitGrantedPermissions(groups);
		admins.Insert(player);
	}

	void OnLoad() {
		foreach (LBAdminsPlayer player : admins) {
			player.InitGrantedPermissions(groups);
		}
	}

	void OnAdminChange(LBAPIResponse resp, LBAdminsPlayer player) {
		LBAdminsPlayer found;
		if (!FindPlayer(player.steamid, found)) {
			found = player;
			admins.Insert(player);
		}
		found.ingameNameForPermissions = player.ingameNameForPermissions;
		found.comment = player.comment;
		found.grantAllPermissions = player.grantAllPermissions;
		found.permissionGroups = player.permissionGroups;
		found.InitGrantedPermissions(groups);
		string content;
		LBAPIParser.m_Serial.WriteToString(found, true, content);
		resp.SetData(content);
		resp.SetOK();
		Save();
	}

	void OnAdminsList(LBAPIResponse resp) {
		TStringArray steamids = new TStringArray();
		foreach (LBAdminsPlayer player : admins) {
			steamids.Insert(player.steamid);
		}
		string content;
		LBAPIParser.m_Serial.WriteToString(steamids, true, content);
		resp.SetData(content);
		resp.SetOK();
	}

	void OnAdminsListFull(LBAPIResponse resp) {
		string content;
		LBAPIParser.m_Serial.WriteToString(admins, true, content);
		resp.SetData(content);
		resp.SetOK();
	}

	void OnAdminDetail(LBAPIResponse resp, LBKeyValue key) {
		resp.SetNotFound();
		foreach (LBAdminsPlayer player : admins) {
			if (player.steamid == key.key) {
				string content;
				LBAPIParser.m_Serial.WriteToString(player, true, content);
				resp.SetData(content);
				resp.SetOK();
			}
		}
	}

	void OnAdminDelete(LBAPIResponse resp, LBKeyValue key) {
		resp.SetNotFound();
		for (int i = 0; i < admins.Count(); ++i) {
			LBAdminsPlayer player = admins.Get(i);
			if (player.steamid == key.key) {
				resp.SetOK();
				admins.Remove(i);
				--i;
			}
		}
		Save();
	}

	void OnRanksList(LBAPIResponse resp) {
		TStringArray ranks = new TStringArray();
		foreach (LBAdminsGroup rank : groups) {
			ranks.Insert(rank.name);
		}
		string content;
		LBAPIParser.m_Serial.WriteToString(ranks, true, content);
		resp.SetData(content);
		resp.SetOK();
	}

	void OnRanksListFull(LBAPIResponse resp) {
		string content;
		LBAPIParser.m_Serial.WriteToString(groups, true, content);
		resp.SetData(content);
		resp.SetOK();
	}

	void OnRankDetail(LBAPIResponse resp, LBKeyValue key) {
		resp.SetNotFound();
		foreach (LBAdminsGroup rank : groups) {
			if (rank.name == key.key) {
				string content;
				LBAPIParser.m_Serial.WriteToString(rank, true, content);
				resp.SetData(content);
				resp.SetOK();
			}
		}
	}

	void SendAll() {
		array<PlayerIdentity> identities = new array<PlayerIdentity>();
		g_Game.GetPlayerIndentities(identities);
		foreach (PlayerIdentity ident : identities) {
			if (!ident)
				continue;
			ScriptRPC rpc = new ScriptRPC();
			WriteToCtx(rpc, ident);
			rpc.Send(null, LBmaster_Core_RPCs.ADMIN_SYNC, true, ident);
		}
	}

	void Update() {
		if (CURRENT_VERSION == version)
			return;

		version = CURRENT_VERSION;
		Save();
	}

	void OnRegisterFinished() {
		if (needSave)
			Save();
	}

	void Save() {
#ifdef NO_GUI
		string error;
		if (!JsonFileLoader<LBAdmins>.SaveFile(CFG_PATH, this, error))
			LBLogger.Fatal("Could not save Admins.json! Error: " + error, "Core");
		needSave = false;
		OnLoad();
#endif
	}

	bool LoadFromCtx(ParamsReadContext ctx) {
		if (!g_Game.IsClient())
			return false;
		int count = 0;
		if (!ctx.Read(mySteamid))
			return false;
		if (!ctx.Read(count))
			return false;
		LBLogger.Verbose("Trying to read " + count + " Admin entries from Server. My Steamid: " + mySteamid, "Core");
		admins.Clear();
		for (int i = 0; i < count; i++) {
			LBAdminsPlayer player = new LBAdminsPlayer();
			if (!player.ReadFromCtx(ctx))
				return false;
			admins.Insert(player);
		}
		received = true;
		if (onReceivedCall)
			onReceivedCall.Invoke();
		onReceivedCall = new ScriptInvoker();
		return true;
	}

	void WriteToCtx(ParamsWriteContext ctx, PlayerIdentity requestor) {
		int count = 0;
		string steamid = "";
		if (!requestor) {
			ctx.Write(steamid);
			ctx.Write(count);
			return;
		}
		LBAdminsPlayer player = null;
		steamid = requestor.GetPlainId();
		ctx.Write(steamid);
		if (!FindPlayer(steamid, player)) {
			ctx.Write(count);
			return;
		}
		string name = requestor.GetName();
		if (player.ingameNameForPermissions == "" || player.ingameNameForPermissions == name) {
			disabledNamesSteamids.RemoveItem(steamid);
			count = 1;
			ctx.Write(count);
			player.WriteToCtx(ctx);
		} else {
			disabledNamesSteamids.Insert(steamid);
			ctx.Write(count);
		}
	}

	bool FindPlayer(string steamid, out LBAdminsPlayer found) {
		foreach (LBAdminsPlayer player : admins) {
			if (player && player.steamid == steamid) {
				found = player;
				return true;
			}
		}
		return false;
	}

	bool HasPermissionActive(string permission, Man player, bool cannotUseGrantAllPermissions = false) {
		return IsActive(player) && HasPermission(permission, player, cannotUseGrantAllPermissions);
	}

	bool HasPermissionActive(string permission, PlayerIdentity player, bool cannotUseGrantAllPermissions = false) {
		return IsActive(player) && HasPermission(permission, player, cannotUseGrantAllPermissions);
	}

	bool HasPermissionActive(string permission, string steamid = "", bool cannotUseGrantAllPermissions = false) {
		return IsActive(steamid) && HasPermission(permission, steamid, cannotUseGrantAllPermissions);
	}

	bool HasPermission(string permission, Man player, bool cannotUseGrantAllPermissions = false) {
#ifdef SERVER
		if (player)
			return HasPermission(permission, player.GetIdentity(), cannotUseGrantAllPermissions);
#endif
		return HasPermission(permission, "", cannotUseGrantAllPermissions);

	}

	bool HasPermission(string permission, PlayerIdentity player, bool cannotUseGrantAllPermissions = false) {
#ifdef SERVER
		if (player)
			return HasPermission(permission, player.GetPlainId(), cannotUseGrantAllPermissions);
#endif
		return HasPermission(permission, "", cannotUseGrantAllPermissions);
	}

	bool HasPermission(string permission, string steamid = "", bool cannotUseGrantAllPermissions = false) {
		LBAdminsPlayer player = null;
		if (!GetPermissionPlayer(player, steamid))
			return false;
		if (disabledNamesSteamids.Find(steamid) != -1)
			return false;
		return player.HasPermission(permission, cannotUseGrantAllPermissions);
	}

	bool GetPermissionPlayer(out LBAdminsPlayer player, string steamid) {
		if (!GetSteamid(steamid))
			return false;
		if (!FindPlayer(steamid, player))
			return false;
		return true;
	}

	bool IsAllPermissionsGranted(string steamid = "") {
		LBAdminsPlayer player = null;
		if (!GetPermissionPlayer(player, steamid))
			return false;
		return player.grantAllPermissions;
	}

	bool OverwriteValue(string permission, out int changed, int value, string steamid = "") {
		if (HasPermission(permission, steamid)) {
			changed = value;
			return true;
		}
		return false;
	}

	bool OverwriteValue(string permission, out string changed, string value, string steamid = "") {
		if (HasPermission(permission, steamid)) {
			changed = value;
			return true;
		}
		return false;
	}

	TStringSet FindAdminsWithPermission(string permission) {
		TStringSet arr = new TStringSet();
		foreach (LBAdminsPlayer player : admins) {
			if (player.HasPermission(permission))
				arr.Insert(player.steamid);
		}
		return arr;
	}

	array<LBAdminsPlayer> FindAdminEntriesWithPermission(string permission) {
		array<LBAdminsPlayer> arr = new array<LBAdminsPlayer>();
		foreach (LBAdminsPlayer player : admins) {
			if (player.HasPermission(permission))
				arr.Insert(player);
		}
		return arr;
	}

	array<PlayerIdentity> FindAdminsWithPermissionOnline(string permission) {
		TStringSet adminsWithPerm = FindAdminsWithPermission(permission);
		array<PlayerIdentity> players = new array<PlayerIdentity>();
		g_Game.GetPlayerIndentities(players);
		array<PlayerIdentity> list = new array<PlayerIdentity>();
		foreach (PlayerIdentity ident : players) {
			if (adminsWithPerm.Find(ident.GetPlainId()) != -1)
				list.Insert(ident);
		}
		return list;
	}

	void SendRPCToAdminsWithPermission(string permission, ScriptRPC rpc, int type) {
		array<PlayerIdentity> list = FindAdminsWithPermissionOnline(permission);
		foreach (PlayerIdentity online : list) {
			if (!online)
				continue;
			rpc.Send(null, type, true, online);
		}
	}

	private bool GetSteamid(out string steamid) {
		if (steamid == "" || g_Game.IsClient()) {
			if (g_Game.IsServer()) {
				LBLogger.Error("Cannot Check permissions in offline mode or without player Steamid !", "Core");
				Error("Cannot Check permissions in offline mode or without player Steamid !");
				return false;
			}
			steamid = GetMySteamid();
		}
		return true;
	}

	string GetMySteamid() {
		return mySteamid;
	}

}
