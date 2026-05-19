class LBMenuDataListSelector : LBMenuData {

	ref TStringArray list;
	string title;
	ref ScriptCaller callback;

	void LBMenuDataListSelector(ScriptCaller callback_, TStringArray arr_, string title_ = "#lb_title_list_selector") {
		this.callback = callback_;
		this.title = title_;
		this.list = arr_;
	}

	void OnSelect(string entry) {
		if (callback)
			callback.Invoke(entry);
	}

}

