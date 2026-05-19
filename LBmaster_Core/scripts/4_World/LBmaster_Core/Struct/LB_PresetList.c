class LB_PresetList : LB_PresetBase {

	bool root = false; // Set this to 1 if the preset should be visible in the Spawn Select admin menu for adding it to the loadouts list of a spawn to not have too many entries in the list. Some of the Presets will be used as intermediate Presets like `Preset_Military_Clothing`, which then contains more different clothing presets to allow more precise controll over which items to spawn with their chance

	static LB_PresetList InitList(string setName, bool individual, float chance_) {
		LB_PresetList preset = new LB_PresetList();
		preset.presetName = setName;
		preset.individualChance = individual;
		preset.chance = chance_;
		return preset;
	}

	LB_PresetList Add(string item) {
		items.Insert(item);
		return this;
	}

	LB_PresetList SetRoot(bool root_) {
		this.root = root_;
		return this;
	}

	string ToListString(int column) {
		return presetName;
	}

}