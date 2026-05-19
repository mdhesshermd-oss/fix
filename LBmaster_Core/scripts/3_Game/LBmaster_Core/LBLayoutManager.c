class LBLayoutManager {

	static ref LBLayoutManager g_LBLayoutManager;

	static LBLayoutManager Get() {
		if (!g_LBLayoutManager)
			g_LBLayoutManager = new LBLayoutManager();
		return g_LBLayoutManager;
	}

	ref map<string, string> registeredLayouts = new map<string, string>();

	void LBLayoutManager() {
		RegisterLayout("AdminMenu", "LBmaster_Core/gui/layouts/adminmenu/adminMenu.layout");
		RegisterLayout("AdminMenu_CurrenciesAdminMenu", "LBmaster_Core/gui/layouts/adminmenu/currenciesAdmin.layout");
		RegisterLayout("AdminMenu_LBUpdateCheckerMenu", "LBmaster_Core/gui/layouts/adminmenu/updates.layout");
		RegisterLayout("AdminMenu_LB_Item_PlaceMenu", "LBmaster_Core/gui/layouts/adminmenu/placeMenu.layout");
		RegisterLayout("AdminMenu_LB_PlacedItemsOverviewMenu", "LBmaster_Core/gui/layouts/adminmenu/adminPlacedItems.layout");
		RegisterLayout("AdminMenu_LBWebhookAdminMenu", "LBmaster_Core/gui/layouts/adminmenu/adminwebhooks.layout");
		RegisterLayout("AdminMenu_LBPlayerListAdminMenu", "LBmaster_Core/gui/layouts/adminmenu/adminPlayerList.layout");
		RegisterLayout("AdminMenu_SpawnMenuLoadoutEditor", "LBmaster_Core/GUI/layouts/adminmenu/spawnloadouts.layout");
		RegisterLayout("AdminMenu_LBConfigReloadMenu", "LBmaster_Core/GUI/layouts/adminmenu/adminMenuReloadConfig.layout");
		RegisterLayout("AdminMenu_AdminPermissionMenu", "LBmaster_Core/GUI/layouts/adminmenu/adminMenuAdmins.layout");
		RegisterLayout("AdminMenu_ErrorLogAdminMenu", "LBmaster_Core/GUI/layouts/adminmenu/errorLogsViewer.layout");
		RegisterLayout("AdminMenu_LBIconSettingsMenu", "LBmaster_Core/GUI/layouts/adminmenu/iconSettings.layout");
		RegisterLayout("AdminMenu_LBLoggingSettingsMenu", "LBmaster_Core/GUI/layouts/adminmenu/loggingSettings.layout");

		RegisterLayout("WarningMessage", "LBmaster_Core/gui/layouts/menu/warningMessage.layout");
		RegisterLayout("FileSelector", "LBmaster_Core/gui/layouts/menu/selectFileDialog.layout");
		RegisterLayout("ItemSelector", "LBmaster_Core/gui/layouts/menu/selectItemDialog.layout");
		RegisterLayout("PlayerSelector", "LBmaster_Core/gui/layouts/menu/selectPlayerDialog.layout");
		RegisterLayout("ListSelector", "LBmaster_Core/gui/layouts/menu/selectListDialog.layout");
		RegisterLayout("SimplePopupMessage", "LBmaster_Core/gui/layouts/menu/simpleInputPopup.layout");
		RegisterLayout("HologramPlacementInfo", "LBmaster_Core/gui/layouts/menu/hologramPlacementInfo.layout");

		RegisterLayout("LBQRCodeTestMenu", "LBmaster_Core/GUI/layouts/helper/qrcode-test.layout");
		RegisterLayout("AdminMenuButton", "LBmaster_Core/gui/layouts/helper/adminMenuDropdownButton.layout");
		RegisterLayout("AdminButtonImage", "LBmaster_Core/gui/layouts/helper/adminMenuImage.layout");
		RegisterLayout("AdminButtonDropdown", "LBmaster_Core/gui/layouts/helper/adminMenuDropdown.layout");
		RegisterLayout("AutoComplete", "LBmaster_Core/gui/layouts/helper/autocompleteoverlay.layout");
		RegisterLayout("ButtonOverlay", "LBmaster_Core/gui/layouts/helper/buttonOverlay.layout");
		RegisterLayout("EditBoxOverlay", "LBmaster_Core/gui/layouts/helper/textOverlay.layout");
		RegisterLayout("DragListEntry", "LBmaster_Core/gui/layouts/helper/dragEntry.layout");
		RegisterLayout("Tooltip", "LBmaster_Core/gui/layouts/helper/tooltip.layout");
		RegisterLayout("LBMultiLine", "LBmaster_Core/gui/layouts/helper/multiline.layout");
		RegisterLayout("LBMultiLineEmpty", "LBmaster_Core/gui/layouts/helper/multilineempty.layout");
		RegisterLayout("Underline", "LBmaster_Core/gui/layouts/helper/underline.layout");
		RegisterLayout("Image", "LBmaster_Core/gui/layouts/helper/image.layout");
		RegisterLayout("WrapSpacer", "LBmaster_Core/gui/layouts/helper/wrapSpacer.layout");
		RegisterLayout("ColorPicker", "LBmaster_Core/gui/layouts/helper/colorpicker.layout");
		RegisterLayout("SimpleCheckbox", "LBmaster_Core/gui/layouts/helper/checkboxWidget.layout");
		RegisterLayout("DayZConfigOption", "LBmaster_Core/gui/layouts/helper/dayz_options_entry.layout");
		RegisterLayout("LBCopiedPopup", "LBmaster_Core/gui/layouts/helper/simpleCopiedPopup.layout");
		RegisterLayout("LBLoadingImage", "LBmaster_Core/gui/layouts/helper/loading.layout");
		RegisterLayout("ClearButton", "LBmaster_Core/gui/layouts/helper/btn_clear.layout");
		RegisterLayout("EmptyCanvas", "LBmaster_Core/gui/layouts/helper/emptycanvas.layout");
	}

	Widget CreateLayout(string name, Widget parent = null, bool immedUpdate = true) {
		string path = GetLayoutPath(name);
		if (!g_Game || !g_Game.GetWorkspace())
			return null;
		Widget w = g_Game.GetWorkspace().CreateWidgets(path, parent, immedUpdate);
		if (!w)
			LBLogger.Error("Could not create layout for " + name + " Path: " + path, "Core");
		return w;
	}

	void RegisterLayout(string name, string path) {
		registeredLayouts.Insert(name, path);
	}

	void OverwriteLayout(string name, string path) {
		registeredLayouts.Set(name, path);
	}

	// Register the layout overwrites like this
	/*
	modded class LBLayoutManager {

		void LBLayoutManager() {
			// To overwrite an existing layout
			OverwriteLayout("LayoutA", "Path/To/layout/a.layout");
			OverwriteLayout("LayoutB", "Path/To/layout/b.layout");
			// If you want to register your own layout
			RegisterLayout("LayoutC", "Path/To/layout/c.layout");
		}

	}
	*/
	string GetLayoutPath(string name) {
		return GetLayoutPathOriginal(name);
	}

	// Do not overwrite
	// This is only for the LBmaster mod to get the path. All overwrites are handled through the OverwriteLayout(name, path) Method in the Constructor of this class
	string GetLayoutPathOriginal(string name) {
		string path;
		if (registeredLayouts.Find(name, path))
			return path;
		return "";
	}

}