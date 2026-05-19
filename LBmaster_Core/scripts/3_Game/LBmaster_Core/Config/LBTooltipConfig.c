class LBTooltipConfig : LBConfigLoader<LBTooltipConfig_> {

	override void InitVars() {
		InitVarsInternal("Core", "tooltipconfig.json");
	}

	override bool IsClientSideConfig() {
		return true;
	}

}
class LBTooltipConfig_ : LBConfigBase {

	static const int CURRENT_VERSION = 1;
	bool enabled = true;

}