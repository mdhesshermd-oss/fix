typedef Param3<int, string, LBMenuData> LBForceOpenMenuParam;
class LBMenuManager : Managed {

	static ref LBMenuManager g_LBMenuHandler;

	static LBMenuManager Get() {
		if (!g_LBMenuHandler)
			g_LBMenuHandler = new LBMenuManager();
		return g_LBMenuHandler;
	}

	static void Delete() {
		g_LBMenuHandler = null;
	}

	ref array<ref LBMenuBase> menuInstances = new array<ref LBMenuBase>();
	ref array<ref LBMenuData> menuParams = new array<ref LBMenuData>();
	ref array<ref LBForceOpenMenuParam> forceOpenMenus = new array<ref LBForceOpenMenuParam>();
	ref array<ref LBMenuBase> persistentMenus = new array<ref LBMenuBase>();

	ref Timer forceOpenTimer = new Timer();

	void LBMenuManager() {
		forceOpenTimer.Run(0.1, this, "TryOpenForcedMenu", null, true);
	}

	void ~LBMenuManager() {
#ifndef SERVER
		if (forceOpenTimer)
			forceOpenTimer.Stop();
		if (!g_Game)
			return;
		CloseAllMenus();
		persistentMenus.Clear();
#endif
	}

	LBMenuBase TryOpenForcedMenu() {
		if (!forceOpenMenus || forceOpenMenus.Count() == 0)
			return null;
		if (!g_Game.GetPlayer() || !g_Game.GetPlayer().IsAlive()) {
			CloseAllMenus();
			return null;
		}
		LBForceOpenMenuParam mostImportant = null;
		foreach (LBForceOpenMenuParam force : forceOpenMenus) {
			if (mostImportant == null || mostImportant.param1 < force.param1) {
				mostImportant = force;
				LBLogger.Verbose("Force Open found candidate " + mostImportant.param2 + " of " + forceOpenMenus.Count() + " candidates with Parameter " + mostImportant.param3 + " and Prio " + mostImportant.param1, "Core");
			}
		}

		LBMenuBase menu = OpenMenu(mostImportant.param2, mostImportant.param3, true, null, false);
		if (menu)
			forceOpenMenus.RemoveItem(mostImportant);
		return menu;
	}

	LBMenuBase GetPersistentMenu(string classname) {
		foreach (LBMenuBase menu : persistentMenus) {
			if (menu && menu.ClassName() == classname) {
				return menu;
			}
		}
		typename tn = classname.ToType();
		if (!tn || !tn.IsInherited(LBMenuBase)) {
			LBLogger.Error("Could not get menu classname " + classname, "Core");
			return null;
		}
		Class obj = tn.Spawn();
		LBMenuBase lbMenu = LBMenuBase.Cast(obj);
		if (lbMenu && lbMenu.SavePersistentState()) {
			LBLogger.Debug("Created first instance of Persistent Menu: " + menu + " for " + classname, "Core");
			persistentMenus.Insert(lbMenu);
		}
		return lbMenu;
	}

	void CloseAllMenus(PlayerIdentity target = null, bool clearAll = true) {
#ifdef SERVER
		g_Game.RPCSingleParam(null, LBmaster_Core_RPCs.CLOSE_ALL_MENUS, new Param1<bool>(clearAll), true, target);
#else
		LBLogger.Info("Closing all menus...", "Core");
		if (g_Game && g_Game.GetUIManager()) {
			g_Game.GetUIManager().CloseAll();
			GetElevatorGame().ShowIngameHud(true);
			Mission mission = g_Game.GetMission();
			if (mission) {
				mission.PlayerControlEnable(true);
				mission.RemoveActiveInputExcludes({"menu", "movement", "aiming"}, true);
				mission.RefreshExcludes();
				mission.HideInventory();
			}
		}
		if (clearAll) {
			menuInstances.Clear();
			forceOpenMenus.Clear();
		}
#endif
	}

	void ClearMenus(string menu) {
		for (int i = 0; i < forceOpenMenus.Count(); ++i) {
			if (forceOpenMenus.Get(i).param2 == menu) {
				forceOpenMenus.Remove(i);
				--i;
			}
		}
		LBMenuBase open = GetOpenLBMenu();
		if (open && open.ClassName() == menu) {
			open.SetForceOpen(false);
		}
	}

	void ForceOpenMenu(int prio, string menu, LBMenuBase lbMenu) {
		int index = menuInstances.Find(lbMenu);
		if (index == -1)
			return;
		forceOpenMenus.Insert(new LBForceOpenMenuParam(prio, menu, lbMenu.openParameter));
	}

	void OpenMenuRemote(PlayerIdentity target, string menu, LBMenuData data = null, bool forceOpen = false, bool clearOld = true, bool keepParent = false) {
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(menu);
		if (data) {
			string tn = data.Type().ToString();
			rpc.Write(tn);
			data.WriteToCtx(rpc);
		} else {
			tn = "null";
			rpc.Write(tn);
		}
		rpc.Write(forceOpen);
		rpc.Write(clearOld);
		rpc.Write(keepParent);
		rpc.Send(null, LBmaster_Core_RPCs.OPEN_MENU, true, target);
	}

	array<ref LBMenuBase> GetActiveMenus() {
		for (int i = 0; i < menuInstances.Count(); ++i) {
			if (!menuInstances.Get(i)) {
				menuInstances.Remove(i);
				--i;
			}
		}
		return menuInstances;
	}

	void OnRPC(Object target, int rpc_type, ParamsReadContext ctx) {
		array<ref LBMenuBase> menus = GetActiveMenus();
		foreach (LBMenuBase menu : menus) {
			if (!menu.SavePersistentState())
				menu.OnRPC(target, rpc_type, ctx);
		}
		foreach (LBMenuBase menu2 : persistentMenus) {
			if (menu2)
				menu2.OnRPC(target, rpc_type, ctx);
		}
	}

	LBMenuBase OpenMenu(string menuClassname, LBMenuData openParameter = null, bool forceOpen = false, UIScriptedMenu parent = null, bool doForceOpenCheck = true, bool clearOldFromSameType = false) {
		LBLogger.Verbose("Opening Menu: " + menuClassname + " with Parameter " + openParameter + " Force Open: " + forceOpen + " Parent: " + parent, "Core");
		LBMenuBase lbMenu = GetPersistentMenu(menuClassname);

		menuParams.Insert(openParameter);
		lbMenu.openParameter = openParameter;
		if (lbMenu == GetOpenLBMenu()) {
			lbMenu.SetForceOpen(forceOpen);
			lbMenu.OnDataReceived();
			return lbMenu;
		}

		if (clearOldFromSameType)
			ClearMenus(menuClassname);
		if (!lbMenu.SavePersistentState()) {
			if (doForceOpenCheck) {
				if (forceOpen) {
					menuInstances.Insert(lbMenu);
					ForceOpenMenu(lbMenu.GetPriority(), menuClassname, lbMenu);
				}
				LBMenuBase force = TryOpenForcedMenu();
				if (force) {
					menuInstances.RemoveItem(lbMenu);
					return force;
				}
			}
			if (!CanOpenMenu(lbMenu))
				return null;
		}
		menuInstances.Insert(lbMenu);
		if (!parent)
			g_Game.GetUIManager().CloseAll();
		g_Game.GetUIManager().ShowScriptedMenu(lbMenu, parent);
		lbMenu.SetForceOpen(forceOpen);
		lbMenu.OnDataReceivedFirst();
		return lbMenu;
	}

	private bool CanOpenMenu(LBMenuBase menu) {
		if (!g_Game || !g_Game.GetUIManager() || !menu.CanShowMenu())
			return false;
		LBMenuBase openMenu = GetOpenLBMenu();
		return !openMenu || openMenu.GetPriority() <= menu.GetPriority();
	}

	LBMenuBase GetOpenLBMenu() {
		return LBMenuBase.Cast(g_Game.GetUIManager().GetMenu());
	}

}
