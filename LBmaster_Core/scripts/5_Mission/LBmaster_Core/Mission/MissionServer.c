modded class MissionServer {

	private ref set<string> lb_new_players = new set<string>();

	void MissionServer() {
		g_Game.InitRegisteredRPCHandlersLB();
		LBConfigManager.Get().LoadAllConfigs();
		LBInherit.Get();

		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Call(LBTestManager.Finish);
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Call(OnInitLB);

		LBAdmins.Get().RegisterPermission("core.toggle.adminmode", "Allows the player to open the admin menu and toggle Admin mode. Required for most actions");
		LBAdmins.Get().RegisterPermissionImportant("core.change.webhooks", false, false, false, "Allows the player to modify webhook events");
		LBAdmins.Get().RegisterPermission("core.change.icons", false, false, "Allows the player to change the icons.json config file");
		LBAdmins.Get().RegisterPermission("core.change.presets", false, false, "Allows the player to change the loadouts config file");
		LBAdmins.Get().RegisterPermission("core.change.logger", false, false, false, "Allows the player to change the logger settings for his client");
		LBAdmins.Get().RegisterPermissionImportant("core.change.logger_server", false, false, false, false, "Allows the player to change the logger settings for the server");
		LBAdmins.Get().RegisterPermission("core.teleport.staticplaced", false, false, "Allows the player to teleport to items placed down through the admin menu");
		LBAdmins.Get().RegisterPermission("core.change.currencies", false, false, "Allows the player to modify the currencies.json config file");
		LBAdmins.Get().RegisterPermission("core.change.appearancesettings", false, false, "Allows the player to modify the appearance.json config file");
		LBAdmins.Get().RegisterPermission("core.reload.configs", false, false, false, "Allows the player to reload config files from disk");
		LBAdmins.Get().RegisterPermissionImportant("core.change.admins", false, false, false, "Allows the player to change their roles in the admins.json file");
		LBAdmins.Get().RegisterPermissionImportant("core.view.errorlogs", false, false, false, "Allows the player to live view the Error and Fatal logs from the server ingame");
		LBAdmins.Get().RegisterPermission("core.read.players", false, false, "Allows the player to view a list of all players played on the server with info like steamid, name, first join and last seen");
		LBAdmins.Get().OnRegisterFinished();

		LB_Webhook_Manager.Get.RegisterWebhook("AdminModeEnabled", "{\"embeds\":[{\"title\":\"Admin Mode enabled\",\"description\":\"Admin [$$1$$](https://steamcommunity.com/profiles/$$2$$) enabled admin mode\",\"color\":65280}]}");
		LB_Webhook_Manager.Get.RegisterWebhook("AdminModeDisabled", "{\"embeds\":[{\"title\":\"Admin Mode disabled\",\"description\":\"Admin [$$1$$](https://steamcommunity.com/profiles/$$2$$) disabled admin mode\",\"color\":16711680}]}");
		LB_Webhook_Manager.Get.RegisterWebhook("LoggingSettingsChanged", "{\"embeds\":[{\"title\":\"Logging Settings Changed\",\"description\":\"[$$1$$](https:\\\/\\\/steamcommunity.com\\\/profiles\\\/$$2$$) changed the logging settings of the server\",\"color\":16711680,\"fields\":[{\"name\":\"New Log Level\",\"value\":\"$$3$$\",\"inline\":true},{\"name\":\"Log full timestamp\",\"value\":\"$$4$$\",\"inline\":true},{\"name\":\"Log to scriptlog\",\"value\":\"$$5$$\",\"inline\":true},{\"name\":\"Send critical Errors\",\"value\":\"$$6$$\",\"inline\":true},{\"name\":\"Log folder\",\"value\":\"$$7$$\",\"inline\":true}]}],\"allowed_mentions\":{\"parse\":[]}}");
		LB_Webhook_Manager.Get.RegisterWebhook("ConfigSaved", "{\"embeds\":[{\"title\":\"Config File changed\",\"description\":\"[$$1$$](https:\\\/\\\/steamcommunity.com\\\/profiles\\\/$$2$$) changed a config file\",\"color\":16711680,\"fields\":[{\"name\":\"Mod\",\"value\":\"$$3$$\",\"inline\":true},{\"name\":\"Config Name\",\"value\":\"$$4$$\",\"inline\":true},{\"name\":\"Classname\",\"value\":\"$$5$$\",\"inline\":true},{\"name\":\"File Path\",\"value\":\"$$6$$\",\"inline\":true}]}],\"allowed_mentions\":{\"parse\":[]}}");
		LB_Webhook_Manager.Get.RegisterWebhook("PlayerJoined", "{\"embeds\":[{\"type\":\"rich\",\"title\":\"Player joined the Server\",\"description\":\"" + "\",\"color\":65280,\"fields\":[{\"name\":\"Player\",\"value\":\"[$$1$$](https:\\\/\\\/steamcommunity.com\\\/profiles\\\/$$2$$)\",\"inline\":true},{\"name\":\"Obfuscated ID\",\"value\":\"$$3$$\",\"inline\":true}]}],\"allowed_mentions\":{\"parse\":[]}}");
		LB_Webhook_Manager.Get.RegisterWebhook("PlayerLeft", "{\"embeds\":[{\"type\":\"rich\",\"title\":\"Player left the Server\",\"description\":\"" + "\",\"color\":16711680,\"fields\":[{\"name\":\"Player\",\"value\":\"[$$1$$](https:\\\/\\\/steamcommunity.com\\\/profiles\\\/$$2$$)\",\"inline\":true}]}],\"allowed_mentions\":{\"parse\":[]}}");
		LB_Webhook_Manager.Get.AfterRegister();

		LBAPIManager.RegisterRoute("v1/files/list", new LBAPIParser<LBAPIListFiles>(ScriptCaller.Create(APIHandleFilesList), true));
		LBAPIManager.RegisterRoute("v1/files/get", new LBAPIParser<LBAPIPath>(ScriptCaller.Create(APIHandleFileGetContent), true));
		LBAPIManager.RegisterRoute("v1/files/set", new LBAPIParser<LBAPIFileContent>(ScriptCaller.Create(APIHandleFileSetContent), true));

		ServerUpdateRequestPacket checker = new ServerUpdateRequestPacket();
		checker.Load();
		checker.Send();
	}

	void OnInitLB() {
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Call(LBConfigManager.Get().OnMissionInit);
		LBLogger.Debug("Mission Base init finished", "Core");
		LBUniversalServerMarkerImpl.Setup();
	}

	void OnInitLBClient(PlayerIdentity sender) {
		lb_new_players.Insert(sender.GetPlainId());
		ScriptRPC rpc = new ScriptRPC();
		LBAdmins.Get().WriteToCtx(rpc, sender);
		if (sender)
			rpc.Send(null, LBmaster_Core_RPCs.ADMIN_SYNC, true, sender);
#ifdef BASICMAP
		LBUniversalServerMarkerImpl.OnPlayerJoin(sender);
#endif

		string obfName = LBPlayerNameObfuscation.GetObfuscatedName(sender);
		LBLogger.Admin("Player Joined with Obfuscated ID: " + obfName + " Real Identity: " + LBLogger.FormatPlayerIdentity(sender), "Core"); // Sent when a player joins the server
		LB_Webhook_Manager.Get.SendMessage("PlayerJoined", {sender.GetName(), sender.GetPlainId(), obfName}); // Sent when a player joins the server
		// Player Name, Player Steamid, Obfuscated ID

		if (LBAdmins.Get().CanEnableAdminMenu(sender.GetPlainId())) {
			rpc = new ScriptRPC();
			rpc.Write(g_Game.lb_new_mods_list);
			rpc.Send(null, LBmaster_Core_RPCs.NEW_MODS_CHECK_INFO, true, sender);
		}
		LBGroupInfoShare.OnPlayerOnline(sender);
	}

	void ~MissionServer() {
		LBLogger.Debug("Mission Base Deleted", "Core");
		LBAdmins.Delete();
		if (g_Game) {
			g_Game.ClearRegisteredMenus();
			g_Game.ClearRegisteredRPCs();
		}
		LBConfigManager.Delete();
		LB_Elevator.Set(new LB_Elevator_Game());
		LBAPIManager.Delete();
	}

	override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity) {
		if (callBackupClientNew) {
			callBackupClientNew = false;
			LBClientNewEvent(player, identity);
		}
		super.InvokeOnConnect(player, identity);
		LBLastPlayernames.Get.AddName(identity);
		LB_PresetLoader.Get.SendRootLayoutsTo(identity);

		if (identity && player && lb_new_players.Find(identity.GetPlainId()) != -1) {
			lb_new_players.RemoveItem(identity.GetPlainId());
			LBClientLoadedFirstEvent(player, identity);
		}
	}

	override void PlayerDisconnected(PlayerBase player, PlayerIdentity identity, string uid) {
		super.PlayerDisconnected(player, identity, uid);
		LBLastPlayernames.Get.OnPlayerOffline(uid);
		LBLogger.Admin("Player " + LBLogger.FormatPlayerIdentity(identity) + " left the server", "Core");
		if (identity) {
			LB_Webhook_Manager.Get.SendMessage("PlayerLeft", {identity.GetName(), identity.GetPlainId()}); // Sent when a player joins the server
			// Player Name, Player Steamid
			LBGroupInfoShare.OnPlayerOffline(identity.GetPlainId());
		}
	}

	bool callBackupClientNew = false;

	override void OnEvent(EventType eventTypeId, Param params) {
		callBackupClientNew = false;
		switch (eventTypeId) {
			case ClientNewEventTypeID:
				callBackupClientNew = true;
				super.OnEvent(eventTypeId, params);
				if (callBackupClientNew) {
					callBackupClientNew = false;
					ClientNewEventParams newParams;
					Class.CastTo(newParams, params);
					LBClientNewEvent(PlayerBase.Cast(newParams.param1.GetPlayer()), newParams.param1);
				}
				return;

		}
		super.OnEvent(eventTypeId, params);
	}

	void LBClientNewEvent(PlayerBase player, PlayerIdentity identity);
	void LBClientLoadedFirstEvent(PlayerBase player, PlayerIdentity identity);

	override PlayerBase OnClientNewEvent(PlayerIdentity identity, vector pos, ParamsReadContext ctx) {
		callBackupClientNew = false;
		PlayerBase bs = super.OnClientNewEvent(identity, pos, ctx);
		LBLasterSpawnLoadoutsConfig.Get.SaveLastPlayerDelayed(bs);
		LBClientNewEvent(bs, identity);
		return bs;
	}

	void APIHandleFileSetContent(LBAPIResponse resp, LBAPIFileContent path) {
		string filePath = path.GetSanitized();
		if (filePath.IndexOf("$profile:") != 0 && filePath.IndexOf("$saves:") != 0 && filePath.IndexOf("$mission:") != 0) {
			resp.Set(400, "Path must start with $profile: $saves: or $mission:");
			return;
		}
		FileHandle handle = OpenFile(filePath, FileMode.WRITE);
		if (handle == 0) {
			resp.Set(500, "Could not open File to write");
			return;
		}
		FPrint(handle, path.content);
		resp.SetOK();
		CloseFile(handle);
	}

	void APIHandleFileGetContent(LBAPIResponse resp, LBAPIPath path) {
		string filePath = path.GetSanitizedProfile();
		if (filePath.IndexOf("$profile:") != 0 && filePath.IndexOf("$saves:") != 0 && filePath.IndexOf("$mission:") != 0) {
			resp.Set(400, "Path must start with $profile: $saves: or $mission:");
			return;
		}
		if (!FileExist(filePath)) {
			resp.Set(404, "File not found!");
			return;
		}
		string content;
		FileHandle handle = OpenFile(filePath, FileMode.READ);
		if (handle == 0) {
			resp.Set(500, "Could not open File to read");
			return;
		}
		ReadFile(handle, content, 2000000000);
		string json;
		TStringArray serialArr = new TStringArray();
		serialArr.Insert(content);
		LBAPIParser.m_Serial.WriteToString(serialArr, false, json);
		resp.SetData(json);
		resp.SetOK();
		CloseFile(handle);
	}

	void APIHandleFilesList(LBAPIResponse resp, LBAPIListFiles path) {
		string filePath = path.GetSanitized();
		if (filePath.IndexOf("$profile:") != 0 && filePath.IndexOf("$saves:") != 0 && filePath.IndexOf("$mission:") != 0) {
			resp.Set(400, "Path must start with $profile: $saves: or $mission:");
			return;
		}
		if (!FileExist(filePath)) {
			resp.Set(404, "Folder not found!");
			return;
		}
		string filename;
		FileAttr fileAttr;
		FindFileHandle handle = FindFile(filePath + "/" + path.GetPattern(), filename, fileAttr, FindFileFlags.ALL);
		if (handle == 0) {
			resp.Set(500, "Could not open Folder to read");
			return;
		}
		array<ref LBAPIFile> files = new array<ref LBAPIFile>();
		if (filename != "") {
			files.Insert(new LBAPIFile(filename, fileAttr & FileAttr.DIRECTORY));
		}
		while (FindNextFile(handle, filename, fileAttr)) {
			if (filename != "")
				files.Insert(new LBAPIFile(filename, fileAttr & FileAttr.DIRECTORY));
		}
		string json;
		LBAPIParser.m_Serial.WriteToString(files, false, json);
		resp.SetData(json);
		resp.SetOK();
		CloseFindFile(handle);
	}

}
