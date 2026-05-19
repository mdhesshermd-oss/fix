modded class VicinityItemManager {

	ref array<EntityAI> dummyArray;

	override array<EntityAI> GetVicinityItems() {
		if (MissionGameplay.dummyVicinityItem) {
			dummyArray = {MissionGameplay.dummyVicinityItem};
			return dummyArray;
		} else {
			return super.GetVicinityItems();
		}
	}

}