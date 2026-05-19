[DayZGame.RegisterLBAdminPage("LBIconSettingsMenu", "LBmaster_Core/logo/logo.paa", "Message Icons", "core.change.icons")]
class LBIconSettingsMenu : LBAdmin_Menu_Page {

	ButtonWidget btn_error_files, btn_error_imgs, btn_warn_files, btn_warn_imgs, btn_info_files, btn_info_imgs, btn_applied_files, btn_applied_imgs, btn_abort_files;
	ButtonWidget btn_abort_imgs, btn_building_files, btn_building_imgs;
	ImageWidget img_preview;
	EditBoxWidget edit_error, edit_warning, edit_info, edit_applied, edit_abort, edit_building;

	override void OnShow() {
		linked.LoadLinkedVars();
	}

	override array<string> GetEditedConfigs() {
		return {"LBIconConfig"};
	}

	override void RegisterAllLinkedVars() {
		TStringArray imageSets = LBConfigFinder.LoadAllImageSetsAsList();
		linked.RegisterLinkedVar("GetConfig().error", edit_error).EnableFileSelector({".paa", ".edds"}, btn_error_files).EnableListBrowse(imageSets, btn_error_imgs, "Select an Imageset Image from the list");
		linked.RegisterLinkedVar("GetConfig().warning", edit_warning).EnableFileSelector({".paa", ".edds"}, btn_warn_files).EnableListBrowse(imageSets, btn_warn_imgs, "Select an Imageset Image from the list");
		linked.RegisterLinkedVar("GetConfig().info", edit_info).EnableFileSelector({".paa", ".edds"}, btn_info_files).EnableListBrowse(imageSets, btn_info_imgs, "Select an Imageset Image from the list");
		linked.RegisterLinkedVar("GetConfig().applied", edit_applied).EnableFileSelector({".paa", ".edds"}, btn_applied_files).EnableListBrowse(imageSets, btn_applied_imgs, "Select an Imageset Image from the list");
		linked.RegisterLinkedVar("GetConfig().abort", edit_abort).EnableFileSelector({".paa", ".edds"}, btn_abort_files).EnableListBrowse(imageSets, btn_abort_imgs, "Select an Imageset Image from the list");
		linked.RegisterLinkedVar("GetConfig().building", edit_building).EnableFileSelector({".paa", ".edds"}, btn_building_files).EnableListBrowse(imageSets, btn_building_imgs, "Select an Imageset Image from the list");
	}

	LBIconConfig_ GetConfig() {
		return LBIconConfig.Get;
	}

	override bool OnMouseEnter(Widget w, int x, int y) {
		if (super.OnMouseEnter(w, x, y))
			return true;
		EditBoxWidget edit = EditBoxWidget.Cast(w);
		if (edit) {
			img_preview.LoadImageFile(0, edit.GetText());
		}
		return false;
	}

}