class LB_Item_PlaceMenu : LBAdmin_Menu_Page {

	XComboBoxWidget classnames;
	ItemPreviewWidget item_preview;
	Object previewedItem = null;
	ref TStringArray availableClassnames;
	ButtonWidget btn_place;
	TextWidget title;

	override string GetLayoutName() {
		return "AdminMenu_LB_Item_PlaceMenu";
	}

	override void InitWidgets() {
		super.InitWidgets();

		availableClassnames = InitAvailableClassnames();

		foreach (string itemname : availableClassnames) {
			classnames.AddItem(itemname);
		}
		if (availableClassnames.Count() > 0)
			classnames.SetCurrentItem(0);

		title.SetText(LBTranslatedString("#lb_co_choose_place", {"%item%", GetPlacementName()}).Get());
		btn_place.SetText(buttonName);
	}

	override Widget ClickOnSpacePressed() {
		return btn_place;
	}
	override XComboBoxWidget UseArrowKeysWidget() {
		return classnames;
	}

	string GetPlacementName() {

	}

	TStringArray InitAvailableClassnames() {

	}

	void OnPreviewChanged() {

	}

	int GetInitialRotation(string classname) {
		return 0;
	}

	void SetItemPreview() {
		string itemname;
		if (!GetCurrentClassname(itemname))
			return;
		Object newItem = LBWidgetUtils.SpawnAndSetItemPreview(item_preview, itemname);

		if (previewedItem != null)
			g_Game.ObjectDelete(previewedItem);
		previewedItem = newItem;

		OnPreviewChanged();
	}

	override void OnShow() {
		super.OnShow();
		SetItemPreview();
	}

	override void OnHide() {
		super.OnHide();
		if (previewedItem)
			g_Game.ObjectDelete(previewedItem);
	}

	bool GetCurrentClassname(out string itemName) {

		if (classnames.GetNumItems() <= 0)
			return false;
		int index = classnames.GetCurrentItem();
		if (index < 0 || index >= availableClassnames.Count())
			return false;
		itemName = availableClassnames.Get(index);
		return true;
	}

	override bool OnClick(Widget w, int x, int y, int button) {
		if (super.OnClick(w, x, y, button))
			return true;
		if (w == btn_place) {
			CloseMenu();
			string itemName;
			if (!GetCurrentClassname(itemName))
				return false;
			PlayerBase.StartAdminPlacementLB(itemName, GetPlacementName(), GetInitialRotation(itemName));
		}
		return false;
	}

	override bool OnChange(Widget w, int x, int y, bool finished) {
		if (super.OnChange(w, x, y, finished))
			return true;
		if (w == classnames) {
			SetItemPreview();
		}
		return false;
	}

}