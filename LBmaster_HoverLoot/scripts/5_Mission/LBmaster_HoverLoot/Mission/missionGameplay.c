modded class MissionGameplay {

	override void OnUpdate(float timeslice) {
		super.OnUpdate(timeslice);
		VicinityItemManager mgr = VicinityItemManager.GetInstance();
		if (mgr) {
			HoverLootManager.vicinityEntities = mgr.GetVicinityItems();
			HoverLootManager.vicinityCargos = mgr.GetVicinityCargos();
		}
		HoverLootManager.OnUpdateStatic(timeslice);
	}

}