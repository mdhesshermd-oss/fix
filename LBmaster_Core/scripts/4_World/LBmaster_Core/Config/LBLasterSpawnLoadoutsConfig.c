class LBLasterSpawnLoadoutsConfig : LBConfigLoader<LBLasterSpawnLoadoutsConfig_> {

	override void InitVars() {
		InitVarsInternal("Core", "LastLoadouts.json", LBConfigType.DATA, true, "core.change.presets", "core.change.presets"); // (don't touch)
	}

}
// This file keeps track of the last loadout a player was spawned with when his Character spawned. It saves 1.6 seconds after the spawning to allow for mods to spawn related items on the player.
// This is used for the Loadout Editor to preview the player when the inventory is not cleared
class LBLasterSpawnLoadoutsConfig_ : LBConfigBase {

	static const int CURRENT_VERSION = 1;
	ref LBSpawnLoadoutEntry rootEntry = new LBSpawnLoadoutEntry(); // Root Entry for the last Loadout (Player Object)

	void SaveLastPlayerDelayed(EntityAI player) {
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SaveLastPlayer, 1600, false, player);
	}

	void SaveLastPlayer(EntityAI player) {
		if (!player)
			return;
		rootEntry = LBSpawnLoadoutEntry.SaveEntry(player);
		LBLasterSpawnLoadoutsConfig.Loader.Save();
	}

}