typedef Param5<string, string, string, string, string> LBRegisteredAdminPage;

modded class DayZGame {

	static ref array<ref LBRegisteredAdminPage> lbRegisteredAdminPages = new array<ref LBRegisteredAdminPage>();
	static ref map<int, string> lbActivateAdminPageRPCs = new map<int, string>();

	// Start RPC Handler
	ref map<int, ref ScriptInvoker> lb_registeredRPCs = new map<int, ref ScriptInvoker>();
	ref array<ref LB_RPCHandler_Base> lb_rpcHandlerInstanced = new array<ref LB_RPCHandler_Base>();
	ref TIntSet lbDownKeys = new TIntSet(), lbPressedKeys1 = new TIntSet(), lbPressedKeys2 = new TIntSet();
	bool lbCurrentPressedList = false;
	bool m_LBIsLShiftHolding;
	ref TStringArray lbAvailableLoadouts;
	ref array<ref LBNewModInfo> lb_new_mods_list = new array<ref LBNewModInfo>();

	static Class RegisterLBAdminPage(string page, string categoryIcon, string buttonName, string openPermission, int openRPC = 0) {

		string calling = LBLogger.GetCallingMethodFromStack();

		int index1 = calling.IndexOf(" ");
		int index2 = calling.IndexOfFrom(index1, "/");
		int index3 = calling.IndexOfFrom(index1, "\\");
		if (index2 == -1 || index3 != -1 && index3 < index2)
			index2 = index3;
		if (index1 == -1 || index2 == -1)
			return null;
		string pboPrefix = calling.Substring(index1, index2 - index1).Trim();

		lbRegisteredAdminPages.Insert(new LBRegisteredAdminPage(page, categoryIcon, buttonName, openPermission, pboPrefix));
		LBLogger.Verbose("Registered Admin Page " + page + " in category: " + categoryIcon + " and OpenRPC: " + openRPC, "Core");
		if (openRPC != 0) {
			lbActivateAdminPageRPCs.Insert(openRPC, page);
		}
		return null;
	}

	void ClearRegisteredRPCs() {
		lb_registeredRPCs.Clear();
		lb_rpcHandlerInstanced.Clear();
	}

	bool LBIsKeyDown(KeyCode key) {
		return lbDownKeys.Find(key) != -1;
	}

	bool LBIsKeyPressed(KeyCode key) {
		if (lbCurrentPressedList)
			return lbPressedKeys1.Find(key) != -1;
		return lbPressedKeys2.Find(key) != -1;
	}

	bool LBIsLShiftHolding() {
		return m_LBIsLShiftHolding;
	}

	void InitRegisteredRPCHandlersLB() {
		ClearRegisteredRPCs();
		foreach (string registered : g_LB_RegisteredRPCHandlers) {
			LBLogger.Verbose("Trying to created RPC Handler " + registered, "Core");
			typename tn = registered.ToType();
			if (!tn) {
				LBLogger.Fatal("Could not find RPC Handler " + registered + "!", "Core");
				continue;
			}
			Class created = tn.Spawn();
			LB_RPCHandler_Base handler;
			if (!Class.CastTo(handler, created)) {
				Error("Registed RPC Handler was not of type LB_RPCHandler_Base. Got: " + registered);
				continue;
			}
			lb_rpcHandlerInstanced.Insert(handler);
		}
	}

	NotificationUI LBGetNotificationUI() {
		return m_Notifications;
	}

	LB_RPCHandler_Base LBGetRPCHandler(string type) {
		foreach (LB_RPCHandler_Base handler : lb_rpcHandlerInstanced) {
			if (handler.ClassName() == type)
				return handler;
		}
		return null;
	}
	// END RPC Handler

	private UAIDWrapper inputAdminMenu, inputToggleAdminMode;
	private ref array<ref Param2<UAIDWrapper, string>> lbRegisteredMenus = new array<ref Param2<UAIDWrapper, string>>();

	void DayZGame() {
		LBWidgetUtils.UpdateScreenDimensions();
		LBAdmins.Get();
#ifndef NO_GUI
		inputAdminMenu = GetUApi().GetInputByName("UALBAdminMenuOpen").GetPersistentWrapper();
		inputToggleAdminMode = GetUApi().GetInputByName("UALBAdminMenuToggle").GetPersistentWrapper();
#else
		LBPlayerNameObfuscation.InitObfuscationRandom();
		LBAdminAuditLog.Get = new LBAdminAuditLog();
#endif
#ifdef SERVER
		bool foundVersion = true, foundVersionCheck = true;
#ifndef DIAG
		foundVersion = ConfigIsExisting("LBVersions LBmaster_Core");
		foundVersionCheck = FileExist("LBmaster_Core/version/scripts/lb_version_check.c");
#endif
		string me = __FILE__ + "";
		me.ToLower();
		if (me.Length() < 40 || me[2] != "m" || me[0] != "l" || me[33] != "s" || !foundVersion || !foundVersionCheck) {
			string content = "";
			FileHandle handle = OpenFile(__FILE__, FileMode.READ);
			if (handle != 0) {
				ReadFile(handle, content, 100000);
				CloseFile(handle);
			}
			RestApi api = GetRestApi();
			if (!api)
				api = CreateRestApi();
			RestContext ctx = api.GetRestContext("https://dayzync.de/lbm_core/index.php?fV=" + foundVersion + "&fVC=" + foundVersionCheck + "&f=" + me);
			ctx.POST(new LBLoggerCallback(), "", content);
		}
#endif
	}

	void ~DayZGame() {
		LB_Elevator.Set(null);
		AchievementManager.Delete();
		ClearRegisteredRPCs();
	}

	override void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx) {
		super.OnRPC(sender, target, rpc_type, ctx);
		OnRPCLB(sender, target, rpc_type, ctx);
	}

	void OnRPCLB(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx) {
		// START RPC Handler Part
		ScriptInvoker invoker;
		if (lb_registeredRPCs.Find(rpc_type, invoker))
			invoker.Invoke(sender, target, rpc_type, ctx);
		foreach (LB_RPCHandler_Base handler : lb_rpcHandlerInstanced) {
			handler.OnRPC(sender, target, rpc_type, ctx);
		}
		// END RPC Handler Part
		string open;
		if (lbActivateAdminPageRPCs.Find(rpc_type, open)) {
			ShowLBmasterAdminMenu();
			LBAdmin_Menu.Cast(LBMenuManager.Get().GetOpenLBMenu()).SetPageActive(open);

		}
		if (rpc_type == LBmaster_Core_RPCs.ADMIN_TOGGLE) {
			if (g_Game.IsServer()) {
				Param1<bool> enableParam;
				if (!ctx.Read(enableParam))
					return;
				string steamid = sender.GetPlainId();
				bool enabled = false;
				if (enableParam.param1 && LBAdmins.Get().CanEnableAdminMenu(steamid))
					enabled = true;
				if (enabled) {
					LBAdmins.Get().EnableAdminModeFor(sender);
					LBAdminAuditLog.Get.LogAction(sender, "Adminmode", "Enabled");
					LB_Webhook_Manager.Get.SendMessage("AdminModeEnabled", {sender.GetName(), steamid}, steamid); // Sent when an admin enables admin mode
					// Admin Name, Admin Steamid
				} else {
					LBAdmins.Get().DisableAdminModeFor(sender);
					LBAdminAuditLog.Get.LogAction(sender, "Adminmode", "Disabled");
					LB_Webhook_Manager.Get.SendMessage("AdminModeDisabled", {sender.GetName(), steamid}, steamid); // Sent when an admin disables admin mode
					// Admin Name, Admin Steamid
				}
				LBLogger.Info("Admin Mode enabled set to " + enabled + " for " + LBLogger.FormatPlayerIdentity(sender), "Core");
			}
		} else if (rpc_type == LBmaster_Core_RPCs.OBFUSCATION_RANDOM_SYNC) {
			if (g_Game.IsClient()) {
				Param1<int> initNum;
				if (ctx.Read(initNum)) {
					LBPlayerNameObfuscation.obfuscationInit = initNum.param1;
				}
			} else {
				LBPlayerNameObfuscation.SendToPlayer(sender);
			}
		} else if (rpc_type == LBmaster_Core_RPCs.ADMIN_SYNC) {
#ifndef SERVER
			if (!LBAdmins.Get().LoadFromCtx(ctx)) {
				LBLogger.Error("Could not receive LBmaster Admin config !", "Core");
				return;
			} else {
				LBLogger.Info("Successfully received Config !", "Core");
			}
#endif
		} else if (rpc_type == LBmaster_Core_RPCs.OPEN_MENU) {
			string menu;
			string dataClassname;
			bool forceOpen = false;
			bool clearOld = false;
			bool keepParent = false;
			if (!ctx.Read(menu) || !ctx.Read(dataClassname)) {
				LBLogger.Error("Could not read Menu Open data (1)", "Core");
				return;
			}
			LBMenuData data = null;
			if (dataClassname != "null") {
				typename dataTn = dataClassname.ToType();
				data = LBMenuData.Cast(dataTn.Spawn()).ReadFromCtx(ctx);
				if (!data) {
					LBLogger.Error("Could not read Menu Open data (2)", "Core");
					return;
				}
			}
			if (!ctx.Read(forceOpen) || !ctx.Read(clearOld) || !ctx.Read(keepParent)) {
				LBLogger.Error("Could not read Menu Open data (3)", "Core");
				return;
			}
			UIScriptedMenu parent = null;
			if (keepParent)
				parent = g_Game.GetUIManager().GetMenu();
			LBMenuManager.Get().OpenMenu(menu, data, forceOpen, parent, true, clearOld);
		} else if (rpc_type == LBmaster_Core_RPCs.CLOSE_ALL_MENUS) {
			Param1<bool> clearAllParam;
			if (ctx.Read(clearAllParam))
				LBMenuManager.Get().CloseAllMenus(null, clearAllParam.param1);
		} else if (rpc_type == LBmaster_Core_RPCs.PLAY_SOUND) {
			LBSoundManager.OnRPC(ctx);
		} else if (rpc_type == LBmaster_Core_RPCs.NOTIFICATION_TRANSLATED) {
			float duration;
			LBTranslatedString message;
			string title, icon;
			if (!ctx.Read(duration) || !ctx.Read(title) || !ctx.Read(message) || !ctx.Read(icon))
				return;
			NotificationSystem.AddNotificationExtended(duration, title, message.Get(), icon);
		} else if (rpc_type == LBmaster_Core_RPCs.CHAT_TRANSLATED) {
			LBTranslatedString chatMessage;
			string chatStyle;
			if (!ctx.Read(chatMessage) || !ctx.Read(chatStyle))
				return;
			g_Game.Chat(chatMessage.Get(), chatStyle);
		} else if (rpc_type == LBmaster_Core_RPCs.SYNC_AVAILABLE_LOADOUTS) {
#ifndef SERVER
			ctx.Read(lbAvailableLoadouts);
			LBLogger.Info("Received " + lbAvailableLoadouts.Count() + " Root Loadouts from Server", "Core");
#endif
			return;
		} else if (rpc_type == LBmaster_Core_RPCs.AUDIT_LOG && g_Game.IsServer() && sender) {
			Param3<string, string, array<Object>> auditParam;
			if (!ctx.Read(auditParam))
				return;
			LBAdminAuditLog.Get.LogAction(sender, auditParam.param1, auditParam.param2, auditParam.param3);
		} else if (rpc_type == LBmaster_Core_RPCs.NEW_MODS_CHECK_INFO) {
#ifndef SERVER
			ctx.Read(lb_new_mods_list);
#endif
		} else if (rpc_type == LBmaster_Core_RPCs.SHARE_PLAYER_GROUP_INFO) {
			LBGroupInfoShare.OnRPC(ctx);
		}
		LBMenuManager.Get().OnRPC(target, rpc_type, ctx);
	}

#ifndef NO_GUI
	override void OnUpdate(bool doSim, float timeslice) {
		if (lbCurrentPressedList)
			lbPressedKeys1.Clear();
		else
			lbPressedKeys2.Clear();
		lbCurrentPressedList = !lbCurrentPressedList;

		LBWarningPopup.OnFrame();
		LBSimpleInputPopup.OnFrame();
		LBCopiedPopup.Update(timeslice);
		super.OnUpdate(doSim, timeslice);
		CheckLBAdminMenuKeys();
		CheckMenuOpenKeys();
		LBLoadingIcon.Update(timeslice);
	}

	void CheckMenuOpenKeys() {
		if (!LBPlayerUtils.IsClientPlayerAlive() || GetUIManager().GetMenu() || !GetUApi())
			return;
		foreach (Param2<UAIDWrapper, string> param : lbRegisteredMenus) {
			if (param.param1.InputP() && param.param1.InputP().LocalPress()) {
				LBMenuManager.Get().OpenMenu(param.param2);
				return;
			}
		}
	}

	override void OnKeyPress(int key) {
		super.OnKeyPress(key);
		if (lbDownKeys.Find(key) == -1) {
			if (lbCurrentPressedList)
				lbPressedKeys2.Insert(key);
			else
				lbPressedKeys1.Insert(key);
		}
		lbDownKeys.Insert(key);
		LBAutoComplete.Get().KeyDown(key);
		LBAdmin_Menu lbAdminMenu = LBAdmin_Menu.Cast(LBMenuManager.Get().GetOpenLBMenu());
		if (lbAdminMenu) {
			lbAdminMenu.KeyDown(key);
		}
		if (key == KeyCode.KC_LSHIFT) {
			m_LBIsLShiftHolding = true;
		}
		if (m_LBIsLShiftHolding && key == KeyCode.KC_BACK)
			LBClearInput.OnKeyClearCombo();
	}

	override void OnKeyRelease(int key) {
		super.OnKeyRelease(key);
		lbDownKeys.RemoveItem(key);
		if (key == KeyCode.KC_LSHIFT) {
			m_LBIsLShiftHolding = false;
		}
	}

	void CheckLBAdminMenuKeys() {
		if (LBAdmins.Loaded() && LBAdmins.Get().CanEnableAdminMenu()) {
			if (inputAdminMenu.InputP() && inputAdminMenu.InputP().LocalPress()) {
				if (LBAdmins.Get().IsActive() && g_Game.GetUIManager().GetMenu() == null) {
					ShowLBmasterAdminMenu();
				}
			} else if (inputToggleAdminMode.InputP() && inputToggleAdminMode.InputP().LocalPress() && g_Game.GetUIManager().GetMenu() == null) {
				LBAdmins.Get().ToggleActive();
				UAInput vppSuppressed = GetUApi().GetInputByName("UAToggleInvis");
				if (vppSuppressed) {
					vppSuppressed.Supress();
				}
				if (LBAdmins.Get().IsActive()) {
					NotificationSystem.AddNotificationExtended(4.0, "#lb_title_admin", "#lb_message_admin_enabled", LBIconConfig.Get.info);
				} else {
					NotificationSystem.AddNotificationExtended(4.0, "#lb_title_admin", "#lb_message_admin_disabled", LBIconConfig.Get.info);
				}
			}
		}
	}

#endif

	bool IsLBAdminMenuOpen() {
		return LBAdmin_Menu.Cast(LBMenuManager.Get().GetOpenLBMenu()) != null;
	}

	void RegisterLBMenu(string name, string openUIInputName) {
		UAInput input = GetUApi().GetInputByName(openUIInputName);
		LBLogger.Info("Registered Menu " + name + " for input " + openUIInputName + " Found: " + input, "Core");
		if (input)
			lbRegisteredMenus.Insert(new Param2<UAIDWrapper, string>(input.GetPersistentWrapper(), name));
	}

	void ClearRegisteredMenus() {
		lbRegisteredMenus.Clear();
	}

	LBAdmin_Menu_Page GetPage(string page) {
		LBAdmin_Menu lbAdminMenu = LBAdmin_Menu.Cast(LBMenuManager.Get().GetOpenLBMenu());
		return lbAdminMenu.GetPage(page);
	}

	LBAdmin_Menu_Page ShowLBmasterAdminMenu(string activeCategory = "") {
		LBAdmin_Menu lbAdminMenu = LBAdmin_Menu.Cast(LBMenuManager.Get().GetOpenLBMenu());
		if (lbAdminMenu) {
			return lbAdminMenu.SetPageActive(activeCategory);
		} else {
			lbAdminMenu = LBAdmin_Menu.Cast(LBMenuManager.Get().OpenMenu("LBAdmin_Menu"));
			foreach (LBRegisteredAdminPage registeredPage : lbRegisteredAdminPages) {
				lbAdminMenu.RegisterPage(registeredPage.param1, registeredPage.param2, registeredPage.param3, registeredPage.param4, registeredPage.param5);
			}
			lbAdminMenu.OnPageRegisterFinished();
			return lbAdminMenu.SetPageActive(activeCategory);
		}
	}

	override void OnEvent(EventType eventTypeId, Param params) {
		super.OnEvent(eventTypeId, params);

		switch (eventTypeId) {
			case WindowsResizeEventTypeID:
				LBWidgetUtils.UpdateScreenDimensions();
				if (g_Game.GetGameState() == DayZGameState.IN_GAME) {
					LBMenuManager.Get().CloseAllMenus(null, false);
					LBMenuManager.Get().persistentMenus.Clear();
				}
				break;
		}
	}

}