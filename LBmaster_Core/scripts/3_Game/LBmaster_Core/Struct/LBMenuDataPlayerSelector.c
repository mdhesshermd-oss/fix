class LBMenuDataPlayerSelector : LBMenuData {

	bool onlineOnly = true;
	ref ScriptCaller callback;

	void LBMenuDataPlayerSelector(ScriptCaller callback_ = null, bool onlineOnly_ = false) {
		this.callback = callback_;
		this.onlineOnly = onlineOnly_;
	}

	void OnSelect(PlayerNameSave_ player) {
		if (callback)
			callback.Invoke(player);
	}

}

