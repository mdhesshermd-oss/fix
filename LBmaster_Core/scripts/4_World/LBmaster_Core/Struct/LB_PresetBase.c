class LB_PresetBase {

	string presetName = "Preset_NEW"; // Name of the preset it is referenced by. The name must be unique, otherwise the behaviour is undefined
	bool individualChance = false; // If set to 1, the chance for the preset to spawn is defined by `chance` (`0.0 = 0%`, `0.5 = 50%`, and `1.0 = 100%`). If set to 0, the chance is defined by summing up all chances of presets with also have `individualChance` set to 0 and in the same parent preset. So this basically allows to spawn one of the child presets with the option to have one spawn more often than the other. For example different scopes on weapons, where only one should be spawned, but some should be more common than others
	float chance = 1.0; // explained above.
	ref TStringArray items = new TStringArray(); // List of presets, which can spawn within this preset. This will be the parent preset referenced in the `individualChance` explaination. This contains the `presetName` of either a `basePreset` or an `itemsPreset`
	int minSpawnTries = 1; // How often this preset will be spawned at least (this way you can have min and max items spawning)
	int maxSpawnTries = 1; // How often this preset will be spawned at most
	bool allowDuplicates = true; // only applies to subpresets, which have individual chance disabled. With individual chance enabled, these are not affected by this setting. Example: You spawn 2 to 3 items and have the subpresets: common, uncommon, rare, epic, legendary. When this is enabled, you can end up spawning 3x common or 2x rare and 1x common or 3x legendary, but when disabled, you can always have 1 of each maximum, because they cannot be picked again. So 1x common, 1x rare and 1x legendary, but never more than once
	[NonSerialized()]
	private ref array<LB_PresetBase> linked = new array<LB_PresetBase>();
	[NonSerialized()]
	private ref array<float> linkedChanceMult = new array<float>();

	static ref array<Object> lastSpawnedObjects = new array<Object>();

	static void ResetLastSpawnedItems() {
		lastSpawnedObjects.Clear();
	}

	static void SetLastSpawnedItemsHealth(float min, float max) {
		if (min < 0 || max < 0 || min > max)
			return;
		foreach (Object obj : lastSpawnedObjects) {
			if (!obj)
				continue;
			float health = Math.RandomFloatInclusive(min, max);
			obj.SetHealth01("", "", health);
		}
	}

	void Invalidate() {
		linked = null;
		linkedChanceMult = null;
	}

	array<LB_PresetBase> GetLinked() {
		return linked;
	}

	TFloatArray GetLinkedMultis() {
		return linkedChanceMult;
	}

	void Initialize(LB_PresetLoader_ loader, TStringArray errors, string myName) {
		linked = new array<LB_PresetBase>();
		linkedChanceMult = new array<float>();
		foreach (string search : items) {
			int index = search.LastIndexOf("*");
			float multi = 1.0;
			if (index != -1) {
				multi = LBStringTools.End(search, index + 1).ToFloat();
				search = search.Substring(0, index);
			}
			LB_PresetBase foundPreset = loader.GetPreset(search);
			if (foundPreset == null) {
				LBLogger.Error("Could not find Preset " + search + " in Preset " + myName, "Core");
				errors.Insert("Could not find Preset " + search + " in Preset " + myName);
				continue;
			}
			linked.Insert(foundPreset);
			linkedChanceMult.Insert(multi);
			//LBLogger.Verbose("Linked " + search + " to " + foundPreset + " (" + foundPreset.presetName + ") in " + myName, "Core");
		}
	}

	bool IsSearched(string search, array<Widget> extraWidgets) {
		return LBStringTools.ContainsIgnoreCase(presetName, search);
	}

	bool ChechIndividualChance(float multi) {
		if (!individualChance)
			return false;
		return Math.RandomFloat01() <= (chance * multi);
	}

	bool CheckGlobalChance(out float current, float choosen, float multi) {
		if (individualChance)
			return false;
		current += chance * multi;
		if (choosen <= current)
			return true;
		return false;
	}

	void AddGlobalChance(out float global, float multi) {
		if (individualChance)
			return;
		global += chance * multi;
	}

	static void SpawnPresets(PlayerBase player, array<LB_PresetBase> presets, EntityAI parent, vector altPos = vector.Zero, float radius = 0.0) {
		SpawnPresets(player, presets, null, parent, altPos, radius);
	}

	static void SpawnPresets(PlayerBase player, array<LB_PresetBase> presets, TIntArray chances, EntityAI parent, vector altPos = vector.Zero, float radius = 0.0) {
		if (!presets || presets.Count() <= 0)
			return;
		int spawned = 0;
		float globalChance = 0.0;
		for (int i = 0; i < presets.Count(); ++i) {
			float multi = 1.0;
			if (chances && chances.Count() > i)
				multi = chances.Get(i);
			presets.Get(i).AddGlobalChance(globalChance, multi);

		}
		LB_PresetBase globalSpawned = null;
		if (globalChance > 0) {
			float choosen = Math.RandomFloat(0, globalChance);
			globalChance = 0;
			for (i = 0; i < presets.Count(); ++i) {
				LB_PresetBase preset2 = presets.Get(i);
				multi = 1.0;
				if (chances && chances.Count() > i)
					multi = chances.Get(i);
				if (preset2.CheckGlobalChance(globalChance, choosen, multi)) {
					LBLogger.Verbose("Global Chance reached at " + (globalChance * multi) + " with " + preset2.presetName, "Core");
					preset2.SpawnPreset(player, parent, altPos, radius);
					globalSpawned = preset2;
					break;
				}
			}
		}
		for (i = 0; i < presets.Count(); ++i) {
			LB_PresetBase preset3 = presets.Get(i);
			multi = 1.0;
			if (chances && chances.Count() > i)
				multi = chances.Get(i);
			if (preset3.ChechIndividualChance(multi)) {
				LBLogger.Verbose("Individual Chance reached at " + (globalChance * multi) + " with " + preset3.presetName, "Core");
				preset3.SpawnPreset(player, parent, altPos, radius);
			}
		}

	}

	void SpawnPreset(PlayerBase player, EntityAI parent, vector altPos = vector.Zero, float radius = 0.0) {
		int count = minSpawnTries;
		if (maxSpawnTries > count)
			count = Math.RandomIntInclusive(minSpawnTries, maxSpawnTries);
		EntityAI originalParent = parent;
		EntityAI newParent;
		LBLogger.Verbose("Spawning Loot Preset " + presetName + " " + count + "x in " + parent + " Player: " + player + " Has Items: " + linked.Count(), "Core");

		LB_ItemPreset itemPreset = LB_ItemPreset.Cast(this);
		set<int> alreadySpawnedIndices = new set<int>();
		for (int i = 0; i < count; ++i) {
			newParent = originalParent;
			if (itemPreset) {
				float quantity = itemPreset.GetRandomQuantity();
				if (quantity < 0) {
					newParent = itemPreset.Create(player, originalParent, altPos, radius, -1);
				} else {
					while (quantity >= 1) {
						newParent = itemPreset.Create(player, originalParent, altPos, radius, -2);
						quantity -= 1;
					}
					if (quantity > 0)
						newParent = itemPreset.Create(player, originalParent, altPos, radius, quantity);
				}
				if (newParent == null)
					return;
#ifdef SERVER
				LBLogger.Verbose("Spawned Item " + itemPreset.itemname + " on " + newParent + " at " + newParent.GetPosition(), "Core");
#endif
			}

			if (!linked || linked.Count() <= 0)
				continue;
			int spawned = 0;
			float globalChance = 0.0;
			for (int a = 0; a < linked.Count(); ++a) {
				if (allowDuplicates || alreadySpawnedIndices.Find(a) == -1) {
					LB_PresetBase preset1 = linked.Get(a);
					preset1.AddGlobalChance(globalChance, linkedChanceMult.Get(a));
				}
			}
			LB_PresetBase globalSpawned = null;
			if (globalChance > 0) {
				float choosen = Math.RandomFloat(0, globalChance);
				globalChance = 0;
				for (a = 0; a < linked.Count(); ++a) {
					LB_PresetBase preset2 = linked.Get(a);
					if (preset2.CheckGlobalChance(globalChance, choosen, linkedChanceMult.Get(a))) {
						if (allowDuplicates || alreadySpawnedIndices.Find(a) == -1) {
							LBLogger.Verbose("Global Chance reached at " + globalChance + " with " + preset2.presetName, "Core");
							preset2.SpawnPreset(player, newParent, altPos, radius);
							globalSpawned = preset2;
							alreadySpawnedIndices.Insert(a);
							break;
						}
					}
				}
			}
			for (a = 0; a < linked.Count(); ++a) {
				LB_PresetBase preset3 = linked.Get(a);
				if (preset3.ChechIndividualChance(linkedChanceMult.Get(a))) {
					LBLogger.Verbose("Individual Chance reached at " + globalChance + " with " + preset3.presetName, "Core");
					preset3.SpawnPreset(player, newParent, altPos, radius);
				}
			}
		}
	}

	LBPresetTestResult CanSpawnAllItems(TStringSet allItemsLower, int depth = 0) {
		if (depth > 20) {
			return LBPresetTestResult.INFINITE_LOOP;
		}
		if (!linked) {
			LBLogger.Error("Potential duplicate Preset Entry: " + presetName, "Core");
			LB_PresetLoader_.errors.Insert("Potential duplicate Preset Entry: " + presetName);
			return LBPresetTestResult.POSSIBLE_DUPLICATE;
		}
		if (linked.Count() != items.Count())
			return LBPresetTestResult.MISSING_ITEMS;
		foreach (LB_PresetBase list : linked) {
			LBPresetTestResult listResult = list.CanSpawnAllItems(allItemsLower, depth + 1);
			if (listResult != LBPresetTestResult.OK)
				return listResult;
		}
		return LBPresetTestResult.OK;
	}

}
