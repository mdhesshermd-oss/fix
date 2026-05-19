class LBNewModsConfig : LBConfigLoader<LBNewModsConfig_> {

	override void InitVars() {
		InitVarsInternal("Common", "newMods.json");
	}

	override bool IsClientSideConfig() {
		return true;
	}

}
class LBNewModsConfig_ : LBConfigBase {

	static const int CURRENT_VERSION = 1;
	ref TStringSet ignoredMods = new TStringSet();

}