class LBAppearanceConfig : LBConfigLoader<LBAppearanceConfig_> {

	override void InitVars() {
		InitVarsInternal("Common", "Appearance.json", LBConfigType.CONFIG, true, "core.change.appearancesettings"); // (easy)
	}

}
// This file contains the path to your logo, which will be displayed on some mod menus. The default path is set to `LB_Server_Logo/gui/images/logo.paa`
// A PBO with the logo inside at this location can easily be generated via [The Logo Generator](https://lbmaster.de/logo/)
// Follow the instructions there to generate the PBO
class LBAppearanceConfig_ : LBConfigBase {

	static const int CURRENT_VERSION = 0;

	private string logoPath = "LB_Server_Logo/gui/images/logo.paa"; // Path to the logo displayed in some mod menus. Currently only used by Advanced Groups and Virtual Garage.

	void LoadLogo(ImageWidget widget) {
		if (!widget)
			return;
		bool exists = FileExist(logoPath);
		LBLogger.Debug("Loading Logo: " + logoPath + ". Exists ? " + exists, "Core");
		widget.LoadImageFile(0, logoPath);
		widget.Show(exists);
	}

}