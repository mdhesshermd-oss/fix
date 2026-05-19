class LB_PlacedItemsOverviewMenu : LBAdmin_Menu_Page {

	XComboBoxWidget combo_type;
	TextWidget txt_position;
	CheckBoxWidget chckbx_preview;
	ItemPreviewWidget preview;
	MapWidget mapwidget;
	TextListboxWidget list_items;
	ButtonWidget btn_tp, btn_delete;

	ref TStringArray availableClassnames;
	Object previewedItem = null;
	bool skipSetMapPos = false;

	LBPlacementConfigBase GetConfig(); // Overwrite this method
	TStringArray InitAvailableClassnames() {
		return GetConfig().GetAvailableClassnames();
	}

	override void InitWidgets() {
		super.InitWidgets();

		availableClassnames = InitAvailableClassnames();
		combo_type.ClearAll();
		foreach (string classname : availableClassnames) {
			combo_type.AddItem(classname);
		}
	}

	override void OnShow() {
		super.OnShow();
		linked.LoadLinkedVars();
	}

	override void RegisterAllLinkedVars() {
		linked.RegisterLinkedList("GetConfig().objects", list_items, null, btn_delete).SetListManager(new LBListManager<ref LBPlacementEntry>()).PreventDuplicating().SetChangeTrigger("LoadPreview()");
		linked.RegisterLinkedVar("GetConfig().objects[list_items].position", txt_position);

		LoadPreview();
	}

	override string GetLayoutName() {
		return "AdminMenu_LB_PlacedItemsOverviewMenu";
	}

	LBPlacementEntry GetCurrentEntry() {
		int index = list_items.GetSelectedRow();
		if (index < 0 || index >= GetConfig().objects.Count())
			return null;
		return GetConfig().objects.Get(index);
	}

	int GetEntryNearIndex(vector pos) {
		int best = -1, index = -1;
		float distanceBest;
		foreach (LBPlacementEntry entry : GetConfig().objects) {
			++index;
			float dist = vector.DistanceSq(pos, entry.position);
			if (dist > 25000)
				continue;
			if (best == -1 || dist < distanceBest) {
				distanceBest = dist;
				best = index;
			}
		}
		return best;
	}

	void LoadPreview() {
		DeletePreview();
		LBPlacementEntry entry = GetCurrentEntry();
		if (!entry)
			return;
		if (chckbx_preview.IsChecked()) {
			previewedItem = LBWidgetUtils.SpawnAndSetItemPreview(preview, entry.itemname);
		}
		int index = availableClassnames.Find(entry.itemname);
		if (index != -1 && combo_type.GetCurrentItem() != index) {
			combo_type.SetCurrentItem(index);
		}
		UpdateMap();
	}

	void DeletePreview() {
		if (previewedItem) {
			g_Game.ObjectDelete(previewedItem);
		}
	}

	void UpdateMap() {
		mapwidget.ClearUserMarks();
		int index = -1;
		int selected = list_items.GetSelectedRow();
		foreach (LBPlacementEntry pos : GetConfig().objects) {
			++index;
			int color = ARGB(255, 255, 255, 255);
			if (index == selected)
				color = ARGB(255, 0, 255, 0);
			LBWidgetUtils.DrawSimpleMarker(mapwidget, pos.position, pos.itemname, color, "DZ\\gear\\navigation\\data\\map_tree_ca.paa");
		}
	}

	override bool OnItemSelected(Widget w, int x, int y, int row, int column, int oldRow, int oldColumn) {
		if (w == list_items) {
			SetMapPosition();
		}
		return super.OnItemSelected(w, x, y, row, column, oldRow, oldColumn);
	}

	void SetMapPosition() {
		if (skipSetMapPos) {
			skipSetMapPos = false;
			return;
		}
		LBPlacementEntry entry = GetCurrentEntry();
		if (!entry)
			return;
		mapwidget.SetScale(0.2);
		if (entry)
			mapwidget.SetMapPos(entry.position);
	}

	override bool OnClick(Widget w, int x, int y, int button) {
		if (super.OnClick(w, x, y, button))
			return true;
		if (w == combo_type) {
			LBPlacementEntry entry = GetCurrentEntry();
			if (entry)
				entry.itemname = availableClassnames.Get(combo_type.GetCurrentItem());
			LoadPreview();
			return true;
		} else if (w == btn_tp) {
			entry = GetCurrentEntry();
			if (entry)
				g_Game.RPCSingleParam(null, LBmaster_Core_RPCs.TELEPORT_TO_PLACED_ITEM, new Param1<vector>(entry.position), true);
			return true;
		} else if (w == chckbx_preview) {
			LoadPreview();
			return true;
		} else if (w == mapwidget) {
			vector near = mapwidget.ScreenToMap(Vector(x, y, 0));
			int nearEntryIndex = GetEntryNearIndex(near);
			if (nearEntryIndex != -1) {
				skipSetMapPos = true;
				list_items.SelectRow(nearEntryIndex);
				list_items.EnsureVisible(nearEntryIndex);
				list_items.Update();
				return true;
			}
		}
		return false;
	}

}