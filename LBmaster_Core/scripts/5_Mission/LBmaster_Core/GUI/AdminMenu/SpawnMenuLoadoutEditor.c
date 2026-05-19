[DayZGame.RegisterLBAdminPage("SpawnMenuLoadoutEditor", "LBmaster_Core/logo/logo.paa", "#lb_sp_admin_btn_loadout", "core.change.presets")]
class SpawnMenuLoadoutEditor : LBAdmin_Menu_Page {

	static string lastSelectedContainer = "";
	static bool loadLastSelectedContainer = false;

	ref LayoutHolderSpawnLayout dummyLayout = new LayoutHolderSpawnLayout(null);
	ref LeftArea leftArea;
	ref RightArea rightArea;
	ref HandsArea handsArea;
	PlayerBase dummyPlayer;
	EntityAI dummyObject;
	EntityAI spawnedPreviewItem;
	ref TStringArray spawnedPresets = new TStringArray();
	ref TStringSet allItems;

	ItemPreviewWidget item_preview, preview_parent;
	CheckBoxWidget chckbx_individual_chance, chckbx_root, chckbx_individual_chance_item, chckbx_hotbar, chckbx_clear_inv, chckbx_enable_item_preview, chckbx_duplicates;
	CheckBoxWidget chckbx_duplicates_item;
	ImageWidget img_info_stats;
	TextListboxWidget list_presets, list_preset_items, list_items, list_item_preset_items, list_test, list_popup_select, list_stats, list_errors;
	ButtonWidget btn_add_preset, btn_del_preset, btn_add_preset_item, btn_del_preset_item, btn_add_item, btn_del_item, btn_add_item_preset_item, btn_del_item_preset_item;
	ButtonWidget btn_test_reload, btn_test_clear, btn_container, btn_player, btn_preset, btn_item, btn_defaultitem, btn_select_popup, btn_close_popup, btn_stats_calc;
	ButtonWidget btn_close_stats, btn_add_category, btn_del_category, btn_close_errors;
	ScrollWidget HandsPanel;
	PlayerPreviewWidget player_preview;
	Widget DropzoneX;
	XComboBoxWidget combo_spawn_in, combo_categories, combo_find_category;
	EditBoxWidget edit_preset_name, edit_preset_chance, edit_item_name, edit_item_chance, edit_item_min, edit_item_max, edit_item_itemname, edit_search_popup;
	EditBoxWidget edit_preset_min_spawns, edit_preset_max_spawns, edit_preset_min_spawns_item, edit_preset_max_spawns_item, edit_search_presets, edit_search_items;
	EditBoxWidget edit_sub_preset_name, edit_sub_item_name, edit_category_name;
	Widget RightPanel, panel_player, panel_item, LeftPanel, panel_preset_select_parent, panel_preset_select, panel_statistics_parent, panel_statistics, panel_errors;

	bool needFullReload = true;
	bool addSelectedToPreset = true;
	int colorFound = -16711936;
	int colorNotFound = -55256;
	bool spawnPlayer = true;
	string containerItemname, popupItemname;
	int selectedPopupCategory = 0;

	LBWidget_Linked_List linked_Base, linked_Item;

	override void InitWidgets() {
		dummyLayout.LBSetWidgets(layoutRoot);
		leftArea = new LeftArea(dummyLayout);
		rightArea = new RightArea(dummyLayout);
		handsArea = new HandsArea(dummyLayout);
		RightPanel.Update();
		LeftPanel.Update();
		chckbx_enable_item_preview.SetChecked(true);
		allItems = LBInherit.Get().GetAllChildren(null, true, true, false, 2);
		RespawnParent();

		LBClearInput.AddClearButton(edit_search_popup, ScriptCaller.Create(LoadSelectPopup));
	}

	override void OnShow() {
		linked.LoadLinkedVars();
		ColorAllItems();
		ColorAllPresets();
		panel_errors.Show(false);
		if (loadLastSelectedContainer) {
			SetContainerItem(lastSelectedContainer);
		}
	}

	override void RegisterAllLinkedVars() {
		linked.RegisterLinkedList("GetConfig().presetLists", combo_categories, btn_add_category, btn_del_category).SetListManager(new LBListManager<LB_Presets>()).EnableDeleteConfirmation();
		linked.RegisterLinkedVar("GetConfig().presetLists[combo_categories].name", edit_category_name, true);

		linked.RegisterLinkedList("GetConfig().presetLists[combo_categories].base", list_presets, btn_add_preset, btn_del_preset).SetListManager(new LBListManagerSearch<LB_PresetList>()).SetSearchBar(edit_search_presets).SetLoadFunctions("", "", "ColorAllPresets");
		linked.RegisterLinkedVar("GetConfig().presetLists[combo_categories].base[list_presets].presetName", edit_preset_name, true);
		linked.RegisterLinkedVar("GetConfig().presetLists[combo_categories].base[list_presets].individualChance", chckbx_individual_chance);
		linked.RegisterLinkedVar("GetConfig().presetLists[combo_categories].base[list_presets].chance", edit_preset_chance);
		linked.RegisterLinkedVar("GetConfig().presetLists[combo_categories].base[list_presets].root", chckbx_root);
		linked.RegisterLinkedVar("GetConfig().presetLists[combo_categories].base[list_presets].allowDuplicates", chckbx_duplicates);
		linked.RegisterLinkedVar("GetConfig().presetLists[combo_categories].base[list_presets].minSpawnTries", edit_preset_min_spawns);
		linked.RegisterLinkedVar("GetConfig().presetLists[combo_categories].base[list_presets].maxSpawnTries", edit_preset_max_spawns);
		linked_Base = linked.RegisterLinkedList("GetConfig().presetLists[combo_categories].base[list_presets].items", list_preset_items, null, btn_del_preset_item).SetListManager(new LBListManagerSubPreset()).SetAllowedFromDrop(list_presets, list_items).SetColumnCount(2);
		linked_Base.SetChangeTrigger("OnSubPresetSelected()");
		linked.RegisterLinkedList("GetConfig().presetLists[combo_categories].items", list_items, btn_add_item, btn_del_item).SetListManager(new LBListManagerSearch<LB_ItemPreset>()).SetSearchBar(edit_search_items).EnableItemSelector(true, "", true).SetLoadFunctions("", "", "ColorAllItems");
		linked.RegisterLinkedVar("GetConfig().presetLists[combo_categories].items[list_items].presetName", edit_item_name, true);
		linked.RegisterLinkedVar("GetConfig().presetLists[combo_categories].items[list_items].individualChance", chckbx_individual_chance_item);
		linked.RegisterLinkedVar("GetConfig().presetLists[combo_categories].items[list_items].chance", edit_item_chance);
		linked.RegisterLinkedVar("GetConfig().presetLists[combo_categories].items[list_items].itemname", edit_item_itemname);
		linked.RegisterLinkedVar("GetConfig().presetLists[combo_categories].items[list_items].addToHotbar", chckbx_hotbar);
		linked.RegisterLinkedVar("GetConfig().presetLists[combo_categories].items[list_items].quantmin", edit_item_min);
		linked.RegisterLinkedVar("GetConfig().presetLists[combo_categories].items[list_items].quantmax", edit_item_max);
		linked.RegisterLinkedVar("GetConfig().presetLists[combo_categories].items[list_items].allowDuplicates", chckbx_duplicates_item);
		linked.RegisterLinkedVar("GetConfig().presetLists[combo_categories].items[list_items].minSpawnTries", edit_preset_min_spawns_item);
		linked.RegisterLinkedVar("GetConfig().presetLists[combo_categories].items[list_items].maxSpawnTries", edit_preset_max_spawns_item);
		array<ref Param2<string, string>> arr = new array<ref Param2<string, string>>();
		arr.Insert(new Param2<string, string>("Any", "any"));
		arr.Insert(new Param2<string, string>("Attachment", "att"));
		arr.Insert(new Param2<string, string>("Cargo", "cargo"));
		arr.Insert(new Param2<string, string>("Root", "root"));
		arr.Insert(new Param2<string, string>("Hands", "hands"));
		arr.Insert(new Param2<string, string>("Magazine", "mag"));
		linked.RegisterLinkedVar("GetConfig().presetLists[combo_categories].items[list_items].spawnIn", combo_spawn_in).SetComboValues(arr);
		linked_Item = linked.RegisterLinkedList("GetConfig().presetLists[combo_categories].items[list_items].items", list_item_preset_items, null, btn_del_item_preset_item).SetListManager(new LBListManagerSubPreset()).SetAllowedFromDrop(list_presets, list_items).SetColumnCount(2);
		linked_Item.SetChangeTrigger("OnSubItemSelected()");
		panel_preset_select_parent.Show(false);
		panel_statistics_parent.Show(false);
	}

	override string GetTooltip(Widget w) {
		if (w == img_info_stats)
			return "This statistics tool can give you some powerful insight into your item spawning. You can select Presets you want to check by double clicking them to add them to the test items list.\nWhat are the different columns:\n- Chance: The chance the item spawns at least once\n- Chance (2.7x): 2.7x means if the item spawns, it will spawn around 2.7 times. For example a weapon spawns extra mags (The weapon has to spawn to at least spawn the mag and then the weapon spawns extra mags)\n- x in 10000: calculates how often the item would spawn if the preset spawns 10.000 times. This is an average value, so 5000 would mean, that 50% of the times, there is one mag (this ignores the fact, that items can spawn in packs like explained with the weapon example before)\n- Quantity: If the item has quantity (ammo, planks, etc.), then the average quantity, each time the item would be spawned, is calculated. This would properly mix different chances and also repects the quantmin and quantmax settings. The first is an absolute number and the second is in percent";
		return "";
	}

	void ColorAllItems() {
		for (int i = 0; i < list_items.GetNumItems(); ++i) {
			UpdateItemColor(i);
		}
	}

	void UpdateItemColor(int index) {
		int index_category;
		if (index < 0 || index >= list_items.GetNumItems() || !LBWidgetUtils.CheckSelectedInRange(combo_categories, index_category))
			return;
		int list_index = linked.SearchedIndexToListIndex(list_items, index);
		LB_ItemPreset item = GetConfig().presetLists.Get(index_category).items.Get(list_index);
		string itemnameLower = item.itemname + "";
		itemnameLower.ToLower();
		if (allItems.Find(itemnameLower) == -1 || item.CanSpawnAllItems(allItems) != LBPresetTestResult.OK) {
			list_items.SetItemColor(index, 0, colorNotFound);
		} else {
			list_items.SetItemColor(index, 0, colorFound);
		}
	}

	void ColorAllPresets() {
		for (int i = 0; i < list_presets.GetNumItems(); ++i) {
			UpdatePresetColor(i);
		}
	}

	void UpdatePresetColor(int index) {
		int index_category;
		if (index < 0 || index >= list_presets.GetNumItems() || !LBWidgetUtils.CheckSelectedInRange(combo_categories, index_category))
			return;
		int list_index = linked.SearchedIndexToListIndex(list_presets, index);
		LB_PresetList preset = GetConfig().presetLists.Get(index_category).base.Get(list_index);
		if (preset.CanSpawnAllItems(allItems) == LBPresetTestResult.OK) {
			if (preset.root)
				list_presets.SetItemColor(index, 0, ARGB(255, 0, 255, 0));
			else
				list_presets.SetItemColor(index, 0, ARGB(255, 0, 255, 200));
		} else {
			if (preset.root)
				list_presets.SetItemColor(index, 0, ARGB(255, 255, 40, 40));
			else
				list_presets.SetItemColor(index, 0, ARGB(255, 255, 255, 40));
		}
	}

	LB_PresetLoader_ GetConfig() {
		return LB_PresetLoader.Get;
	}

	override array<string> GetEditedConfigs() {
		return {"LB_PresetLoader"};
	}

	void RespawnParent() {
		if (spawnPlayer) {
			if (dummyPlayer)
				g_Game.ObjectDelete(dummyPlayer);
			panel_player.Show(true);
			panel_item.Show(false);
			RightPanel.Show(true);
			LeftPanel.Show(false);
			int flags = ECE_SETUP | ECE_KEEPHEIGHT | ECE_PLACE_ON_SURFACE | ECE_INITAI | ECE_CREATEPHYSICS | ECE_LOCAL;
			dummyPlayer = PlayerBase.Cast(g_Game.CreateObjectEx("SurvivorM_Rolf", vector.Zero, flags));
			rightArea.m_PlayerContainer.SetPlayer(dummyPlayer);
			rightArea.RecomputeOpenedContainers();
			player_preview.SetPlayer(dummyPlayer);
			handsArea.LBGetHandsContainer().LBGetHandsPreview().SetCustomPlayer(dummyPlayer);
			SpawnStartGear();
			dummyPlayer.Update();
		} else {
			if (dummyObject)
				g_Game.ObjectDelete(dummyObject);
			panel_player.Show(false);
			panel_item.Show(true);
			RightPanel.Show(false);
			LeftPanel.Show(true);
			flags = ECE_SETUP | ECE_KEEPHEIGHT | ECE_PLACE_ON_SURFACE | ECE_LOCAL;
			dummyObject = EntityAI.Cast(g_Game.CreateObjectEx(containerItemname, vector.Zero, flags));
			MissionGameplay.dummyVicinityItem = dummyObject;
			preview_parent.SetItem(dummyObject);
			leftArea.UpdateInterval();
		}
	}

	void SpawnStartGear() {
		if (chckbx_clear_inv.IsChecked())
			return;
		LBSpawnLoadoutEntry root = LBLasterSpawnLoadoutsConfig.Get.rootEntry;
		root.SpawnChildren(dummyPlayer);
	}

	void ~SpawnMenuLoadoutEditor() {
		if (dummyPlayer)
			g_Game.ObjectDelete(dummyPlayer);
		if (dummyObject)
			g_Game.ObjectDelete(dummyObject);
		if (spawnedPreviewItem)
			g_Game.ObjectDelete(spawnedPreviewItem);
	}

	override void OnUpdateFrame() {
		if (dummyPlayer)
			player_preview.UpdateItemInHands(dummyPlayer.GetHumanInventory().GetEntityInHands());
	}

	override bool OnMouseButtonDown(Widget w, int x, int y, int button) {
		if (super.OnMouseButtonDown(w, x, y, button))
			return true;
		if (w == btn_close_popup || w == panel_preset_select_parent) {
			panel_preset_select_parent.Show(false);
			return true;
		} else if (w == btn_close_stats || w == panel_statistics_parent) {
			panel_statistics_parent.Show(false);
			return true;
		}
		return false;
	}

	override bool OnClick(Widget w, int x, int y, int button) {
		if (w == btn_test_clear) {
			spawnedPresets.Clear();
			list_test.ClearItems();
			ReloadSpawnedPresets(false);
			return true;
		} else if (w == btn_test_reload) {
			ReloadSpawnedPresets(false);
		} else if (w == chckbx_clear_inv) {
			ReloadSpawnedPresets(false);
		} else if (w == chckbx_enable_item_preview) {
			UpdatePreviewItem();
			return true;
		} else if (w == btn_container) {
			UIScriptedMenu current = g_Game.GetUIManager().GetMenu();
			LBMenuManager.Get().OpenMenu("LBItemSelectorDialog", new LBMenuDataItemSelector(ScriptCaller.Create(SetContainerItem), true, lastSelectedContainer), false, current);
			return true;
		} else if (w == btn_player) {
			spawnPlayer = true;
			loadLastSelectedContainer = false;
			ReloadSpawnedPresets(true);
			return true;
		} else if (w == btn_add_preset_item) {
			addSelectedToPreset = true;
			LoadSelectPopup();
			return true;
		} else if (w == btn_add_item_preset_item) {
			addSelectedToPreset = false;
			LoadSelectPopup();
			return true;
		} else if (w == btn_stats_calc) {
			ReloadSpawnedPresets(true);
			ShowStatistics();
			return true;
		} else if (w == btn_preset) {
			selectedPopupCategory = 0;
			LoadSelectPopup();
			return true;
		} else if (w == btn_item) {
			selectedPopupCategory = 1;
			LoadSelectPopup();
			return true;
		} else if (w == btn_defaultitem) {
			LBMenuManager.Get().OpenMenu("LBItemSelectorDialog", new LBMenuDataItemSelector(ScriptCaller.Create(OnSelectDefaultItem)), false, LBMenuManager.Get().GetOpenLBMenu());
			return true;
		} else if (w == btn_select_popup) {
			AddSelectedFromPopup();
			return true;
		} else if (w == btn_close_errors) {
			panel_errors.Show(false);
			return true;
		} else if (w == list_items && g_Game.LBIsKeyDown(KeyCode.KC_LSHIFT)) {
			int list_index, index_category;
			if (!LBWidgetUtils.CheckSelectedInRange(list_items, list_index, linked) || !LBWidgetUtils.CheckSelectedInRange(combo_categories, index_category))
				return false;
			LB_ItemPreset itemPreset = GetConfig().presetLists.Get(index_category).items.Get(list_index);
			linked_Base.AddEntryStr(itemPreset.presetName, false);
			return true;
		} else if (w == list_presets && g_Game.LBIsKeyDown(KeyCode.KC_LSHIFT)) {
			if (!LBWidgetUtils.CheckSelectedInRange(list_presets, list_index, linked) || !LBWidgetUtils.CheckSelectedInRange(combo_categories, index_category))
				return false;
			LB_PresetBase preset = GetConfig().presetLists.Get(index_category).base.Get(list_index);
			linked_Item.AddEntryStr(preset.presetName, false);
			return true;
		} else if (w == combo_find_category) {
			LoadSelectPopup();
			return true;
		}
		return super.OnClick(w, x, y, button);
	}

	override void SaveEditedConfigs() {
		LB_PresetLoader.Get.OnLoad();
		if (LB_PresetLoader_.errors.Count() > 0) {
			panel_errors.Show(true);
			list_errors.ClearItems();
			foreach (string error : LB_PresetLoader_.errors) {
				list_errors.AddItem(error, null, 0);
			}
		}
		super.SaveEditedConfigs();
	}

	void AddSelectedFromPopup() {
		int selected = list_popup_select.GetSelectedRow();
		if (selected < 0 || selected >= list_popup_select.GetNumItems())
			return;
		string text;
		list_popup_select.GetItemText(selected, 0, text);
		if (addSelectedToPreset) {
			linked_Base.AddEntryStr(text, false);
		} else {
			linked_Item.AddEntryStr(text, false);
		}
		panel_preset_select_parent.Show(false);
	}

	void OnSelectDefaultItem(string itemname) {
		popupItemname = itemname;
		selectedPopupCategory = 2;
		LoadSelectPopup();
	}

	void LoadSelectPopup() {
		SetFocus(edit_search_popup);
		if (!panel_preset_select_parent.IsVisible()) {
			combo_find_category.ClearAll();
			foreach (LB_Presets presets : GetConfig().presetLists)
				combo_find_category.AddItem(presets.name);
			combo_find_category.SetCurrentItem(combo_categories.GetCurrentItem());
		}
		panel_preset_select_parent.Show(true);
		TStringArray entries = GetPopupEntries();
		int oldSelected = list_popup_select.GetSelectedRow();
		int i = 0, a = 0;
		string searched = edit_search_popup.GetText();
		searched.ToLower();
		for (; i < entries.Count(); ++i) {
			string entry = entries.Get(i);
			if (searched == "" || LBStringTools.ContainsIgnoreCase(entry, searched)) {
				if (list_popup_select.GetNumItems() <= a)
					list_popup_select.AddItem(entry, null, 0);
				else
					list_popup_select.SetItem(a, entry, null, 0);
				++a;
			}
		}
		for (int j = list_popup_select.GetNumItems() - 1; j >= 0; --j) {
			if (j >= a) {
				list_popup_select.RemoveRow(j);
			}
		}
		if (list_popup_select.GetSelectedRow() >= a)
			list_popup_select.SelectRow(a - 1);
	}

	TStringArray GetPopupEntries() {
		TStringArray entries = new TStringArray();
		int selected = combo_find_category.GetCurrentItem();
		if (selectedPopupCategory == 0) {
			foreach (LB_PresetBase preset : GetConfig().presetLists.Get(selected).base) {
				entries.Insert(preset.presetName);
			}
		} else if (selectedPopupCategory == 1) {
			foreach (LB_ItemPreset item : GetConfig().presetLists.Get(selected).items) {
				entries.Insert(item.presetName);
			}
		} else if (selectedPopupCategory == 2) {
			foreach (LB_PresetBase def : GetConfig().defaultItemConfigs) {
				entries.Insert(popupItemname + def.presetName);
			}
		}
		return entries;
	}

	void ShowStatistics() {
		array<LB_PresetBase> presets = LB_PresetLoader.Get.FindPresets(spawnedPresets);
		array<ref Param3<string, float, float>> stats = CalculateStatistics(presets);
		SortStatistics(stats);
		list_stats.ClearItems();
		if (stats.Count() == 0) {
			list_stats.AddItem("No Presets in Test Presets list", null, 0);
		} else {
			foreach (Param3<string, float, float> stat : stats) {
				int tenK = stat.param2 * 10000.0;
				int row = list_stats.AddItem(stat.param1, null, 0);
				float chance = GetItemChance(presets, stat.param1);
				float percent = chance * 100.0;
				float max = GetMaxQuantity(stat.param1);
				float quantF = stat.param3 / stat.param2 * 100;
				float count = stat.param2 / chance;
				if (max == 0) {
					count *= stat.param3 / stat.param2;
					tenK *= stat.param3 / stat.param2;
				}
				list_stats.SetItem(row, "" + percent + "% (" + count + "x)", null, 1);
				list_stats.SetItem(row, " " + tenK, null, 2);
				if (max > 0)
					list_stats.SetItem(row, " " + ((int)(stat.param3 / stat.param2 * max)) + " (" + quantF + "%)", null, 3);
				else
					list_stats.SetItem(row, "", null, 3);
			}
		}
		panel_statistics_parent.Show(true);
	}

	float GetItemChance(array<LB_PresetBase> presets, string itemname) {
		float chance = 0.0;
		foreach (LB_PresetBase selected : presets) {
			chance = Math.Max(chance, GetItemChance(selected, 1.0, 0, itemname));
		}
		return chance;
	}

	float GetItemChance(LB_PresetBase preset, float mult, int depth, string itemname) {
		if (depth > 20) // Prevent stack overflow from missconfiguration
			return 0;
		float chance = 0.0;
		LB_ItemPreset item = LB_ItemPreset.Cast(preset);
		if (item && item.itemname == itemname) {
			chance = mult;
		}
		array<LB_PresetBase> global = new array<LB_PresetBase>();
		array<float> globalMultis = new array<float>();
		float globalChance = 0;
		array<LB_PresetBase> linkedList = preset.GetLinked();
		for (int i = 0; i < linkedList.Count(); ++i) {
			LB_PresetBase presetLinked = linkedList.Get(i);
			float presetChanceMulti = preset.GetLinkedMultis().Get(i);
			if (presetLinked.individualChance) {
				chance = Math.Max(chance, GetItemChance(presetLinked, mult * Math.Clamp(presetLinked.chance * presetChanceMulti, 0.0, 1.0), depth + 1, itemname));
			} else {
				global.Insert(presetLinked);
				globalMultis.Insert(presetChanceMulti);
				globalChance += presetLinked.chance * presetChanceMulti;
			}
		}
		float totalGlobal = 0.0;
		if (globalChance > 0) {
			for (i = 0; i < global.Count(); ++i) {
				LB_PresetBase nonIndividual = global.Get(i);
				float chanceGlobalMult = globalMultis.Get(i);
				totalGlobal += GetItemChance(nonIndividual, mult * nonIndividual.chance / globalChance * chanceGlobalMult, depth + 1, itemname);
			}
		}
		return Math.Max(chance, totalGlobal);
	}

	float GetMaxQuantity(string itemname) {
		if (g_Game.ConfigIsExisting("CfgMagazines " + itemname)) {
			return g_Game.ConfigGetInt("CfgMagazines " + itemname + " count");
		} else if (g_Game.ConfigIsExisting("CfgVehicles " + itemname)) {
			int quantMax = g_Game.ConfigGetInt("CfgVehicles " + itemname + " varQuantityMax");
			if (quantMax <= 0)
				quantMax = g_Game.ConfigGetInt("CfgVehicles " + itemname + " varStackMax");
			return quantMax;
		}
		return 0.0;
	}

	void SortStatistics(array<ref Param3<string, float, float>> arr) {
		bool unsorted = true;
		int count = arr.Count() - 1;
		while (unsorted) {
			unsorted = false;
			for (int i = 0; i < count; ++i) {
				if (arr[i].param2 < arr[i + 1].param2) {
					Param3<string, float, float> temp = arr[i];
					arr[i] = arr[i + 1];
					arr[i + 1] = temp;
					unsorted = true;
				}
			}
		}
	}

	array<ref Param3<string, float, float>> CalculateStatistics(array<LB_PresetBase> presets) {
		array<ref Param3<string, float, float>> output = new array<ref Param3<string, float, float>>();
		foreach (LB_PresetBase selected : presets) {
			float mult = 1.0;
			if (selected.minSpawnTries < selected.maxSpawnTries && selected.maxSpawnTries > 1) {
				float min = selected.minSpawnTries;
				float max = selected.maxSpawnTries;
				mult = (min + max) / 2.0;
			}
			CalculatePresetChances(selected, mult, output, 0);
		}
		return output;
	}

	void CalculatePresetChances(LB_PresetBase preset, float mult, array<ref Param3<string, float, float>> output, int depth) {
		if (depth > 20) // Prevent stack overflow from misconfiguration
			return;
		LB_ItemPreset item = LB_ItemPreset.Cast(preset);
		if (item) {
			AddToOutput(output, item.itemname, mult, item.quantmin, item.quantmax);
		}
		array<LB_PresetBase> global = new array<LB_PresetBase>();
		array<float> globalMultis = new array<float>();
		float globalChance = 0;
		array<LB_PresetBase> linkedList = preset.GetLinked();
		for (int i = 0; i < linkedList.Count(); ++i) {
			LB_PresetBase presetLinked = linkedList.Get(i);
			float presetChanceMult = preset.GetLinkedMultis().Get(i);
			if (presetLinked.individualChance) {
				CalculatePresetChances(presetLinked, mult * Math.Clamp(presetLinked.chance * presetChanceMult, 0.0, 1.0), output, depth + 1);
			} else {
				global.Insert(presetLinked);
				globalMultis.Insert(presetChanceMult);
				globalChance += presetLinked.chance * presetChanceMult;
			}
		}
		if (globalChance > 0) {
			for (i = 0; i < global.Count(); ++i) {
				LB_PresetBase nonIndividual = global.Get(i);
				float globalChanceMulti = globalMultis.Get(i);
				float chance = nonIndividual.chance / globalChance * globalChanceMulti;
				float mult2 = 1.0;
				if (nonIndividual.minSpawnTries < nonIndividual.maxSpawnTries && nonIndividual.maxSpawnTries > 1) {
					float min = nonIndividual.minSpawnTries;
					float max = nonIndividual.maxSpawnTries;
					mult2 = (min + max) / 2.0;
				}
				CalculatePresetChances(nonIndividual, mult * mult2 * chance, output, depth + 1);
			}
		}
	}

	void AddToOutput(array<ref Param3<string, float, float>> output, string itemname, float chance, float quantMin, float quantMax) {
		float quant = (quantMin + quantMax) / 2.0 * chance;
		if (quantMin < 0 || quantMax < 0)
			quant = chance;
		foreach (Param3<string, float, float> param : output) {
			if (param.param1 == itemname) {
				param.param2 += chance;
				param.param3 += quant;
				return;
			}
		}
		output.Insert(new Param3<string, float, float>(itemname, chance, quant));
	}

	void SetContainerItem(string itemname) {
		spawnPlayer = false;
		containerItemname = itemname;
		loadLastSelectedContainer = true;
		lastSelectedContainer = itemname;
		ReloadSpawnedPresets(false);
	}

	override bool OnChange(Widget w, int x, int y, bool finished) {
		bool ok = super.OnChange(w, x, y, finished);
		if (w == edit_item_itemname) {
			UpdatePreviewItem();
			ColorAllPresets();
			UpdateItemColor(list_items.GetSelectedRow());
		} else if (w == edit_search_popup) {
			LoadSelectPopup();
			return true;
		} else if (w == edit_sub_preset_name) {
			int selected, selectedPreset, selectedCategory;
			if (!LBWidgetUtils.CheckSelectedInRange(combo_categories, selectedCategory) || !LBWidgetUtils.CheckSelectedInRange(list_preset_items, selected, linked) || !LBWidgetUtils.CheckSelectedInRange(list_presets, selectedPreset, linked)) {
				return ok;
			}
			GetConfig().presetLists.Get(selectedCategory).base.Get(selectedPreset).items.Set(selected, edit_sub_preset_name.GetText());
			linked_Base.LoadLine();
			return true;
		} else if (w == edit_sub_item_name) {
			if (!LBWidgetUtils.CheckSelectedInRange(combo_categories, selectedCategory) || !LBWidgetUtils.CheckSelectedInRange(list_item_preset_items, selected, linked) || !LBWidgetUtils.CheckSelectedInRange(list_items, selectedPreset, linked))
				return ok;
			GetConfig().presetLists.Get(selectedCategory).items.Get(selectedPreset).items.Set(selected, edit_sub_item_name.GetText());
			linked_Item.LoadLine();
			return true;
		} else if (w == edit_preset_name) {
			ColorAllItems();
			ColorAllPresets();
			return true;
		}
		return ok;
	}

	void OnSubPresetSelected() {
		int selected, selectedPreset, selectedCategory;
		if (!LBWidgetUtils.CheckSelectedInRange(combo_categories, selectedCategory) || !LBWidgetUtils.CheckSelectedInRange(list_preset_items, selected, linked) || !LBWidgetUtils.CheckSelectedInRange(list_presets, selectedPreset, linked))
			return;
		edit_sub_preset_name.SetText(GetConfig().presetLists.Get(selectedCategory).base.Get(selectedPreset).items.Get(selected));
	}

	void OnSubItemSelected() {
		int selected, selectedPreset, selectedCategory;
		if (!LBWidgetUtils.CheckSelectedInRange(combo_categories, selectedCategory) || !LBWidgetUtils.CheckSelectedInRange(list_item_preset_items, selected, linked) || !LBWidgetUtils.CheckSelectedInRange(list_items, selectedPreset, linked))
			return;
		edit_sub_item_name.SetText(GetConfig().presetLists.Get(selectedCategory).items.Get(selectedPreset).items.Get(selected));
	}

	override bool OnItemSelected(Widget w, int x, int y, int row, int column, int oldRow, int oldColumn) {
		if (w == list_items) {
			UpdatePreviewItem();
			UpdateItemColor(row);
		} else if (w == list_presets) {
			UpdatePresetColor(row);
		}
		return super.OnItemSelected(w, x, y, row, column, oldRow, oldColumn);
	}

	void UpdatePreviewItem() {
		if (spawnedPreviewItem)
			g_Game.ObjectDelete(spawnedPreviewItem);
		if (!chckbx_enable_item_preview.IsChecked())
			return;
		int selected, selectedCategory;
		if (!LBWidgetUtils.CheckSelectedInRange(combo_categories, selectedCategory) || !LBWidgetUtils.CheckSelectedInRange(list_items, selected, linked))
			return;
		LB_ItemPreset item = GetConfig().presetLists.Get(selectedCategory).items.Get(selected);
		Object obj = g_Game.CreateObject(item.itemname, vector.Zero, true);
		if (!obj)
			return;
		spawnedPreviewItem = EntityAI.Cast(obj);
		if (!spawnedPreviewItem) {
			g_Game.ObjectDelete(obj);
			return;
		}
		item_preview.SetItem(spawnedPreviewItem);
	}

	void SelectItemOrPreset(string name) {
		int index = 0, selectedCategory;
		if (!LBWidgetUtils.CheckSelectedInRange(combo_categories, selectedCategory))
			return;
		foreach (LB_ItemPreset find1 : GetConfig().presetLists.Get(selectedCategory).items) {
			if (find1.presetName == name)
				break;
			++index;
		}
		int _index = linked.ListIndexToSearchedIndex(list_items, index);
		if (index >= GetConfig().presetLists.Get(selectedCategory).items.Count()) {
			index = 0;
			foreach (LB_PresetList find2 : GetConfig().presetLists.Get(selectedCategory).base) {
				if (find2.presetName == name)
					break;
				++index;
			}
			_index = linked.ListIndexToSearchedIndex(list_presets, index);
			if (_index != -1) {
				list_presets.SelectRow(_index);
				list_presets.EnsureVisible(_index);
				list_presets.Update();
			}
		} else if (_index != -1) {
			list_items.SelectRow(_index);
			list_items.EnsureVisible(_index);
			list_items.Update();
		}
	}

	override bool OnDoubleClick(Widget w, int x, int y, int button) {
		if (super.OnDoubleClick(w, x, y, button))
			return true;
		if (w == list_preset_items) {
			int selected = list_preset_items.GetSelectedRow();
			int selected_preset = 0, selectedCategory;
			if (!LBWidgetUtils.CheckSelectedInRange(combo_categories, selectedCategory) || selected < 0 || selected >= list_preset_items.GetNumItems() || !LBWidgetUtils.CheckSelectedInRange(list_presets, selected_preset, linked))
				return true;
			string name = GetConfig().presetLists.Get(selectedCategory).base.Get(selected_preset).items.Get(selected);
			int index = name.LastIndexOf("*");
			if (index != -1)
				name = name.Substring(0, index);
			SelectItemOrPreset(name);
			return true;
		} else if (w == list_item_preset_items) {
			selected = list_item_preset_items.GetSelectedRow();
			if (!LBWidgetUtils.CheckSelectedInRange(combo_categories, selectedCategory) || selected < 0 || selected >= list_item_preset_items.GetNumItems() || !LBWidgetUtils.CheckSelectedInRange(list_items, selected_preset, linked))
				return true;
			name = GetConfig().presetLists.Get(selectedCategory).items.Get(selected_preset).items.Get(selected);
			index = name.LastIndexOf("*");
			if (index != -1)
				name = name.Substring(0, index);
			SelectItemOrPreset(name);
			return true;
		} else if (w == list_presets) {
			if (!LBWidgetUtils.CheckSelectedInRange(combo_categories, selectedCategory) || !LBWidgetUtils.CheckSelectedInRange(list_presets, selected, linked))
				return true;
			name = GetConfig().presetLists.Get(selectedCategory).base.Get(selected).presetName;
			index = name.LastIndexOf("*");
			if (index != -1)
				name = name.Substring(0, index);
			spawnedPresets.Insert(name);
			list_test.AddItem(name, null, 0);
			ReloadSpawnedPresets(true);
		} else if (w == list_test) {
			selected = list_test.GetSelectedRow();
			if (selected < 0 || selected >= list_test.GetNumItems())
				return true;
			spawnedPresets.RemoveOrdered(selected);
			list_test.RemoveRow(selected);
			ReloadSpawnedPresets(true);
		} else if (w == list_popup_select) {
			AddSelectedFromPopup();
			return true;
		}
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y) {
		if (w == btn_test_reload || enterW == btn_test_reload) {
			needFullReload = true;
		}
		return super.OnMouseLeave(w, enterW, x, y);
	}

	void ReloadSpawnedPresets(bool reloadConfig) {
		RespawnParent();
		if (reloadConfig || needFullReload)
			LB_PresetLoader.Get.OnLoad();
		needFullReload = false;
		array<LB_PresetBase> presets = LB_PresetLoader.Get.FindPresets(spawnedPresets);
		if (spawnPlayer) {
			LB_PresetBase.SpawnPresets(dummyPlayer, presets, dummyPlayer);
			handsArea.LBGetHandsContainer().UpdateInterval();
			rightArea.UpdateInterval();
		} else {
			LB_PresetBase.SpawnPresets(null, presets, dummyObject);
			VicinityItemManager.GetInstance().GetVicinityItems().Clear();
			VicinityItemManager.GetInstance().GetVicinityItems().Insert(dummyObject);
			leftArea.UpdateInterval();
		}
	}

}