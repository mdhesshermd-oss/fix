modded class LBConfigInfo {
	void LBConfigInfo() {
		RegisterClassComment("HoverLootConfig", "A very simple config for changing a few variables for the mod");
		RegisterClassVariableComment("HoverLootConfig", "lootTimerSeconds", "Time in seconds between automatic item pickups. Do not set this to 0!!");
		RegisterClassVariableComment("HoverLootConfig", "backgroundColor", "Background Color for currently waiting items. You can set the alpha value to 0 to not have any background color");
	}
}