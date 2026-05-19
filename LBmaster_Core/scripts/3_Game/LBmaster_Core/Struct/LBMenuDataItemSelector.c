class LBMenuDataItemSelector : LBMenuData {

	string last_item, title, selectText, find_attachments_for;
	bool scopeFilter = true;
	ref ScriptCaller callback;

	void LBMenuDataItemSelector(ScriptCaller callback_ = null, bool enableScopeFilter = true, string last_item_ = "", string title_ = "Select an item", string selectText_ = "#lb_select", string attachments_for = "") {
		this.last_item = last_item_;
		this.title = title_;
		this.selectText = selectText_;
		this.callback = callback_;
		this.scopeFilter = enableScopeFilter;
		this.find_attachments_for = attachments_for;
	}

	void OnSelect(string itemname) {
		if (callback)
			callback.Invoke(itemname);
	}

}

