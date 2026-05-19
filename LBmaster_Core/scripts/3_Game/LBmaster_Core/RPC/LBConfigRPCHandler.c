[RegisterLBRPCHandler(LBConverter.TypenameToString(LBConfigRPCHandler), LBRPCHandlerType.BOTH)]
class LBConfigRPCHandler : LB_RPCHandler_Base {

	void LBConfigRPCHandler() {
		RegisterRPC(LBmaster_Core_RPCs.CONFIG_REQUEST, ScriptCaller.Create(OnConfigRequest));
		RegisterRPC(LBmaster_Core_RPCs.CONFIG_SYNC, ScriptCaller.Create(HandleOnConfigSync));
		RegisterRPC(LBmaster_Core_RPCs.CONFIG_SYNC_CONFIRM, ScriptCaller.Create(OnConfigSyncConfirm));
		RegisterRPC(LBmaster_Core_RPCs.CONFIG_SAVE_CONFIRM, ScriptCaller.Create(OnConfigSaveConfirm));
		RegisterRPC(LBmaster_Core_RPCs.CONFIG_LIST, ScriptCaller.Create(OnListConfigs));
		RegisterRPC(LBmaster_Core_RPCs.CONFIG_RELOAD_FILE, ScriptCaller.Create(OnReloadConfigRequest));
	}

	override string GetNotificationTitle() {
		return "Configs";
	}

	void OnConfigRequest() {
#ifdef SERVER
		string name;
		if (!ctx.Read(name)) {
			LBLogger.Error("Could not read Config Name Request from RPC by " + LBLogger.FormatPlayerIdentity(sender), "Core");
			return;
		}
		LBConfigLoaderBase loader = LBConfigManager.Get().GetLoaderByName(name);
		if (!loader) {
			LBLogger.Error("Could not find Config Named " + name + " from Request RPC by " + LBLogger.FormatPlayerIdentity(sender), "Core");
			return;
		}
		loader.SendToClient(ctx, sender);
#endif
	}

	void HandleOnConfigSync() {
		bool success;
		OnConfigSync(success);
	}

	bool OnConfigSync(out bool success) {
		string name;
		if (!ctx.Read(name)) {
			LBLogger.Error("Could not read Config Name Sync from Sync RPC by " + LBLogger.FormatPlayerIdentity(sender), "Core");
			return false;
		}
		LBConfigLoaderBase loader = LBConfigManager.Get().GetLoaderByName(name);
		if (!loader) {
			LBLogger.Error("Could not find Config Named " + name + " from RPC by " + LBLogger.FormatPlayerIdentity(sender), "Core");
			return false;
		}
		success = loader.WasInitialized();
#ifdef SERVER
		string webhookPrintName = loader.GetName();
		if (LBStringTools.EndsWith(webhookPrintName, "_"))
			webhookPrintName = webhookPrintName.Substring(0, webhookPrintName.Length() - 1);
		LB_Webhook_Manager.Get.SendMessage("ConfigSaved", {sender.GetName(), sender.GetPlainId(), loader.GetModName(), loader.GetFilename(), webhookPrintName, loader.GetFilePath()}, sender.GetPlainId()); // Sent when an admin saves and config file or player data file
		// Admin Name, Admin Steamid, Mod / Folder the config is saved in, Filename of the config, Classname of the Config, Full file name of the config
#endif
		return loader.ReadFromCtx(ctx, sender);
	}

	void OnConfigSyncConfirm() {
		bool success;
		if (OnConfigSync(success)) {
#ifdef SERVER
			rpc.Write(success);
			Send(LBmaster_Core_RPCs.CONFIG_SAVE_CONFIRM);
#endif
		}
	}

	void OnConfigSaveConfirm() {
#ifndef SERVER
		LBLoadingIcon.Hide();
		bool success = false;
		if (!ctx.Read(success) || !success) {
			NotificationSystem.AddNotificationExtended(4.0, "Could not save config", "A problem occured while saving the config! Please read the script_..._log for more information!", GetLBMessageInfoIcon());
		} else {
			NotificationSystem.AddNotificationExtended(4.0, "#lb_co_config_saved", "The config was saved sucessfully!", GetLBMessageInfoIcon());
		}
#endif
	}

	void OnListConfigs() {
		if (!LBAdmins.Get().HasPermission("core.reload.configs", sender))
			return;
		int count = 0;
#ifdef SERVER
		array<LBConfigLoaderBase> availableLoaders = new array<LBConfigLoaderBase>();
		foreach (string path, LBConfigLoaderBase loader : LBConfigManager.pathLoaders) {
			if (!loader.GetType() == LBConfigType.CONFIG)
				continue;
			if (CanReloadLoader(loader)) {
				availableLoaders.Insert(loader);
			}
		}
		count = availableLoaders.Count();
		rpc.Write(count);
		foreach (LBConfigLoaderBase sendLoader : availableLoaders) {
			rpc.Write(sendLoader.GetModName());
			rpc.Write(sendLoader.GetFilename());
		}
		rpc.Send(null, LBmaster_Core_RPCs.CONFIG_LIST, true, sender);
#endif
	}

	bool CanReloadLoader(LBConfigLoaderBase loader) {
		return loader.GetReceivePermission() == "" || LBAdmins.Get().HasPermission(loader.GetReceivePermission(), sender) || (LBAdmins.Get().IsAllPermissionsGranted(sender.GetPlainId()) && !loader.IsSyncedToClient());
	}

	void OnReloadConfigRequest() {
		if (!LBAdmins.Get().HasPermission("core.reload.configs", sender))
			return;
		int count;
		if (!ctx.Read(count))
			return;
		LBLogger.Info("Reloading " + count + " configs from Disk.", "Core");
		int success = 0;
		for (int i = 0; i < count; ++i) {
			string modName;
			string filename;
			if (!ctx.Read(modName) || !ctx.Read(filename))
				return;
			LBConfigLoaderBase loader = LBConfigManager.GetLoaderByPath("Config/" + modName + "/" + filename);
			if (!loader || !CanReloadLoader(loader))
				continue;
			LBLogger.Debug("Reloading config " + modName + "/" + filename, "Core");
			string error;
			if (loader.ReloadFromDisk(error)) {
				loader.SyncEveryone();
				++success;
				LBLogger.Admin("Config " + modName + "/" + filename + " was reloaded from Disk by " + LBLogger.FormatPlayerIdentity(sender), "Core");
			} else {
				LBLogger.Error("Could not reload " + modName + "/" + filename + " from Disk! Error: " + error, "Core");
			}
		}
		if (success == count) {
			SendInfoNotification(LBTranslatedString("#lb_reload_success_all", {"%count%", "" + count}));
		} else {
			SendWarningNotification(LBTranslatedString("#lb_reload_failed_some", {"%success%", "" + success, "%count%", "" + count}));
		}
	}

}