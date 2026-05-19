class LBListSelectorDialog : LBMenuBase {

	TextWidget title;
	Widget panel_image_preview;
	ImageWidget img_preview;
	EditBoxWidget edit_search;
	TextListboxWidget list_content;
	ButtonWidget btn_select, btn_close;

	LBMenuDataListSelector data;

	override string GetLayoutName() {
		return "ListSelector";
	}

	override void OnDataReceived() {
		data = LBMenuDataListSelector.Cast(openParameter);
		if (!data)
			return;
		title.SetText(data.title);
		LoadList();
		LBClearInput.AddClearButton(edit_search, ScriptCaller.Create(LoadList));
	}

	void LoadList() {
		int row = list_content.GetSelectedRow();
		list_content.ClearItems();
		string search = edit_search.GetText();

		foreach (string entry : data.list) {
			if (search == "" || LBStringTools.ContainsIgnoreCase(entry, search))
				list_content.AddItem(entry, null, 0);
		}

		if (row < 0 || row >= list_content.GetNumItems())
			row = list_content.GetNumItems() - 1;
		list_content.SelectRow(row);
		list_content.Update();
		SetFocus(edit_search);
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(list_content.EnsureVisible, 100, false, row);
	}

	override bool OnItemSelected(Widget w, int x, int y, int row, int column, int oldRow, int oldColumn) {
		if (super.OnItemSelected(w, x, y, row, column, oldRow, oldColumn))
			return true;
		if (w == list_content && row >= 0 && row < list_content.GetNumItems()) {
			string text;
			list_content.GetItemText(row, 0, text);
			if (IsTextImage(text)) {
				panel_image_preview.Show(true);
				img_preview.LoadImageFile(0, text);
			} else {
				panel_image_preview.Show(false);
			}
		}
		return false;
	}

	bool IsTextImage(string path) {
		if (path.IndexOf("set:") == 0 && path.IndexOf("image:") != -1)
			return true;
		if (LBStringTools.EndsWith(path, ".paa") || LBStringTools.EndsWith(path, ".edds"))
			return true;
		return false;
	}

	override bool OnChange(Widget w, int x, int y, bool finished) {
		if (super.OnChange(w, x, y, finished))
			return true;
		if (w == edit_search) {
			LoadList();
			return true;
		}
		return false;
	}

	override bool OnDoubleClick(Widget w, int x, int y, int button) {
		if (super.OnDoubleClick(w, x, y, button))
			return true;
		if (w == list_content) {
			OnSelectEntry();
			return true;
		}
		return false;
	}

	void OnSelectEntry() {
		int row = list_content.GetSelectedRow();
		if (row < 0 || row >= list_content.GetNumItems())
			return;
		string value;
		list_content.GetItemText(row, 0, value);
		data.OnSelect(value);
		CloseMe();
	}

	override bool OnClick(Widget w, int x, int y, int button) {
		if (super.OnClick(w, x, y, button))
			return true;
		if (w == btn_close) {
			CloseMe();
			return true;
		} else if (w == btn_select) {
			OnSelectEntry();
			return true;
		}
		return false;
	}

}