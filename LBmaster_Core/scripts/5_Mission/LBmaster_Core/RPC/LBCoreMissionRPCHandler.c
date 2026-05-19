[RegisterLBRPCHandler(LBConverter.TypenameToString(LBCoreMissionRPCHandler), LBRPCHandlerType.SERVER)]
class LBCoreMissionRPCHandler : LB_RPCHandler {

	void LBCoreMissionRPCHandler() {
		RegisterRPC(LBmaster_Core_RPCs.UPDATE_CHECK_INFO, ScriptCaller.Create(OnCheckInfoRequestReceived));
		RegisterRPC(LBmaster_Core_RPCs.ADMIN_PLACE, ScriptCaller.Create(LBRPCPlaceAdmin));
		RegisterRPC(LBmaster_Core_RPCs.ADMIN_WEBHOOK_CONFIG, ScriptCaller.Create(SendWebhooksConfig));
		RegisterRPC(LBmaster_Core_RPCs.ADMIN_WEBHOOK_CONFIG_SAVE, ScriptCaller.Create(ReadWebhooksConfig));
		RegisterRPC(LBmaster_Core_RPCs.ADMIN_PLAYERS_REQUEST, ScriptCaller.Create(RequestAllPlayerData));
		RegisterRPC(LBmaster_Core_RPCs.TELEPORT_TO_PLACED_ITEM, ScriptCaller.Create(HandleTeleporToPlacedItem));
		RegisterRPC(LBmaster_Core_RPCs.REQUEST_ADMIN_PERMISSIONS, ScriptCaller.Create(HandleRequestAdminPermissions));
		RegisterRPC(LBmaster_Core_RPCs.SAVE_ADMIN_PERMISSIONS, ScriptCaller.Create(HandleSaveAdminPermissions));
		RegisterRPC(LBmaster_Core_RPCs.LOGS_GET, ScriptCaller.Create(HandleGetLogs));
		RegisterRPC(LBmaster_Core_RPCs.LOGS_COUNT, ScriptCaller.Create(HandleCountLogs));
		RegisterRPC(LBmaster_Core_RPCs.CLIENT_MISSION_INITIALIZED, ScriptCaller.Create(HandleClientMissionInitialized));
		RegisterRPC(LBmaster_Core_RPCs.REQUEST_LOGGER_CONFIG, ScriptCaller.Create(HandleLoggerConfigRequested));
		RegisterRPC(LBmaster_Core_RPCs.CHANGE_LOGGER_CONFIG, ScriptCaller.Create(HandleLoggerConfigChanged));
	}

	void HandleGetLogs() {
		if (!HasPermission("core.view.errorlogs"))
			return;
		int start;
		int end;
		if (!ctx.Read(start) || !ctx.Read(end))
			return;
		int count = LBLogger.errorLogs.Count();
		if (start < 0 || start > count || end < 0 || end > count)
			return;
		TStringArray subArray = new TStringArray();
		for (int i = start; i < end; ++i) {
			subArray.Insert(LBLogger.errorLogs.Get(i));
		}
		rpc.Write(start);
		rpc.Write(subArray);
		Respond();
	}

	void HandleLoggerConfigRequested() {
#ifdef SERVER
		if (!HasPermission("core.change.logger_server"))
			return;
		LBLoggerConfig cfg = LBLogger.LoadConfig();
		rpc.Write(cfg);
		Respond();
#endif
	}

	void HandleLoggerConfigChanged() {
#ifdef SERVER
		if (!HasPermission("core.change.logger_server"))
			return;
		LBLoggerConfig cfg;
		if (!ctx.Read(cfg) || !cfg)
			return;
		string error;
		if (JsonFileLoader<LBLoggerConfig>.SaveFile(LBLogger.LOG_CONFIG_PATH, cfg, error)) {
			LBLogger.Admin("Updated logger config by " + LBLogger.FormatPlayerIdentity(sender), "Core");
			LB_Webhook_Manager.Get.SendMessage("LoggingSettingsChanged", {sender.GetName(), steamid, LBLogger.GetLoglevelStringPretty(cfg.logLevel), "" + cfg.logFullTimestamp, "" + cfg.logToScriptlog, "" + cfg.sendCriticalErrorLogs, cfg.logFolder}, steamid); // Sent when an admin changes the logging settings of the server
			// Admin Name, Admin Steamid, new log level, new log full timestamp value, new write to scriptlog value, new send critical logs value, new log folder
			LBLogger.ReloadConfig();
			bool success = true;
			rpc.Write(success);
			Send(LBmaster_Core_RPCs.CONFIG_SAVE_CONFIRM);
		}
#endif
	}

	void HandleClientMissionInitialized() {
		MissionServer mission = MissionServer.Cast(g_Game.GetMission());
		if (mission)
			mission.OnInitLBClient(sender);
	}

	void HandleCountLogs() {
		if (!HasPermission("core.view.errorlogs"))
			return;
		int count = LBLogger.errorLogs.Count();
		rpc.Write(count);
		Respond();
	}

	void HandleRequestAdminPermissions() {
		if (!HasPermission("core.change.admins"))
			return;
		rpc.Write(LBAdmins.Get());
		array<ref Param2<string, string>> permissionComments = new array<ref Param2<string, string>>();
		foreach (string perm, string comment : LBAdmins.Get().permissionComments) {
			int index = 0;
			foreach (Param2<string, string> other : permissionComments) {
				if (LBStringTools.Comp(other.param1, perm, false) < 0)
					++index;
				else
					break;
			}
			permissionComments.InsertAt(new Param2<string, string>(perm, comment), index);
		}
		rpc.Write(permissionComments);
		TIntSet importantPermission = new TIntSet();
		index = 0;
		foreach (Param2<string, string> perm2 : permissionComments) {
			if (LBAdmins.Get().importantPermissions.Find(perm2.param1) != -1)
				importantPermission.Insert(index);
			++index;
		}
		rpc.Write(importantPermission);
		rpc.Send(null, LBmaster_Core_RPCs.REQUEST_ADMIN_PERMISSIONS, true, sender);
	}

	void HandleSaveAdminPermissions() {
		if (!HasPermission("core.change.admins"))
			return;
		LBAdmins admins;
		if (!ctx.Read(admins))
			return;
		LBAdmins original = LBAdmins.Get();
		original.admins = admins.admins;
		original.groups = admins.groups;
		original.Save();
		original.OnLoad();
		original.SendAll();
		bool success = true;
		rpc.Write(success);
		Send(LBmaster_Core_RPCs.CONFIG_SAVE_CONFIRM);
	}

	void HandleTeleporToPlacedItem() {
		if (!HasPermission("core.teleport.staticplaced"))
			return;
		if (!FindPlayer())
			return;
		Param1<vector> pos;
		if (!ctx.Read(pos))
			return;
		player.SetPosition(pos.param1);
	}

	void OnCheckInfoRequestReceived() {
		if (!HasPermission("core.toggle.adminmode"))
			return;
		TStringArray ret;
		if (LBVersionUpdateCheckCallback.received) {
			if (LBVersionUpdateCheckCallback.updateResponse.oldVersions.Count() > 0) {
				ret = new TStringArray();
				foreach (LBInstalledModInfo info : LBVersionUpdateCheckCallback.updateResponse.oldVersions) {
					ret.Insert(info.message);
				}
			} else {
				ret = {"#lb_message_all_updated"};
			}
		} else {
			ret = {"Error", LBVersionUpdateCheckCallback.errorMessage};
		}
		rpc.Write(ret);
		rpc.Send(null, LBmaster_Core_RPCs.UPDATE_CHECK_INFO, true, sender);
	}

	void LBRPCPlaceAdmin() {
		Param4<vector, vector, string, string> placeinfo;
		if (!ctx.Read(placeinfo))
			return;
		if (GetElevatorGame().AddAdminPlacedItem(placeinfo.param1, placeinfo.param2, placeinfo.param3, sender, placeinfo.param4)) {
			SendInfoNotification("#lb_place_success " + placeinfo.param4);
		} else {
			SendErrorNotification("#lb_place_failed " + placeinfo.param4);
		}
	}

	void SendWebhooksConfig() {
		if (!HasPermission("core.change.webhooks", true)) {
			LBLogger.Error("Player " + GetSenderLog() + " tried to get Webhooks config without Permissions!", "Core");
			return;
		}
		LBLogger.Info("Player " + GetSenderLog() + " requested Webhooks.json and config was sent", "Core");
		LB_Webhook_Manager.Get.WriteToCtx(rpc);
		rpc.Send(null, LBmaster_Core_RPCs.ADMIN_WEBHOOK_CONFIG, true, sender);
	}

	void ReadWebhooksConfig() {
		if (!HasPermission("core.change.webhooks", true)) {
			LBLogger.Error("Player " + GetSenderLog() + " tried to save Webhooks config without Permissions!", "Core");
			return;
		}
		if (!LB_Webhook_Manager.Get.ReadFromCtx(ctx)) {
			LBLogger.Error("Could not read new Webhooks config from the client", "Core");
			return;
		}
		LBLogger.Admin("Webhook config was changed by " + GetSenderLog(), "Core");
		LB_Webhook_Manager.Get.SendMessage("ConfigSaved", {sender.GetName(), sender.GetPlainId(), LB_Webhook_Manager.Loader.GetModName(), LB_Webhook_Manager.Loader.GetFilename(), LB_Webhook_Manager.Loader.GetName(), LB_Webhook_Manager.Loader.GetFilePath()}, sender.GetPlainId()); // Sent when an admin saves and config file or player data file
		LB_Webhook_Manager.Get.OnReceivedFromRPC(sender);
		LBConfigManager.Get().OnConfigReceived(LB_Webhook_Manager_);
		LB_Webhook_Manager.Loader.Save();
		bool success = true;
		rpc.Write(success);
		Send(LBmaster_Core_RPCs.CONFIG_SAVE_CONFIRM);
	}

	void RequestAllPlayerData() {
		if (!HasPermission("core.toggle.adminmode"))
			return;
		rpc.Write(LBLastPlayernames.Get.loadedPlayers);
		LBLogger.Info("Sending " + LBLastPlayernames.Get.loadedPlayers.Count() + " player infos to " + GetSenderLog(), "Core");
		Respond();
	}

}