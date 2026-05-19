class LBMenuDataFileSelector : LBMenuData {

	string title, selectText, initialFolder;
	ref ScriptCaller callback;
	ref TStringArray fileEndingsOnly;

	void LBMenuDataFileSelector(string initialFolder_ = "", string title_ = "#lb_file_selector", string selectText_ = "#lb_select", TStringArray endingsFilter_ = null, ScriptCaller callback_ = null) {
		this.title = title_;
		this.selectText = selectText_;
		this.callback = callback_;
		this.initialFolder = initialFolder_;
		this.fileEndingsOnly = endingsFilter_;
	}

	void OnSelect(string path) {
		if (callback)
			callback.Invoke(path);
	}

	bool IsInFilter(LBFileTree file) {
		return (file.IsFolder() && AnyChildInFilter(file) || !fileEndingsOnly || fileEndingsOnly.Find(file.ending) != -1);
	}

	bool AnyChildInFilter(LBFileTree folder) {
		bool any = false;
		foreach (LBFileTree child : folder.children) {
			if (IsInFilter(child)) {
				any = true;
				break;
			}
		}
		return any;
	}

}
