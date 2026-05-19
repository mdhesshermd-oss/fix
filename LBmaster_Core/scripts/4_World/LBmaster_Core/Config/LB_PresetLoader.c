
class LB_PresetLoader : LBConfigLoader<LB_PresetLoader_> {

	override void InitVars() {
		InitVarsInternal("Common", "ItemPresets.json", LBConfigType.CONFIG, true, "core.change.presets", "core.change.presets"); // (hard)
	}

	static ref TierMapReader _reader;
}
// This file defines the Presets for the spawn select mod, which can be added to the layouts either via the config, or the ingame admin menu, which is recommended.
// **Important**: Make sure you don't have any circular items in the presets ! Otherwise the Server might end up in an infinite loop when trying to spawn a preset. Example `Preset_Weapons_Medium` in `Preset_Military` in `Preset_Weapons_Medium`
// I would highly recommend using the ingame Loadout editor to create your presets.
// If you never used the Loadout Editor or setup loot presets, I would recommend watching my [Video Tutorials](https://www.youtube.com/watch?v=_Tw7BVqP_CU&list=PLbFzn5xhrn-JF19bTdmt5mjtS7iCzoy7y)
class LB_PresetLoader_ : LBConfigBase {

	static const int CURRENT_VERSION = 3;

	[NonSerialized()]
	private ref map<string, ref LB_PresetBase> allPresets = new map<string, ref LB_PresetBase>();
	[NonSerialized()]
	private ref TStringArray checkPresetNames = new TStringArray();
	[NonSerialized()]
	ref TStringArray rootLayouts = new TStringArray();
	[NonSerialized()]
	static ref TStringArray errors = new TStringArray();

	ref array<ref LB_ItemPreset> defaultItemConfigs = new array<ref LB_ItemPreset>(); // Default config used when a preset was not configured for an item. itemname is ignored. to use it, the preset must not exist and match itemname + presetName of this preset at the end. E.g. Item: "Battery". Presetname is "_ATT". Adding "Battery_ATT" as the item of another preset, will apply the default preset
	ref array<ref LB_Presets> presetLists = new array<ref LB_Presets>(); // You can organize your presets into different categories. Be aware, that all presets are going to be merged into one big list internally, so no duplicate names across different categories are allowed! I highly advise to use a consistent prefix for each item and preset in each category. E.g. Category Weapons always uses WPN_ as a prefix

	override bool OnLoad() {
#ifdef SERVER
		if (!LB_PresetLoader._reader)
			LB_PresetLoader._reader = new TierMapReader();
#endif
		errors = new TStringArray();
		allPresets.Clear();
		checkPresetNames.Clear();
		foreach (LB_Presets category2 : presetLists) {
			foreach (LB_PresetBase preset : category2.base) {
				allPresets.Insert(preset.presetName, preset);
				checkPresetNames.Insert(preset.presetName);
			}
			foreach (LB_PresetBase preset2 : category2.items) {
				allPresets.Insert(preset2.presetName, preset2);
				checkPresetNames.Insert(preset2.presetName);
			}
		}
		LinkAllPresets();
		FillAllLoadouts();
		TStringSet allItems = LBInherit.Get().GetAllChildren(null, true, true, false, 2);
		foreach (LB_Presets category3 : presetLists) {
			foreach (LB_PresetBase preset3 : category3.base) {
				if (preset3.CanSpawnAllItems(allItems) == LBPresetTestResult.INFINITE_LOOP) {
					LBLogger.Fatal("Infinite Loop found in Item Preset " + preset3.presetName, "Core");
					LB_PresetLoader_.errors.Insert("Infinite Loop found in Item Preset " + preset3.presetName);
				}
			}
			foreach (LB_PresetBase preset4 : category3.items) {
				if (preset4.CanSpawnAllItems(allItems) == LBPresetTestResult.INFINITE_LOOP) {
					LBLogger.Fatal("Infinite Loop found in Preset " + preset4.presetName, "Core");
					LB_PresetLoader_.errors.Insert("Infinite Loop found in Preset " + preset4.presetName);
				}
			}
		}
		return false;
	}

	override void UpdateVersion() {
		if (version < 2) {
			defaultItemConfigs = new array<ref LB_ItemPreset>();
			defaultItemConfigs.Insert(LB_ItemPreset.InitItem("", false, 1.0, ""));
			defaultItemConfigs.Insert(LB_ItemPreset.InitItem("_IND", true, 1.0, ""));
			defaultItemConfigs.Insert(LB_ItemPreset.InitItem("_90%", true, 0.9, ""));
			defaultItemConfigs.Insert(LB_ItemPreset.InitItem("_80%", true, 0.8, ""));
			defaultItemConfigs.Insert(LB_ItemPreset.InitItem("_70%", true, 0.7, ""));
			defaultItemConfigs.Insert(LB_ItemPreset.InitItem("_60%", true, 0.6, ""));
			defaultItemConfigs.Insert(LB_ItemPreset.InitItem("_50%", true, 0.5, ""));
			defaultItemConfigs.Insert(LB_ItemPreset.InitItem("_40%", true, 0.4, ""));
			defaultItemConfigs.Insert(LB_ItemPreset.InitItem("_30%", true, 0.3, ""));
			defaultItemConfigs.Insert(LB_ItemPreset.InitItem("_20%", true, 0.2, ""));
			defaultItemConfigs.Insert(LB_ItemPreset.InitItem("_10%", true, 0.1, ""));
			defaultItemConfigs.Insert(LB_ItemPreset.InitItem("_MAG", true, 1.0, "", false, "mag"));
			defaultItemConfigs.Insert(LB_ItemPreset.InitItem("_ROOT", true, 1.0, "", false, "root"));
			defaultItemConfigs.Insert(LB_ItemPreset.InitItem("_BAT", false, 1.0, "").Add("Battery9V"));
			defaultItemConfigs.Insert(LB_ItemPreset.InitItem("_BAT_IND", true, 1.0, "").Add("Battery9V"));
		}
		if (version < 3) {
			foreach (LB_PresetBase preset2 : defaultItemConfigs) {
				preset2.minSpawnTries = 1;
				preset2.maxSpawnTries = 1;
				preset2.allowDuplicates = true;
			}

			foreach (LB_Presets category2 : presetLists) {
				foreach (LB_PresetBase preset3 : category2.base) {
					preset3.minSpawnTries = 1;
					preset3.maxSpawnTries = 1;
					preset3.allowDuplicates = true;
				}
				foreach (LB_PresetBase preset4 : category2.items) {
					preset4.minSpawnTries = 1;
					preset4.maxSpawnTries = 1;
					preset4.allowDuplicates = true;
				}
			}
		}
	}

	override void OnReceivedFromRPC(PlayerIdentity sender) {
		OnLoad();
	}

	LB_Presets GetCategory(string name, bool createNotFound = false) {
		foreach (LB_Presets category2 : presetLists) {
			if (category2.name == name)
				return category2;
		}
		category2 = new LB_Presets();
		category2.name = name;
		presetLists.Insert(category2);
		return category2;
	}

	void AddNewBasePreset(string name, string category) {
		LB_PresetList preset = new LB_PresetList();
		preset.presetName = name;
		GetCategory(category, true).base.Insert(preset);
		allPresets.Insert(preset.presetName, preset);
		checkPresetNames.Insert(preset.presetName);
	}

	LB_PresetBase GetPresetAt(string name, vector position) {
		if (name.IndexOf("$TIER$")) {
			TStringArray tierGroups = LB_PresetLoader._reader.GetTierGroupsAt(position);
			if (tierGroups.Count() == 0) {
				LBLogger.Error("Could not find Tier Value at " + position, "Core");
				name.Replace("$TIER$", "NONE");
			} else {
				LBLogger.Verbose("Found Tier Value at " + position + ": " + tierGroups.Get(0) + " (" + tierGroups.Count() + ")", "Core");
				name.Replace("$TIER$", tierGroups.Get(0));
			}
		}
		return GetPreset(name);
	}

	LB_PresetBase GetPreset(string name) {
		LB_PresetBase found = allPresets.Get(name);
		if (found)
			return found;
		foreach (LB_ItemPreset defaultItem : defaultItemConfigs) {
			int len = defaultItem.presetName.Length();
			if (defaultItem.presetName == "" || LBStringTools.EndsWith(name, defaultItem.presetName)) {
				string itemname = name.Substring(0, name.Length() - len);
				if (LBInherit.Get().GetEntry(itemname)) {
					LB_ItemPreset newItem = defaultItem.CopyToNew(itemname, name);
					allPresets.Insert(name, newItem);
					checkPresetNames.Insert(name);
					string printName = defaultItem.presetName;
					if (printName.IndexOf("%") != -1)
						printName.Replace("%", "");
					LBLogger.Debug("Created Preset from Default Entry: " + printName + " for " + name + " itemname: " + itemname, "Core");
					return newItem;
				}
			}
		}
		return null;
	}

	void FillAllLoadouts() {
		rootLayouts.Clear();
		foreach (string name, LB_PresetBase preset : allPresets) {
			LB_PresetList list = LB_PresetList.Cast(preset);
			if (list && list.root)
				rootLayouts.Insert(name);
		}
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(rootLayouts);
		LBAdmins.Get().SendRPCToAdminsWithPermission("core.toggle.adminmode", rpc, LBmaster_Core_RPCs.SYNC_AVAILABLE_LOADOUTS);
	}

	void SendRootLayoutsTo(PlayerIdentity player) {
		if (!LBAdmins.Get().HasPermission("core.toggle.adminmode", player))
			return;
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(rootLayouts);
		rpc.Send(null, LBmaster_Core_RPCs.SYNC_AVAILABLE_LOADOUTS, true, player);
	}

	array<LB_PresetBase> FindPresets(TStringArray arr) {
		array<LB_PresetBase> presets = new array<LB_PresetBase>();
		if (arr == null)
			return presets;
		foreach (string name : arr) {
			LB_PresetBase preset = GetPreset(name);
			if (preset)
				presets.Insert(preset);
		}
		return presets;
	}

	void LinkAllPresets() {
		foreach (LB_Presets category2 : presetLists) {
			foreach (LB_ItemPreset itemPreset : category2.items) {
				itemPreset.Invalidate();
			}
			foreach (LB_PresetBase presetBase : category2.base) {
				presetBase.Invalidate();
			}
		}
		foreach (LB_PresetBase presetDefault : defaultItemConfigs) {
			presetDefault.Invalidate();
		}
		for (int i = 0; i < checkPresetNames.Count(); ++i) {
			string name = checkPresetNames.Get(i);
			LB_PresetBase preset = GetPreset(name);
			preset.Initialize(this, errors, name);
			//LBLogger.Verbose("Linked " + preset.linked.Count() + " Presets for " + name + " of " + preset.items.Count(), "Core");
		}
	}

	override void LoadDefault() {
		string file = LB_PresetLoader.Loader.GetFileFolder() + "/LootPresets.json";
		if (FileExist(file)) {
			LB_PresetsMigration mig;
			LBJsonLoader<LB_PresetsMigration>.JsonLoadFile(file, mig, true);
			defaultItemConfigs = mig.defaultItemConfigs;
			LB_Presets category = new LB_Presets();
			category.name = "Default";
			category.base = mig.base;
			category.items = mig.items;
			presetLists.Insert(category);
			DeleteFile(file);
			return;
		}

		defaultItemConfigs = new array<ref LB_ItemPreset>();
		defaultItemConfigs.Insert(LB_ItemPreset.InitItem("", false, 1.0, ""));
		defaultItemConfigs.Insert(LB_ItemPreset.InitItem("_IND", true, 1.0, ""));
		defaultItemConfigs.Insert(LB_ItemPreset.InitItem("_90%", true, 0.9, ""));
		defaultItemConfigs.Insert(LB_ItemPreset.InitItem("_80%", true, 0.8, ""));
		defaultItemConfigs.Insert(LB_ItemPreset.InitItem("_70%", true, 0.7, ""));
		defaultItemConfigs.Insert(LB_ItemPreset.InitItem("_60%", true, 0.6, ""));
		defaultItemConfigs.Insert(LB_ItemPreset.InitItem("_50%", true, 0.5, ""));
		defaultItemConfigs.Insert(LB_ItemPreset.InitItem("_40%", true, 0.4, ""));
		defaultItemConfigs.Insert(LB_ItemPreset.InitItem("_30%", true, 0.3, ""));
		defaultItemConfigs.Insert(LB_ItemPreset.InitItem("_20%", true, 0.2, ""));
		defaultItemConfigs.Insert(LB_ItemPreset.InitItem("_10%", true, 0.1, ""));
		defaultItemConfigs.Insert(LB_ItemPreset.InitItem("_MAG", true, 1.0, "", false, "mag"));
		defaultItemConfigs.Insert(LB_ItemPreset.InitItem("_ROOT", true, 1.0, "", false, "root"));
		defaultItemConfigs.Insert(LB_ItemPreset.InitItem("_BAT", false, 1.0, "").Add("Battery9V"));
		defaultItemConfigs.Insert(LB_ItemPreset.InitItem("_BAT_IND", true, 1.0, "").Add("Battery9V"));

		LB_Presets default_ = new LB_Presets();
		default_.name = "Default";

		default_.base.Insert(LB_PresetList.InitList("Preset_Weapons_Medium", true, 1.0).SetRoot(true).Add("Preset_M4A1").Add("Preset_SVD").Add("Preset_SKS"));
		default_.base.Insert(LB_PresetList.InitList("Preset_Military_Clothing", true, 1.0).SetRoot(true).Add("Preset_Military_Clothing_TTSKO").Add("Preset_Military_Clothing_Green").Add("Preset_Military_Clothing_Black"));
		default_.base.Insert(LB_PresetList.InitList("Preset_Military_Clothing_TTSKO", false, 1.0).Add("TTSKOPants").Add("TTsKOJacket_Camo").Add("TTSKOBoots").Add("AssaultBag_Ttsko").Add("Mich2001Helmet").Add("Preset_Military_Clothing_Vests"));
		default_.base.Insert(LB_PresetList.InitList("Preset_Military_Clothing_Green", false, 1.0).Add("CargoPants_Green").Add("M65Jacket_Khaki").Add("MilitaryBoots_Black").Add("AssaultBag_Green").Add("Mich2001Helmet").Add("Preset_Military_Clothing_Vests"));
		default_.base.Insert(LB_PresetList.InitList("Preset_Military_Clothing_Black", false, 1.0).Add("CargoPants_Black").Add("M65Jacket_Black").Add("MilitaryBoots_Black").Add("AssaultBag_Black").Add("Mich2001Helmet").Add("Preset_Military_Clothing_Vests"));
		default_.base.Insert(LB_PresetList.InitList("Preset_Military_Clothing_Vests", true, 0.7).Add("PoliceVest").Add("UKAssVest_Camo").Add("PlateCarrierVest"));
		default_.base.Insert(LB_PresetList.InitList("PlateCarrierAttachments", true, 1.0).Add("PlateCarrierHolster").Add("PlateCarrierPouches"));

		default_.base.Insert(LB_PresetList.InitList("Preset_Vanilla_Loadout", true, 1.0).SetRoot(true).Add("Preset_Vanilla_Clothes").Add("Preset_Vanilla_Items"));
		default_.base.Insert(LB_PresetList.InitList("Preset_Vanilla_Clothes", true, 1.0).Add("Preset_Vanilla_Clothes_White").Add("Preset_Vanilla_Clothes_Red").Add("Preset_Vanilla_Clothes_Green").Add("Preset_Vanilla_Clothes_Blue").Add("Preset_Vanilla_Clothes_Black"));
		default_.base.Insert(LB_PresetList.InitList("Preset_Vanilla_Items", true, 1.0).Add("Preset_Vanilla_Food").Add("Preset_Vanilla_Lights").Add("Preset_Vanilla_Rags"));

		default_.base.Insert(LB_PresetList.InitList("Preset_Vanilla_Clothes_White", false, 1.0).Add("TShirt_White").Add("SlacksPants_White").Add("AthleticShoes_Grey"));
		default_.items.Insert(LB_ItemPreset.InitItem("TShirt_White", true, 1.0));
		default_.items.Insert(LB_ItemPreset.InitItem("SlacksPants_White", true, 1.0));
		default_.items.Insert(LB_ItemPreset.InitItem("AthleticShoes_Grey", true, 1.0));

		default_.base.Insert(LB_PresetList.InitList("Preset_Vanilla_Clothes_Red", false, 1.0).Add("TShirt_Red").Add("ShortJeans_Red").Add("AthleticShoes_Grey"));
		default_.items.Insert(LB_ItemPreset.InitItem("TShirt_Red", true, 1.0));
		default_.items.Insert(LB_ItemPreset.InitItem("ShortJeans_Red", true, 1.0));

		default_.base.Insert(LB_PresetList.InitList("Preset_Vanilla_Clothes_Green", false, 1.0).Add("TShirt_Green").Add("Jeans_Green").Add("AthleticShoes_Green"));
		default_.items.Insert(LB_ItemPreset.InitItem("TShirt_Green", true, 1.0));
		default_.items.Insert(LB_ItemPreset.InitItem("Jeans_Green", true, 1.0));
		default_.items.Insert(LB_ItemPreset.InitItem("AthleticShoes_Green", true, 1.0));

		default_.base.Insert(LB_PresetList.InitList("Preset_Vanilla_Clothes_Blue", false, 1.0).Add("TShirt_Blue").Add("Jeans_Blue").Add("AthleticShoes_Blue"));
		default_.items.Insert(LB_ItemPreset.InitItem("TShirt_Blue", true, 1.0));
		default_.items.Insert(LB_ItemPreset.InitItem("Jeans_Blue", true, 1.0));
		default_.items.Insert(LB_ItemPreset.InitItem("AthleticShoes_Blue", true, 1.0));

		default_.base.Insert(LB_PresetList.InitList("Preset_Vanilla_Clothes_Black", false, 1.0).Add("TShirt_Black").Add("Jeans_Black").Add("AthleticShoes_Black"));
		default_.items.Insert(LB_ItemPreset.InitItem("TShirt_Black", true, 1.0));
		default_.items.Insert(LB_ItemPreset.InitItem("Jeans_Black", true, 1.0));
		default_.items.Insert(LB_ItemPreset.InitItem("AthleticShoes_Black", true, 1.0));

		default_.base.Insert(LB_PresetList.InitList("Preset_Vanilla_Rags", true, 1.0).Add("BandageDressing").Add("Rags"));
		default_.items.Insert(LB_ItemPreset.InitItem("BandageDressing", false, 1.0, "BandageDressing", true));
		default_.items.Insert(LB_ItemPreset.InitItem("Rag", false, 2.0, "Rags", true, "any", 4, 6));

		default_.base.Insert(LB_PresetList.InitList("Preset_Vanilla_Lights", true, 1.0).Add("Chemlight_White").Add("Chemlight_Yellow").Add("Chemlight_Green").Add("Chemlight_Red"));
		default_.items.Insert(LB_ItemPreset.InitItem("Chemlight_White", false, 1.0));
		default_.items.Insert(LB_ItemPreset.InitItem("Chemlight_Yellow", false, 1.0));
		default_.items.Insert(LB_ItemPreset.InitItem("Chemlight_Green", false, 1.0));
		default_.items.Insert(LB_ItemPreset.InitItem("Chemlight_Red", false, 1.0));

		default_.base.Insert(LB_PresetList.InitList("Preset_Vanilla_Food", true, 1.0).Add("Apple").Add("Pear").Add("Plum"));
		default_.items.Insert(LB_ItemPreset.InitItem("Apple", false, 1.0));
		default_.items.Insert(LB_ItemPreset.InitItem("Pear", false, 1.0));
		default_.items.Insert(LB_ItemPreset.InitItem("Plum", false, 1.0));

		default_.items.Insert(LB_ItemPreset.InitItem("Battery9V", false, 1.0));

		default_.items.Insert(LB_ItemPreset.InitItem("TTSKOPants", true, 1.0));
		default_.items.Insert(LB_ItemPreset.InitItem("TTsKOJacket_Camo", true, 1.0));
		default_.items.Insert(LB_ItemPreset.InitItem("TTSKOBoots", true, 1.0));
		default_.items.Insert(LB_ItemPreset.InitItem("AssaultBag_Ttsko", true, 0.7));

		default_.items.Insert(LB_ItemPreset.InitItem("CargoPants_Green", true, 1.0));
		default_.items.Insert(LB_ItemPreset.InitItem("M65Jacket_Khaki", true, 1.0));
		default_.items.Insert(LB_ItemPreset.InitItem("MilitaryBoots_Black", true, 1.0));
		default_.items.Insert(LB_ItemPreset.InitItem("AssaultBag_Green", true, 0.7));

		default_.items.Insert(LB_ItemPreset.InitItem("CargoPants_Black", true, 1.0));
		default_.items.Insert(LB_ItemPreset.InitItem("M65Jacket_Black", true, 1.0));
		default_.items.Insert(LB_ItemPreset.InitItem("AssaultBag_Black", true, 0.7));

		default_.items.Insert(LB_ItemPreset.InitItem("PoliceVest", false, 0.7));
		default_.items.Insert(LB_ItemPreset.InitItem("UKAssVest_Camo", false, 0.7));
		default_.items.Insert(LB_ItemPreset.InitItem("PlateCarrierVest", false, 0.7).Add("PlateCarrierAttachments"));

		default_.items.Insert(LB_ItemPreset.InitItem("PlateCarrierHolster", true, 0.5));
		default_.items.Insert(LB_ItemPreset.InitItem("PlateCarrierPouches", true, 0.5));

		default_.base.Insert(LB_PresetList.InitList("Preset_Scopes_M4_Short", true, 1.0).Add("ACOGOptic").Add("BUISOptic").Add("M68Optic").Add("ReflexOptic"));
		default_.items.Insert(LB_ItemPreset.InitItem("ACOGOptic", false, 0.5));
		default_.items.Insert(LB_ItemPreset.InitItem("BUISOptic", false, 0.5));
		default_.items.Insert(LB_ItemPreset.InitItem("M68Optic", false, 0.5).Add("Battery9V"));
		default_.items.Insert(LB_ItemPreset.InitItem("ReflexOptic", false, 0.5).Add("Battery9V"));

		default_.items.Insert(LB_ItemPreset.InitItem("M4A1", false, 1.0, "Preset_M4A1", true, "hands").Add("M4_Suppressor").Add("M4_MPBttstck").Add("M4_MPHndgrd").Add("Preset_Scopes_M4_Short").Add("Mag_STANAG_30Rnd_100").Add("Preset_STANAG_extra_Mags"));
		default_.items.Insert(LB_ItemPreset.InitItem("M4_Suppressor", true, 0.2));
		default_.items.Insert(LB_ItemPreset.InitItem("M4_MPBttstck", true, 1.0));
		default_.items.Insert(LB_ItemPreset.InitItem("M4_MPHndgrd", true, 1.0));
		default_.items.Insert(LB_ItemPreset.InitItem("Mag_STANAG_30Rnd", true, 1.0, "Mag_STANAG_30Rnd_100", true, "mag"));
		default_.base.Insert(LB_PresetList.InitList("Preset_STANAG_extra_Mags", true, 1.0).Add("Mag_STANAG_30Rnd_70").Add("Mag_STANAG_30Rnd_30"));
		default_.items.Insert(LB_ItemPreset.InitItem("Mag_STANAG_30Rnd", true, 0.7, "Mag_STANAG_30Rnd_70", true, "root"));
		default_.items.Insert(LB_ItemPreset.InitItem("Mag_STANAG_30Rnd", true, 0.3, "Mag_STANAG_30Rnd_30", true, "root"));

		default_.items.Insert(LB_ItemPreset.InitItem("SVD", false, 1.0, "Preset_SVD", true, "hands").Add("AK_Suppressor").Add("PSO1Optic").Add("Mag_SVD_10Rnd").Add("Preset_SVD_Extra"));
		default_.items.Insert(LB_ItemPreset.InitItem("PSO1Optic", true, 1.0).Add("Battery9V"));
		default_.items.Insert(LB_ItemPreset.InitItem("AK_Suppressor", true, 0.2));
		default_.items.Insert(LB_ItemPreset.InitItem("Mag_SVD_10Rnd", true, 1.0, "Mag_SVD_10Rnd", true, "mag"));
		default_.base.Insert(LB_PresetList.InitList("Preset_SVD_Extra", true, 1.0).Add("Ammo_762x54_100F").Add("Ammo_762x54_100R").Add("Ammo_762x54_30F").Add("Mag_SVD_10Rnd_F").Add("Mag_SVD_10Rnd_70F"));
		default_.items.Insert(LB_ItemPreset.InitItem("Ammo_762x54", true, 1.0, "Ammo_762x54_100F", false, "root"));
		default_.items.Insert(LB_ItemPreset.InitItem("Ammo_762x54", true, 1.0, "Ammo_762x54_100R", false, "root", 0.5, 1.0));
		default_.items.Insert(LB_ItemPreset.InitItem("Ammo_762x54", true, 0.3, "Ammo_762x54_30F", false, "root"));
		default_.items.Insert(LB_ItemPreset.InitItem("Mag_SVD_10Rnd", true, 1.0, "Mag_SVD_10Rnd_F", true, "root"));
		default_.items.Insert(LB_ItemPreset.InitItem("Mag_SVD_10Rnd", true, 0.7, "Mag_SVD_10Rnd_70F", true, "root"));

		default_.items.Insert(LB_ItemPreset.InitItem("Mich2001Helmet", true, 1.0));

		default_.items.Insert(LB_ItemPreset.InitItem("SKS", false, 1.0, "Preset_SKS", true, "hands").Add("Ammo_762x39_L").Add("Ammo_762x39_F"));
		default_.items.Insert(LB_ItemPreset.InitItem("Ammo_762x39", true, 1.0, "Ammo_762x39_L", false, "mag"));
		default_.items.Insert(LB_ItemPreset.InitItem("Ammo_762x39", true, 1.0, "Ammo_762x39_F", true, "root"));
	}

}