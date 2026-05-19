ref set<string> g_LB_RegisteredRPCHandlers = new set<string>();

Class RegisterLBRPCHandler(string classname, LBRPCHandlerType type) {

#ifdef SERVER // Set when Server
	if (type == LBRPCHandlerType.CLIENT)
		return null;
#else
	if (type == LBRPCHandlerType.SERVER)
		return null;
#endif

	g_LB_RegisteredRPCHandlers.Insert(classname);
	return null;
}
class LB_RPCHandler_Base {

	private ref map<int, ref LB_RPCHandler_Config> registeredRpcs = new map<int, ref LB_RPCHandler_Config>();

	protected PlayerIdentity sender;
	protected string steamid;
	protected Object target;
	protected int rpc_type;
	protected ParamsReadContext ctx;
	protected Man man;
	protected ref ScriptRPC rpc = new ScriptRPC();

	string GetSenderLog() {
		return LBLogger.FormatPlayerIdentity(sender);
	}

	void Send(int type, Object targetObj = null) {
		rpc.Send(targetObj, type, true, sender);
	}

	void SendAll(int type, Object targetObj = null) {
		rpc.Send(targetObj, type, true, null);
	}

	void SendAdmins(string permission, int type, Object targetObj = null) {
		array<PlayerIdentity> admins = LBAdmins.Get().FindAdminsWithPermissionOnline(permission);
		foreach (PlayerIdentity admin : admins)
			rpc.Send(targetObj, type, true, admin);
	}

	void RespondAdmins(string permission) {
		array<PlayerIdentity> admins = LBAdmins.Get().FindAdminsWithPermissionOnline(permission);
		foreach (PlayerIdentity admin : admins)
			rpc.Send(target, rpc_type, true, admin);
	}

	void Respond() {
		rpc.Send(target, rpc_type, true, sender);
	}

	void RegisterRPC(int rpc_type_1, int rpc_type_2, int rpc_type_3, int rpc_type_4, int rpc_type_5, ScriptCaller function) {
		RegisterRPC(rpc_type_1, function);
		RegisterRPC(rpc_type_2, function);
		RegisterRPC(rpc_type_3, function);
		RegisterRPC(rpc_type_4, function);
		RegisterRPC(rpc_type_5, function);
	}

	void RegisterRPC(int rpc_type_1, int rpc_type_2, int rpc_type_3, int rpc_type_4, ScriptCaller function) {
		RegisterRPC(rpc_type_1, function);
		RegisterRPC(rpc_type_2, function);
		RegisterRPC(rpc_type_3, function);
		RegisterRPC(rpc_type_4, function);
	}

	void RegisterRPC(int rpc_type_1, int rpc_type_2, int rpc_type_3, ScriptCaller function) {
		RegisterRPC(rpc_type_1, function);
		RegisterRPC(rpc_type_2, function);
		RegisterRPC(rpc_type_3, function);
	}

	void RegisterRPC(int rpc_type_1, int rpc_type_2, ScriptCaller function) {
		RegisterRPC(rpc_type_1, function);
		RegisterRPC(rpc_type_2, function);
	}

	void RegisterRPC(int rpc_type_, ScriptCaller function) {
		LB_RPCHandler_Config cfg;
		if (!registeredRpcs.Find(rpc_type_, cfg)) {
			cfg = new LB_RPCHandler_Config();
			registeredRpcs.Insert(rpc_type_, cfg);
		}
		cfg.AddCaller(function);
	}

	bool HasPermission(string perm, bool ignoreGrantAll = false) {
		return LBAdmins.Get().HasPermission(perm, sender, ignoreGrantAll);
	}

	void PostRPC();

	void OnRPC(PlayerIdentity sender_, Object target_, int rpc_type_, ParamsReadContext ctx_) {
#ifdef SERVER
		if (!sender_)
			return;
		steamid = sender_.GetPlainId();
#else
		steamid = "";
#endif
		LB_RPCHandler_Config config;
		if (!registeredRpcs.Find(rpc_type_, config))
			return;
		sender = sender_;
		target = target_;
		rpc_type = rpc_type_;
		ctx = ctx_;
		LinkVariable();
		config.Invoke();
		PostRPC();
	}

	void SendErrorNotification(PlayerIdentity to, string message, int show_time = 5) {
		NotificationSystem.SendNotificationToPlayerIdentityExtended(to, show_time, GetNotificationTitle(), message, LBIconConfig.Get.error);
	}

	void SendWarningNotification(PlayerIdentity to, string message, int show_time = 5) {
		NotificationSystem.SendNotificationToPlayerIdentityExtended(to, show_time, GetNotificationTitle(), message, LBIconConfig.Get.warning);
	}

	void SendInfoNotification(PlayerIdentity to, string message, int show_time = 5) {
		NotificationSystem.SendNotificationToPlayerIdentityExtended(to, show_time, GetNotificationTitle(), message, LBIconConfig.Get.info);
	}

	void SendErrorNotification(PlayerIdentity to, LBTranslatedString message, int show_time = 5) {
		LBNotificationSystem.LBSendNotificationTranslated(to, show_time, GetNotificationTitle(), message, LBIconConfig.Get.error);
	}

	void SendWarningNotification(PlayerIdentity to, LBTranslatedString message, int show_time = 5) {
		LBNotificationSystem.LBSendNotificationTranslated(to, show_time, GetNotificationTitle(), message, LBIconConfig.Get.warning);
	}

	void SendInfoNotification(PlayerIdentity to, LBTranslatedString message, int show_time = 5) {
		LBNotificationSystem.LBSendNotificationTranslated(to, show_time, GetNotificationTitle(), message, LBIconConfig.Get.info);
	}

	void SendErrorNotification(string message, int show_time = 5) {
		SendErrorNotification(sender, message, show_time);
	}

	void SendWarningNotification(string message, int show_time = 5) {
		SendWarningNotification(sender, message, show_time);
	}

	void SendInfoNotification(string message, int show_time = 5) {
		SendInfoNotification(sender, message, show_time);
	}

	void SendErrorNotification(LBTranslatedString message, int show_time = 5) {
		SendErrorNotification(sender, message, show_time);
	}

	void SendWarningNotification(LBTranslatedString message, int show_time = 5) {
		SendWarningNotification(sender, message, show_time);
	}

	void SendInfoNotification(LBTranslatedString message, int show_time = 5) {
		SendInfoNotification(sender, message, show_time);
	}

	string GetNotificationTitle() {}

	void LinkVariable() {
		rpc.Reset();
		man = null;
	}

	protected Man GetMan() {
		if (man)
			return man;
#ifdef SERVER
		man = sender.GetPlayer();
#else
		man = g_Game.GetPlayer();
#endif
		return man;
	}

}