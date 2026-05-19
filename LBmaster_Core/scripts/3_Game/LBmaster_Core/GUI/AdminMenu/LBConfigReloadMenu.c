[DayZGame.RegisterLBAdminPage("LBConfigReloadMenu", "LBmaster_Core/logo/logo.paa", "#lb_admin_reload", "core.reload.configs")]
class LBConfigReloadMenu : LBAdmin_Menu_Page {

	ref map<string, ref TStringArray> loaders = new map<string, ref TStringArray>();

	TextListboxWidget list_configs;
	ButtonWidget btn_reload_selected, btn_reload_mod, btn_reload_all;
	EditBoxWidget edit_search;

	string currentMod, currentFile;

	override void OnShow() {
		super.OnShow();
		LBLogger.Info("Sending Update Check Request to server...", "Core");
		loaders = new map<string, ref TStringArray>();
		LoadConfigList();
		g_Game.RPCSingleParam(null, LBmaster_Core_RPCs.CONFIG_LIST, new Param1<bool>(true), true);
	}

	override array<string> GetEditedConfigs() {}

	override void OnRPC(Object target, int rpc_type, ParamsReadContext ctx) {
		if (rpc_type == LBmaster_Core_RPCs.CONFIG_LIST) {
			int count;
			if (!ctx.Read(count))
				return;
			for (int i = 0; i < count; ++i) {
				string modName, filename;
				if (!ctx.Read(modName) || !ctx.Read(filename))
					return;
				AddConfig(modName, filename);
			}
			LoadConfigList();
		}
	}

	void AddConfig(string modName, string filename) {
		TStringArray loader;
		if (!loaders.Find(modName, loader)) {
			loader = new TStringArray();
			loaders.Insert(modName, loader);
		}
		loader.Insert(filename);
	}

	void LoadConfigList() {
		currentMod = "";
		currentFile = "";
		list_configs.ClearItems();
		string search = edit_search.GetText();
		foreach (string modName, TStringArray files : loaders) {
			foreach (string file : files) {
				if (!LBStringTools.ContainsIgnoreCase(file, search) && !LBStringTools.ContainsIgnoreCase(modName, search))
					continue;
				list_configs.AddItem(modName + "/" + file, null, 0);
			}
		}
	}

	void LoadSelectedIndex() {
		int index = list_configs.GetSelectedRow();
		int i = 0;
		currentMod = "";
		currentFile = "";
		string search = edit_search.GetText();
		foreach (string modName, TStringArray files : loaders) {
			foreach (string file : files) {
				if (!LBStringTools.ContainsIgnoreCase(file, search) && !LBStringTools.ContainsIgnoreCase(modName, search))
					continue;
				if (i == index) {
					currentMod = modName;
					currentFile = file;
					return;
				}
				++i;
			}
		}
	}

	override bool OnChange(Widget w, int x, int y, bool finished) {
		if (super.OnChange(w, x, y, finished))
			return true;
		if (w == edit_search) {
			LoadConfigList();
		}
		return false;
	}

	override bool OnClick(Widget w, int x, int y, int button) {
		if (super.OnClick(w, x, y, button))
			return true;
		if (w == btn_reload_selected) {
			LoadSelectedIndex();
			LBWarningPopup.Get().Show("#lb_reload", LBTranslatedString("#lb_co_reload_config_confirm", {"%path%", currentMod + "/" + currentFile}).Get(), ScriptCaller.Create(ReloadCurrentConfig));
			return true;
		} else if (w == btn_reload_mod) {
			LoadSelectedIndex();
			LBWarningPopup.Get().Show("#lb_reload", LBTranslatedString("#lb_co_reload_mod_confirm", {"%currentMod%", currentMod}).Get(), ScriptCaller.Create(ReloadCurrentMod));
			return true;
		} else if (w == btn_reload_all) {
			LBWarningPopup.Get().Show("#lb_reload", "#lb_co_reload_all_confirm", ScriptCaller.Create(ReloadAll));
			return true;
		}
		return false;
	}

	void ReloadCurrentConfig() {
		int count = 1;
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(count);
		rpc.Write(currentMod);
		rpc.Write(currentFile);
		rpc.Send(null, LBmaster_Core_RPCs.CONFIG_RELOAD_FILE, true);
	}

	void ReloadCurrentMod() {
		TStringArray files = loaders.Get(currentMod);
		if (!files)
			return;
		int count = files.Count();
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(count);
		foreach (string file : files) {
			rpc.Write(currentMod);
			rpc.Write(file);
		}
		rpc.Send(null, LBmaster_Core_RPCs.CONFIG_RELOAD_FILE, true);
	}

	void ReloadAll() {
		int count = list_configs.GetNumItems();
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(count);
		string search = edit_search.GetText();
		foreach (string modName, TStringArray files : loaders) {
			foreach (string file : files) {
				if (!LBStringTools.ContainsIgnoreCase(file, search) && !LBStringTools.ContainsIgnoreCase(modName, search))
					continue;
				rpc.Write(modName);
				rpc.Write(file);
			}
		}
		rpc.Send(null, LBmaster_Core_RPCs.CONFIG_RELOAD_FILE, true);
	}

	override bool OnItemSelected(Widget w, int x, int y, int row, int column, int oldRow, int oldColumn) {
		if (super.OnItemSelected(w, x, y, row, column, oldRow, oldColumn))
			return true;
		if (w == list_configs) {
			LoadSelectedIndex();
			btn_reload_mod.SetText(LBTranslatedString("#lb_reload_selected_mod", {"%mod%", currentMod}).Get());
			return true;
		}
		return false;
	}

}