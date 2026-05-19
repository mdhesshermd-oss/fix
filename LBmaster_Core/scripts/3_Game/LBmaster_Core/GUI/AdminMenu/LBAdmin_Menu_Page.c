class LBAdmin_Menu_Page : ScriptedWidgetEventHandler {

	ButtonWidget buttonWidget = null;
	Widget layoutRoot, parentRoot;
	ref LBLinkedVarHandler linked;
	string buttonName, showPermission, oldConfigContent, pboPrefix;
	LBAdmin_Menu_Page warningTargetPage;
	bool wasShown = false;
	LBAdmin_Menu_Category parentCategory;
	float timeslice;

	void ~LBAdmin_Menu_Page() {
		if (g_Game)
			LoadEditedConfigs();
	}

	void Init(Widget parent_, string buttonName_, string showPermission_, LBAdmin_Menu_Category parent) {
		this.parentRoot = parent_;
		this.parentCategory = parent;
		this.buttonName = buttonName_;
		this.showPermission = showPermission_;
		linked = new LBLinkedVarHandler(this);
	}

	void RegisterAllLinkedVars();

	void CloseMenu() {
		g_Game.GetUIManager().CloseAll();
	}
	void OnBeforeUpdate() {
		if (g_Game.LBIsKeyPressed(KeyCode.KC_SPACE) && ClickOnSpacePressed()) {
			Widget focus = GetFocus();
			if (!EditBoxWidget.Cast(focus) && !MultilineEditBoxWidget.Cast(focus))
				OnClick(ClickOnSpacePressed(), 0, 0, MouseState.LEFT);
		} else if ((g_Game.LBIsKeyPressed(KeyCode.KC_LEFT) || g_Game.LBIsKeyPressed(KeyCode.KC_RIGHT)) && UseArrowKeysWidget()) {
			focus = GetFocus();
			if (!EditBoxWidget.Cast(focus) && !MultilineEditBoxWidget.Cast(focus)) {
				XComboBoxWidget combo = UseArrowKeysWidget();
				if (combo == focus)
					return;
				int num = combo.GetNumItems();
				int current = combo.GetCurrentItem();
				if (g_Game.LBIsKeyPressed(KeyCode.KC_LEFT))
					combo.SetCurrentItem((current - 1 + num) % num);
				else
					combo.SetCurrentItem((current + 1) % num);
				OnChange(combo, 0, 0, true);
			}
		}
	}
	void InitLayoutRoot() {
		if (!layoutRoot) {
			string layoutName = GetLayoutName();
			layoutRoot = LBLayoutManager.Get().CreateLayout(layoutName, parentRoot);
			if (layoutRoot)
				ConnectClassWidgetVariables(this, layoutRoot, {"buttonWidget", "parentRoot"}, null, LBLayoutManager.Get().GetLayoutPath(layoutName));
			else
				LBLogger.Fatal("Could not create layout " + layoutName + " for Admin Menu " + this, "Core");
			InitWidgets();
			RegisterAllLinkedVars();
			EditBoxWidget searchBar = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("edit_search"));
			if (searchBar) {
				LBClearInput.AddClearButton(searchBar, ScriptCaller.Create(ClearSearch));
			}
		}
	}

	void ClearSearch(EditBoxWidget searchBar) {
		OnChange(searchBar, 0, 0, false);
	}

	// Optional Overwrites
	void InitWidgets();
	void OnUpdateFrame();
	bool CanShowButton() {
		return true;
	}
	bool HasSaveButton() {
		array<string> edited = GetEditedConfigs();
		return edited && edited.Count() > 0;
	}
	Widget ClickOnSpacePressed() {}
	XComboBoxWidget UseArrowKeysWidget() {}

	void OnBeforeSave();
	void OnShow();
	void OnHide();
	void OnPageHidden() {
		LoadEditedConfigs();
	}
	void OnPageShown() {
		wasShown = true;
		oldConfigContent = GetConfigContent();
		LBLogger.Verbose("Init Admin Page: " + this + " Old Content: " + oldConfigContent.Length() + " (" + oldConfigContent.Hash() + ")", "Core");
	}
	bool DisableMenuBackground();
	void OnRPC(Object target, int rpc_type, ParamsReadContext ctx);
	string GetTooltip(Widget w) {}

	array<string> GetEditedConfigs() {}

	void SaveEditedConfigs() {
		OnBeforeSave();
		array<string> edited = GetEditedConfigs();
		LBLogger.Verbose("Saving Edited configs: " + edited, "Core");
		if (!edited || edited.Count() == 0)
			return;
		int i = 0;
		LBLoadingIcon.Show();
		foreach (string tn : edited) {
			++i;
			LBConfigLoaderBase loaderClass = LBConfigManager.Get().GetLoaderByName(tn + "_");
			LBLogger.Verbose("Logger for: " + tn + " is: " + loaderClass, "Core");
			if (!loaderClass)
				continue;
			loaderClass.Save(i == edited.Count());
		}
		oldConfigContent = GetConfigContent();
	}

	string GetConfigContent() {
		string content = "";
		array<string> edited = GetEditedConfigs();
		LBLogger.Verbose("Getting config content for " + edited, "Core");
		if (!edited || edited.Count() == 0)
			return "";
		foreach (string tn : edited) {
			LBConfigLoaderBase loaderClass = LBConfigManager.Get().GetLoaderByName(tn + "_");
			if (!loaderClass)
				continue;
			content += "" + loaderClass.SaveToString() + "\n";
		}
		return content;
	}

	bool WarnUneditedConfigs(LBAdmin_Menu_Page targetPage) {
		string newContent = GetConfigContent();
		if (oldConfigContent == newContent)
			return false;
		LBLogger.Verbose("Showing warning popup, because Config changed. " + oldConfigContent.Length() + " (" + oldConfigContent.Hash() + ") -> " + newContent.Length() + " (" + newContent.Hash() + ")", "Core");
		warningTargetPage = targetPage;
		LBWarningPopup.Get().Show("Continue", "There are unsaved changes! Do you want to confinue without saving the config?", ScriptCaller.Create(OnWarnConfirm), null, false, "Back");
		return true;
	}

	void OnWarnConfirm() {
		LBAdmin_Menu menu = LBAdmin_Menu.Cast(LBMenuManager.Get().GetOpenLBMenu());
		if (menu && warningTargetPage)
			menu.SetPageActive(warningTargetPage, false);
		else if (menu)
			menu.CloseMe(true);
	}

	void LoadEditedConfigs() {
		if (!wasShown)
			return; // Do not request all configs again if they could not be changed by the admin anyways
		array<string> edited = GetEditedConfigs();
		LBLogger.Verbose("Loading Edited configs: " + edited, "Core");
		if (!edited || edited.Count() == 0)
			return;
		foreach (string tn : edited) {
			LBConfigLoaderBase loaderClass = LBConfigManager.Get().GetLoaderByName(tn + "_");
			LBLogger.Verbose("Logger for: " + tn + " is: " + loaderClass, "Core");
			if (!loaderClass)
				continue;
			loaderClass.Load();
		}
	}

	void SetPBOPrefix(string prefix) {
		this.pboPrefix = prefix;
	}

	void SetCurrentVersion(MultilineTextWidget txt_version) {
		string version = g_Game.ConfigGetTextOut("LBVersions " + pboPrefix + " version");
		if (!g_Game.ConfigIsExisting("LBVersions " + pboPrefix + " version"))
			version = "DEV";

		if (pboPrefix == "LBmaster_Core") {
			txt_version.SetText("Core: v" + version);
		} else {
			string coreVersion = g_Game.ConfigGetTextOut("LBVersions LBmaster_Core version");
			if (!g_Game.ConfigIsExisting("LBVersions LBmaster_Core version"))
				coreVersion = "DEV";
			int index4 = pboPrefix.IndexOf("_") + 1;
			string pboBeautified = pboPrefix.Substring(index4, pboPrefix.Length() - index4);
			txt_version.SetText("Core: v" + coreVersion + "\n" + pboBeautified + ": v" + version);
		}
	}

	// Required Overwrites
	string GetLayoutName() {
		return "AdminMenu_" + ClassName();
	}

	bool HasShowPermissions() {
		return showPermission.Length() == 0 || LBAdmins.Get().HasPermission(showPermission);
	}
	void UpdateButtonVisibility() {
		buttonWidget.Show(CanShowButton() && HasShowPermissions());
	}

	void ApplyWidgetPermission(string widgetName, string permission) {
		Widget w = layoutRoot.FindAnyWidget(widgetName);
		ApplyWidgetPermission(w, permission);
	}

	void ApplyWidgetPermission(Widget w, string permission) {
		if (!w)
			return;
		w.Show(LBAdmins.Get().HasPermission(permission));
	}

	override bool OnChange(Widget w, int x, int y, bool finished) {
		if (super.OnChange(w, x, y, finished))
			return true;
		return linked.OnVarChange(w, -1);
	}

	override bool OnDropReceived(Widget w, int x, int y, Widget reciever) {
		if (super.OnDropReceived(w, x, y, reciever))
			return true;
		return linked.OnDropReceived(w, x, y, reciever);
	}

	override bool OnClick(Widget w, int x, int y, int button) {
		if (super.OnClick(w, x, y, button))
			return true;
		return linked.OnVarChange(w, -1);
	}

	override bool OnKeyPress(Widget w, int x, int y, int key) {
		return super.OnKeyPress(w, x, y, key);
	}

	override bool OnKeyDown(Widget w, int x, int y, int key) {
		return linked.OnKeyDown(w, x, y, key) | super.OnKeyDown(w, x, y, key);
	}

	override bool OnMouseEnter(Widget w, int x, int y) {
		return linked.OnMouseEnter(w, x, y) | super.OnMouseEnter(w, x, y);
	}

	override bool OnKeyUp(Widget w, int x, int y, int key) {
		bool ok = super.OnKeyUp(w, x, y, key);
		return linked.OnKeyUp(w, x, y, key) || ok;
	}

	void KeyDown(int key) {
		Widget focussed = GetFocus();
		if (!ButtonWidget.Cast(focussed))
			linked.OnVarChange(focussed, key);
		if (g_Game.IsLeftCtrlDown()) {
			if (key == KeyCode.KC_S) {
				SaveEditedConfigs();
			} else if (key == KeyCode.KC_C) {
				string str;
				linked.CopySelectedListEntry(str);
			} else if (key == KeyCode.KC_V) {
				linked.PasteCopiedValue();
			}
		} else {
			if (key == KeyCode.KC_DELETE) {
				linked.DeleteSelectedListEntry();
			}
		}
	}

	override bool OnItemSelected(Widget w, int x, int y, int row, int column, int oldRow, int oldColumn) {
		if (super.OnItemSelected(w, x, y, row, column, oldRow, oldColumn))
			return true;

		return linked.OnVarChange(w, -1);
	}

}
