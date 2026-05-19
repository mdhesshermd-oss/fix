modded class MissionGameplay {

	static EntityAI dummyVicinityItem = null;

	void MissionGameplay() {
		g_Game.InitRegisteredRPCHandlersLB();
		LBConfigManager.Get().LoadAllConfigs();
		LBInherit.Get();
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Call(OnInitLB);

		g_Game.RPCSingleParam(null, LBmaster_Core_RPCs.OBFUSCATION_RANDOM_SYNC, new Param1<bool>(true), true);
		dummyVicinityItem = null;
		LBConfigInfo.Get();
		LB_ElevatorInitMission();
	}

	void OnInitLB() {
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Call(LBConfigManager.Get().OnMissionInit);
		LBGroupInfoShare.playerGroups = new map<string, ref Param2<string, string>>();
		g_Game.RPCSingleParam(null, LBmaster_Core_RPCs.CLIENT_MISSION_INITIALIZED, new Param1<bool>(true), true);
		LBLogger.Debug("Mission Base init finished", "Core");
	}

	override void CloseAllMenus() {
		LBMenuManager.Get().CloseAllMenus(null, false);
		super.CloseAllMenus();
	}

	void ~MissionGameplay() {
		LBLogger.Debug("Mission Base Deleted", "Core");
		LBAdmins.Delete();
		if (g_Game) {
			g_Game.ClearRegisteredMenus();
			g_Game.ClearRegisteredRPCs();
		}
		LBWarningPopup.Delete();
		LBConfigManager.Delete();
		LBMenuManager.Delete();
		LB_Elevator.Set(new LB_Elevator_Game());
	}
}
