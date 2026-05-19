class LBItemSelectorDialog : LBMenuBase {

	ref array<ref TStringArray> categories = {null, {"Transport"}, {"CarWheel", "CarDoor"}, {"House"}, {"Clothing_Base"}, {"Container_Base"}, {"Weapon_Base", "Rifle_Base", "Fal_Base", "Pistol_Base"}, {"Magazine_Base"}, {"ItemOptics"}, {"SurvivorBase"}, {"DayZInfected"}, {"AnimalBase"}, {"Plant"}, {"Edible_Base"}, {"Trap_Base"}};
	ref TStringArray categoryNames = {"All", "Vehicles", "Car Parts", "Static Objects", "Clothing", "Containers", "Weapons", "Magazines + Ammo", "Optics", "Survivors", "Zombies", "Animals", "Plants", "Food", "Traps"};

	ButtonWidget btn_select, btn_close, btn_show_children, btn_find_attachments;
	TextWidget title;
	EditBoxWidget edit_search;
	TextListboxWidget list_items;
	CheckBoxWidget chckbx_spawnable, chckbx_enable_preview;
	XComboBoxWidget combo_category;
	ItemPreviewWidget item_preview;
	Object previewItem = null;

	ref TStringArray currentParent = new TStringArray();
	ref TStringArray currentItems = new TStringArray();

	ref LBMenuDataItemSelector data;

	static string lastValue;

	void InitCategories() {
		for (int i = 0; i < categoryNames.Count(); ++i) {
			combo_category.AddItem(categoryNames.Get(i));
		}
		combo_category.SetCurrentItem(0);
		OnCategoryChanged();
	}

	override void OnDataReceived() {
		data = LBMenuDataItemSelector.Cast(openParameter);
		if (!data) {
			data = new LBMenuDataItemSelector();
		}
		chckbx_spawnable.SetChecked(data.scopeFilter);
		title.SetText(data.title);
		btn_select.SetText(data.selectText);
		InitCategories();
		if (data.last_item != "")
			AddCustomCategory(data.last_item);
		else if (data.find_attachments_for)
			SetAttachmentsFor(data.find_attachments_for);
		edit_search.SetText(lastValue);
		SetFocus(edit_search);
		FillItemList();
		LBClearInput.AddClearButton(edit_search, ScriptCaller.Create(UpdateSearch));
	}

	override string GetLayoutName() {
		return "ItemSelector";
	}

	override void OnHide() {
		super.OnHide();
		ResetPreview();
	}

	override void Update(float timeslice) {
		super.Update(timeslice);
		if (g_Game.LBIsKeyDown(KeyCode.KC_LCONTROL) && g_Game.LBIsKeyPressed(KeyCode.KC_F)) {
			string itemname = GetSelectedItemname();
			if (itemname != "")
				SetAttachmentsFor(itemname);
		}
	}

	override ButtonWidget GetCloseButton() {
		return btn_close;
	}

	void ResetPreview() {
		if (previewItem)
			g_Game.ObjectDelete(previewItem);
	}

	void UpdatePreview() {
		ResetPreview();
		string itemname = GetSelectedItemname();
		if (!chckbx_enable_preview.IsChecked())
			return;
		previewItem = LBWidgetUtils.SpawnAndSetItemPreview(item_preview, itemname);
	}

	void OnCategoryChanged() {
		currentParent = categories.Get(combo_category.GetCurrentItem());
		FillItemList();
	}

	void FillItemList() {
		list_items.ClearItems();
		int scope = -1;
		if (chckbx_spawnable.IsChecked())
			scope = 2;
		currentItems.Clear();
		TStringSet found = LBInherit.Get().GetAllChildren(currentParent, false, true, true, scope);
		string search = edit_search.GetText();
		search.ToLower();
		foreach (string item : found) {
			if (IsSearched(item, search)) {
				list_items.AddItem(item, null, 0);
				currentItems.Insert(item);
			}
		}
		LBLogger.Verbose("ItemList of " + currentParent + " Scope: " + scope + " Items: " + currentItems.Count(), "Core");
		list_items.SelectRow(0);
	}

	TStringArray GetAttachments(string item) {
		TStringArray output = new TStringArray();
		TStringArray attNames = new TStringArray();
		LBInheritEntry entry = LBInherit.Get().GetEntry(item);
		if (!entry)
			return output;
		g_Game.ConfigGetTextArray(entry.category + " " + item + " attachments", attNames);
		foreach (string att : attNames) {
			output.InsertArray(LBInherit.Get().GetItemsIntoSlot(att));
		}
		g_Game.ConfigGetTextArray(entry.category + " " + item + " magazines", attNames);
		foreach (string att2 : attNames) {
			output.Insert(att2);
		}
		g_Game.ConfigGetTextArray(entry.category + " " + item + " chamberableFrom", attNames);
		foreach (string att3 : attNames) {
			output.Insert(att3);
		}
		return output;
	}

	static bool IsSearched(string itemname, string searchStr) {
		if (searchStr == "")
			return true;
		itemname.ToLower();
		return itemname.IndexOf(searchStr) != -1;
	}

	string GetSelectedItemname() {
		int row = list_items.GetSelectedRow();
		if (row < 0 || row >= currentItems.Count())
			return "";
		return currentItems.Get(row);
	}

	void OnSelected() {
		string selected = GetSelectedItemname();
		data.OnSelect(selected);
	}

	void UpdateSearch() {
		FillItemList();
		lastValue = edit_search.GetText();
	}

	override bool OnChange(Widget w, int x, int y, bool finished) {
		if (super.OnChange(w, x, y, finished))
			return true;
		if (w == edit_search) {
			UpdateSearch();
			return true;
		}
		return false;
	}

	void AddCustomCategory(string itemname) {
		categories.Insert({itemname});
		categoryNames.Insert(itemname);
		combo_category.AddItem(itemname);
		combo_category.SetCurrentItem(categories.Count() - 1);
		edit_search.SetText("");
		OnCategoryChanged();
	}

	override bool OnClick(Widget w, int x, int y, int button) {
		if (super.OnClick(w, x, y, button))
			return true;
		if (w == btn_select) {
			OnSelected();
			CloseMe();
			return true;
		} else if (w == btn_show_children) {
			string itemname = GetSelectedItemname();
			if (itemname == "")
				return false;
			AddCustomCategory(itemname);
			return true;
		} else if (w == chckbx_enable_preview) {
			UpdatePreview();
			return true;
		} else if (w == combo_category) {
			OnCategoryChanged();
			return true;
		} else if (w == chckbx_spawnable) {
			FillItemList();
			return true;
		} else if (w == btn_find_attachments) {
			itemname = GetSelectedItemname();
			if (itemname == "")
				return false;
			SetAttachmentsFor(itemname);
			return true;
		}
		return false;
	}

	void SetAttachmentsFor(string itemname) {
		TStringArray attachments = GetAttachments(itemname);
		LBLogger.Verbose("Attachments for " + itemname + ": " + attachments.Count(), "Core");
		if (attachments.Count() == 0)
			return;
		categories.Insert(attachments);
		categoryNames.Insert("Att: " + itemname);
		combo_category.AddItem("Att: " + itemname);
		combo_category.SetCurrentItem(categories.Count() - 1);
		edit_search.SetText("");
		OnCategoryChanged();
	}

	override bool OnItemSelected(Widget w, int x, int y, int row, int column,	int oldRow, int oldColumn) {
		if (super.OnItemSelected(w, x, y, row, column, oldRow, oldColumn))
			return true;
		if (w == list_items) {
			UpdatePreview();
		}
		return false;
	}

	override bool OnDoubleClick(Widget w, int x, int y, int button) {
		if (super.OnDoubleClick(w, x, y, button))
			return true;
		if (w == list_items) {
			OnSelected();
			CloseMe();
			return true;
		}
		return false;
	}

}