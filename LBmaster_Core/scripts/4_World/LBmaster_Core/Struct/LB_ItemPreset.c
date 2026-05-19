class LB_ItemPreset : LB_PresetBase {

	string itemname; // The item classname for the item, which should be spawned. E.g `TShirt_White`, `ShortJeans_Red`, `Apple`
	bool addToHotbar = false; // If the preset is spawned on a player, the item will be placed in the next free hotbar slot. So make sure the order of the presets has the items first, which should be in the first slots of the hotbar. Useful for weapons, magazines or bandages
	float quantmin = -1; // -1.0 to spawn with default quantity. Used for ammo stacks, magazines (also when attached to weapons), bottles, money etc. Otherwise it's 0.0 to 1.0 for 0% and 100% minimum quantity. So this is not an absolute number
	float quantmax = -1; // -1.0 to spawn with default quantity. Otherwise it's 0.0 to 1.0 for 0% and 100% maximum quantity. So this is not an absolute number. You can exceed this and the mod will spawn multiple stacks. you could set it to 5.0 to spawn up to 5 stacks
	string spawnIn = "any"; /* Very powerful option, which defines the location the item is spawned. The following options are available (make sure to use the exact lowercase name !):
	 - `any`: Tries to spawn the item in any location (attachment or cargo where attachment is tried first and then cargo) of the parent item. Useful to have items still spawning in cargo when no attachment slot is free
	 - `att`: Tries to spawn the item as attachment of the parent Item. If it failes, the item is not spawned at all
	 - `cargo`: Tries to spawn the item in the cargo space of the parent item. If it failed, the item is not spawned at all
	 - `root`: Tries to spawn the item like any, but the parent item is the root item the preset is spawned at. For example the player. This is especially useful when wanting to spawn extra mags with weapons in the player inventory. any would try to spawn the item in the cargo of the weapon and it will fail. root will spawn the item in the player inventory instead.
	 - `hands`: Tries to spawn the item in the Hands of the Player when the root entity is a player. If it's not a player or the player already has an item in hands, the item will be spawned like described in any
	 - `mag`: Tries to spawn the item in the magazine slot of the parent weapon. If the parent is not a weapon or already has a magazine, it will fail. Otherwise the weapon will be properly loaded with the mag and can be fired by the player right away. useful to be combined with the parent weapon spawning in hands and magazines in the weapon to give the player a fire ready weapon in hands. If the weapon does not have a magazine, you can also use the ammo stacks to load the internal magazine (addToHotbar and quantity does not apply to the internal magazine spawning)
	*/

	static LB_ItemPreset InitItem(string itemname_, bool individual, float chance_, string setName = "", bool addToHotbar_ = false, string spawnIn_ = "any", float quantmin_ = -1, float quantmax_ = -1) {
		LB_ItemPreset preset = new LB_ItemPreset();
		preset.presetName = setName;
		if (preset.presetName == "")
			preset.presetName = itemname_;
		preset.itemname = itemname_;
		preset.individualChance = individual;
		preset.chance = chance_;
		preset.spawnIn = spawnIn_;
		preset.quantmin = quantmin_;
		preset.quantmax = quantmax_;
		preset.addToHotbar = addToHotbar_;

		return preset;
	}

	LB_ItemPreset CopyToNew(string itemname_, string presetName_) {
		LB_ItemPreset n = new LB_ItemPreset();
		n.itemname = itemname_;
		n.addToHotbar = addToHotbar;
		n.quantmin = quantmin;
		n.quantmax = quantmax;
		n.spawnIn = spawnIn;
		n.presetName = presetName_;
		n.individualChance = individualChance;
		n.chance = chance;
		n.items = items;
		return n;
	}

	override LBPresetTestResult CanSpawnAllItems(TStringSet allItemsLower, int depth = 0) {
		if (depth > 20) {
			return LBPresetTestResult.INFINITE_LOOP;
		}
		string lowerItem = LBStringTools.ToLowerString(itemname);
		if (allItemsLower.Find(lowerItem) == -1) {
			LBLogger.Error("Item " + itemname + " could not be found on your server! Check the " + presetName + " preset", "Core");
			LB_PresetLoader_.errors.Insert("Item " + itemname + " could not be found on your server! Check the " + presetName + " preset");
			return LBPresetTestResult.MISSING_ITEMS;
		}
		return super.CanSpawnAllItems(allItemsLower, depth + 1);
	}

	void Setup(string name) {
		presetName = name;
		itemname = name;
		quantmin = -1;
		quantmax = -1;
		individualChance = true;
		chance = 1.0;
	}

	string ToListString(int column) {
		return presetName;
	}

	LB_ItemPreset Add(string item) {
		items.Insert(item);
		return this;
	}

	EntityAI Create(PlayerBase player, EntityAI parent, vector altPos, float radius, float quantity) {
		EntityAI item = null;
		if (itemname == "")
			return item;
		if (!parent && altPos != vector.Zero) {
			item = CreateGround(altPos, radius, quantity);
		} else if (spawnIn == "att") {
			item = CreateAttachment(player, parent, quantity);
		} else if (spawnIn == "mag") {
			if (parent && LBInherit.Get().IsWeaponBroken(parent.GetType())) {
				LBLogger.Info("Spawning Mag " + itemname + " for broken Weapon " + parent.GetType() + " at root instead of mag to prevent crashing the game", "Core");
				parent = parent.GetHierarchyRoot();
				item = CreateInventory(player, parent, quantity);
			} else {
				item = CreateWeaponMag(player, parent, quantity);
			}
		} else if (spawnIn == "hands") {
			item = CreateInHands(player, parent, quantity);
		} else if (spawnIn == "cargo") {
			item = CreateInCargo(player, parent, quantity);
		} else if (spawnIn == "root") {
			parent = parent.GetHierarchyRoot();
			item = CreateInventory(player, parent, quantity);
		} else {
			item = CreateInventory(player, parent, quantity);
		}

#ifdef SERVER
		if (!item && altPos != vector.Zero)
			item = CreateGround(altPos, radius, quantity);
		lastSpawnedObjects.Insert(item);
#endif
		return item;
	}

	EntityAI CreateInCargo(PlayerBase player, EntityAI parent, float quantity) {
		EntityAI item = parent.GetInventory().CreateEntityInCargo(itemname);
		if (!item) {
			Object itemObj = g_Game.CreateObjectEx(itemname, vector.Zero, ECE_LOCAL);
			item = EntityAI.Cast(itemObj);
			if (itemObj && !item)
				g_Game.ObjectDelete(item);
			if (item) {
				InventoryLocation invLoc = new InventoryLocation();
				if (parent.GetInventory().FindFreeLocationFor(item, FindInventoryLocationType.ANY_CARGO, invLoc) && invLoc.IsValid()) {
#ifdef SERVER
					item = parent.GetInventory().LocationCreateEntity(invLoc, itemname, ECE_NONE, RF_NONE);
#else
					item = parent.GetInventory().LocationCreateLocalEntity(invLoc, itemname, ECE_LOCAL, RF_NONE);
#endif
				}

			}
			if (itemObj)
				g_Game.ObjectDelete(itemObj);
			if (!item) {
				LBLogger.Debug("Failed to spawn item preset in Cargo with item: " + itemname + " on " + parent, "Core");
				return null;
			}
		}
		return SetupSpawnedItem(player, item, quantity);
	}

	EntityAI CreateInHands(PlayerBase player, EntityAI parent, float quantity) {
		EntityAI item = null;
		if (player) {
			HumanInventory inv = player.GetHumanInventory();
			item = inv.CreateInHands(itemname);
		}
		if (!item) {
			LBLogger.Debug("Failed to spawn item preset in Hands with item: " + itemname + " on " + player + " Trying to spawn Item in Inventory Instead", "Core");
			return CreateInventory(player, parent, quantity);
		}
		return SetupSpawnedItem(player, item, quantity);
	}

	EntityAI CreateGround(vector pos, float radius, float quantity) {
		vector spawnPos = pos;
		if (radius > 0.0) {
			float randAngle = Math.RandomFloat(0, Math.PI2);
			float x = pos[0] + Math.Sin(randAngle) * radius;
			float z = pos[2] + Math.Cos(randAngle) * radius;
			spawnPos = Vector(x, pos[1] + 0.1, z);
		}
		EntityAI obj = EntityAI.Cast(g_Game.CreateObjectEx(itemname, spawnPos, ECE_PLACE_ON_SURFACE));
		if (!obj) {
			LBLogger.Debug("Failed to spawn item preset on ground: " + itemname, "Core");
			return null;
		}
		obj.SetPosition(spawnPos);
		return SetupSpawnedItem(null, obj, quantity);
	}

	EntityAI CreateAttachment(PlayerBase player, EntityAI parent, float quantity) {
		EntityAI item = parent.GetInventory().CreateAttachment(itemname);
		if (!item) {
			LBLogger.Debug("Failed to spawn item preset as Attachment with item: " + itemname + " on " + parent, "Core");
			return null;
		}
		return SetupSpawnedItem(player, item, quantity);
	}

	EntityAI CreateInventory(PlayerBase player, EntityAI parent, float quantity) {
		EntityAI item = parent.GetInventory().CreateInInventory(itemname);
		if (!item) {
			Object itemObj = g_Game.CreateObjectEx(itemname, vector.Zero, ECE_LOCAL);
			item = EntityAI.Cast(itemObj);
			if (itemObj && !item)
				g_Game.ObjectDelete(item);
			if (item) {
				InventoryLocation invLoc = new InventoryLocation();
				if (parent.GetInventory().FindFreeLocationFor(item, FindInventoryLocationType.ANY, invLoc) && invLoc.IsValid()) {
#ifdef SERVER
					item = parent.GetInventory().LocationCreateEntity(invLoc, itemname, ECE_NONE, RF_NONE);
#else
					item = parent.GetInventory().LocationCreateLocalEntity(invLoc, itemname, ECE_LOCAL, RF_NONE);
#endif
				} else {
					item = parent.GetInventory().CreateAttachment(itemname);
					if (!item)
						item = parent.GetInventory().CreateEntityInCargo(itemname);
				}

			}
			if (itemObj)
				g_Game.ObjectDelete(itemObj);
			if (!item) {
				LBLogger.Debug("Failed to spawn item preset in Inventory with item: " + itemname + " on " + parent + " Item: " + item, "Core");
				return null;
			}
		}
		return SetupSpawnedItem(player, item, quantity);
	}

	EntityAI CreateWeaponMag(PlayerBase player, EntityAI parent, float quantity) {
		int state = -1;
		Weapon_Base wpn = Weapon_Base.Cast(parent);
		if (!wpn) {
			LBLogger.Debug("Could not spawn Magazine on Non Weapon item: " + wpn + ": " + itemname, "Core");
			return null;
		}

		if (wpn.GetMagazineTypeCount(0) > 0) {
			Magazine mag = SpawnAttachedMagazine(wpn, itemname);
			if (!mag) {
				LBLogger.Debug("Could not spawn Magazine on " + wpn + ": " + itemname, "Core");
				return null;
			}
			LBLogger.Verbose("Weapon Mag attached to weapon " + wpn, "Core");
			return SetupSpawnedItem(player, mag, quantity);
		} else if (wpn.HasInternalMagazine(-1)) {
			LBLogger.Verbose("Weapon has internal Mag. Trying to spawn " + itemname + " in internal mag of " + wpn, "Core");
			wpn.FillInnerMagazine(itemname);
			return null;
		} else if (wpn.FillChamber(itemname)) {
			LBLogger.Verbose("Filled weapon Chamber of " + wpn + " with " + itemname, "Core");
			return null;
		} else {
			LBLogger.Verbose("Weapon does not have magazine slot or internal mag: " + wpn, "Core");
		}
		LBLogger.Debug("Could not spawn Magazine on " + wpn + ": " + itemname + ". Weapon does not have Magazine Slots", "Core");
		return null;
	}

	Magazine SpawnAttachedMagazine(Weapon_Base wpn, string magazineClassname) {
		if (wpn.GetMuzzleCount() == 0 || wpn.GetMagazineTypeCount(0) == 0) {
			return null;
		}

		Magazine mag = Magazine.Cast(wpn.GetInventory().CreateAttachment(magazineClassname));
		if (!mag) {
			return null;
		}

		if (wpn.MustBeChambered(0)) {
			string bulletType;
			float dmg;
			if (mag.ServerAcquireCartridge(dmg, bulletType)) {
				wpn.FillChamber(bulletType);
			}
		}
		if (!wpn.FillChamber(magazineClassname)) {
			// If successful we already called these two methods
			wpn.RandomizeFSMState();
			wpn.Synchronize();
		}

		return mag;
	}

	float GetRandomQuantity() {
		if (quantmin < 0 || quantmax < 0)
			return -1;
		if (quantmin >= quantmax)
			return quantmax;
		return Math.RandomFloatInclusive(quantmin, quantmax);
	}

	private EntityAI SetupSpawnedItem(PlayerBase player, EntityAI item, float quantity) {
		if (!item)
			return null;

		LB_Key_SpawnHelper.AddToQuickbar(player, item, addToHotbar);
		LB_Key_SpawnHelper.SetRandomQuantity(ItemBase.Cast(item), quantity);
		return item;
	}

}