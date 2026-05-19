class HoverLootConfig : LBConfigLoader<HoverLootConfig_> {

	override void InitVars() {
		InitVarsInternal("HoverLoot", "config.json", LBConfigType.CONFIG, true, "hoverloot.change.config"); // (Easy)
	}

}
// A very simple config for changing a few variables for the mod
class HoverLootConfig_ : LBConfigBase {

	static const int CURRENT_VERSION = 1;
	float lootTimerSeconds = 0.2; // Time in seconds between automatic item pickups. Do not set this to 0!!
	ref LBColorConfig backgroundColor = LBColorConfig.Init(120, 255, 50, 50); // Background Color for currently waiting items. You can set the alpha value to 0 to not have any background color

}