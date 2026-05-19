class LB_Presets {

	string name = "NAME"; // Name of this category. Be aware, that in the end all categories will be merged and no duplicate names are allowed across different categories!
	ref array<ref LB_PresetList> base = new array<ref LB_PresetList>(); // List of presets, which can contain other presets or items. If you want to spawn items, you need to define a preset in the items list for each Item !
	ref array<ref LB_ItemPreset> items = new array<ref LB_ItemPreset>(); // the List of items, which can have other presets as children to allow either cargo items or attachments or even weapon magazines

	string ToListString(int column) {
		return name;
	}

}