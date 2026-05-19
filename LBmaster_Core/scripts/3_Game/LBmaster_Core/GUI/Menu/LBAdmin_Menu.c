class LBAdmin_Menu : LBMenuBase {

	static string lastOpenedPage = "LBUpdateCheckerMenu";

	Widget pageWidgets;
	Widget buttonWidgets, buttonWidgetsNew;
	MultilineTextWidget txt_version;
	ButtonWidget btn_close_admin, btn_save;
	LBAdmin_Menu_Page activePage;
	ref map<string, ref LBAdmin_Menu_Category> categorizedPages = new map<string, ref LBAdmin_Menu_Category>();
	ref array<ref LBAdmin_Menu_New_Mod> newAvailableMods = new array<ref LBAdmin_Menu_New_Mod>();

	private bool availableModRegistered = false;

	override string GetLayoutName() {
		return "AdminMenu";
	}

	override Widget GetCloseButton() {
		return btn_close_admin;
	}

	void RegisterPage(string pageType, string logopath, string buttonName, string showPermission, string pboPrefix) {
		LBAdmin_Menu_Category category;
		bool canShow = false;
		if (showPermission.Length() == 0 || LBAdmins.Get().HasPermission(showPermission))
			canShow = true;
		LBLogger.Debug("Registering Page " + pageType + " in Category " + category + " -> " + logopath + " can show: " + canShow, "Core");
		if (!canShow)
			return;
		if (!categorizedPages.Find(logopath, category)) {
			category = new LBAdmin_Menu_Category(buttonWidgets, logopath);
			categorizedPages.Insert(logopath, category);
		}
		category.AddPage(pageType, pageWidgets, buttonName, showPermission, pboPrefix);

		UpdateAllButtonVisibilities();
	}

	LBAdmin_Menu_Page GetPage(string page) {
		foreach (string icon, LBAdmin_Menu_Category category : categorizedPages) {
			LBAdmin_Menu_Page _page = category.GetPage(page);
			if (_page)
				return _page;
		}
		return null;
	}

	override void Update(float timeslice) {
		super.Update(timeslice);
		if (activePage) {
			activePage.timeslice = timeslice;
			activePage.OnBeforeUpdate();
			activePage.OnUpdateFrame();
		}
		if (!categorizedPages)
			return;
		foreach (string icon, LBAdmin_Menu_Category category : categorizedPages) {
			category.OnUpdate(timeslice);
		}
		foreach (LBAdmin_Menu_New_Mod available : newAvailableMods) {
			if (available)
				available.OnUpdate(timeslice);
		}
	}

	void OnPageRegisterFinished() {
		//TestConfigInfo();
	}

	void TestConfigInfo() {
		LBConfigInfo.Get().checkedComments = new TStringSet();
		LBLogger.Debug("Testing config info...", "Core");
		foreach (string icon, LBAdmin_Menu_Category category : categorizedPages) {
			LBLogger.Debug("Testing config info for category: " + category, "Core");
			foreach (LBAdmin_Menu_Page page : category.pages) {
				LBLogger.Debug("Testing config info for page " + page.ClassName(), "Core");
				page.linked.TestConfigInfo();
			}
		}
		LBConfigInfo.Get().PrintMissingAdminMenu();
	}

	override string GetTooltip(Widget w) {
		if (activePage)
			return activePage.GetTooltip(w);
		return "";
	}

	override void OnShow() {
		super.OnShow();
		UpdateAllButtonVisibilities();
		SetPageActive("");
		if (g_Game.LBGetNotificationUI())
			g_Game.LBGetNotificationUI().LBClearNotifications();
		LBAdminAuditLog.RemoteLog("AdminMenu", "Opened");
		RegisterAvailableMods();
	}

	void RegisterAvailableMods() {
		if (availableModRegistered)
			return;
		availableModRegistered = true;
		foreach (LBNewModInfo info : g_Game.lb_new_mods_list) {
			RegisterAvailableMod(info);
		}
	}

	void RegisterAvailableMod(LBNewModInfo modInfo) {
		if (g_Game.ConfigIsExisting("LBVersions " + modInfo.cfgName) || g_Game.ConfigIsExisting("cfgmods " + modInfo.cfgName) || g_Game.ConfigIsExisting("cfgpatches " + modInfo.cfgName))
			return;
		LBAdmin_Menu_New_Mod mod = new LBAdmin_Menu_New_Mod(buttonWidgetsNew, modInfo);
		newAvailableMods.Insert(mod);
	}

	override void OnHide() {
		super.OnHide();
		if (activePage)
			activePage.OnHide();
	}

	override void CloseMe(bool forceClose = false) {
		if (!forceClose && activePage && activePage.WarnUneditedConfigs(null)) {
			return;
		}
		super.CloseMe(forceClose);
	}

	LBAdmin_Menu_Page SetPageActive(string classname) {
		if (classname == "") {
			if (activePage)
				return activePage;
			classname = lastOpenedPage;
			LBLogger.Debug("Setting Active Page to last Opened Page: " + lastOpenedPage, "Core");
		}
		foreach (string name, LBAdmin_Menu_Category category : categorizedPages) {
			foreach (LBAdmin_Menu_Page page : category.pages) {
				if ((page.ClassName() == classname || activePage == null && classname == "") && SetPageActive(page))
					return page;
			}
		}
		return null;
	}

	LBAdmin_Menu_Page SetPageActive(LBAdmin_Menu_Page page, bool checkUneditedConfigs = true) {
		if (page == null || page == activePage)
			return page;
		if (!page.HasShowPermissions())
			return null;
		if (activePage) {
			if (checkUneditedConfigs && activePage.WarnUneditedConfigs(page))
				return null;
			activePage.parentCategory.SetSelected(false);
			activePage.OnHide();
			activePage.OnPageHidden();
			activePage.layoutRoot.Show(false);
			activePage.buttonWidget.FindAnyWidget("btnColor").SetColor(ARGB(255, 255, 255, 255));
		}
		activePage = page;
		if (activePage) {
			activePage.parentCategory.SetSelected(true);
			btn_save.Show(activePage.HasSaveButton());
			activePage.SetCurrentVersion(txt_version);
			activePage.InitLayoutRoot();
			activePage.OnShow();
			activePage.OnPageShown();
			activePage.layoutRoot.Show(true);
			if (activePage.DisableMenuBackground()) {
				PPEffects.SetBlurMenu(0);
				layoutRoot.SetColor(ARGB(0, 0, 0, 0));
			} else {
				PPEffects.SetBlurMenu(0.7);
				layoutRoot.SetColor(ARGB(100, 129, 129, 129));
			}
			if (activePage.CanShowButton())
				lastOpenedPage = activePage.ClassName();
			LBLogger.Debug("Currently Opened Page: " + activePage.ClassName() + " Last Opened now: " + lastOpenedPage, "Core");
			activePage.buttonWidget.FindAnyWidget("btnColor").SetColor(ARGB(255, 0, 255, 0));
		}
		LBAdminAuditLog.RemoteLog("AdminMenu", "Page " + activePage.ClassName());
		UpdateAllButtonVisibilities();
		return activePage;
	}

	void UpdateAllButtonVisibilities() {
		foreach (string name, LBAdmin_Menu_Category category : categorizedPages) {
			foreach (LBAdmin_Menu_Page page : category.pages) {
				page.UpdateButtonVisibility();
			}
		}
	}

	override void OnRPC(Object target, int rpc_type, ParamsReadContext ctx) {
		if (activePage)
			activePage.OnRPC(target, rpc_type, ctx);
	}

	override bool OnClick(Widget w, int x, int y, int button) {
		if (super.OnClick(w, x, y, button))
			return true;
		foreach (string name, LBAdmin_Menu_Category category : categorizedPages) {
			foreach (LBAdmin_Menu_Page page : category.pages) {
				if (w == page.buttonWidget) {
					SetPageActive(page);
					return true;
				}
			}
		}
		if (w == btn_save && activePage) {
			activePage.SaveEditedConfigs();
			return true;
		} else if (activePage)
			return activePage.OnClick(w, x, y, button);
		return false;
	}

	override bool OnMouseButtonDown(Widget w, int x, int y, int button) {
		if (super.OnMouseButtonDown(w, x, y, button))
			return true;
		return activePage && activePage.OnMouseButtonDown(w, x, y, button);
	}

	override bool OnMouseButtonUp(Widget w, int x, int y, int button) {
		if (super.OnMouseButtonUp(w, x, y, button))
			return true;
		return activePage && activePage.OnMouseButtonUp(w, x, y, button);
	}

	override bool OnDoubleClick(Widget w, int x, int y, int button) {
		if (super.OnDoubleClick(w, x, y, button))
			return true;
		return activePage && activePage.OnDoubleClick(w, x, y, button);
	}

	override bool OnDropReceived(Widget w, int x, int y, Widget reciever) {
		if (super.OnDropReceived(w, x, y, reciever))
			return true;
		return activePage && activePage.OnDropReceived(w, x, y, reciever);
	}

	override bool OnChange(Widget w, int x, int y, bool finished) {
		if (super.OnChange(w, x, y, finished))
			return true;
		return activePage && activePage.OnChange(w, x, y, finished);
	}

	override bool OnItemSelected(Widget w, int x, int y, int row, int column,	int oldRow, int oldColumn) {
		if (super.OnItemSelected(w, x, y, row, column, oldRow, oldColumn))
			return true;
		return activePage && activePage.OnItemSelected(w, x, y, row, column, oldRow, oldColumn);
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y) {
		if (super.OnMouseLeave(w, enterW, x, y))
			return true;
		return activePage && activePage.OnMouseLeave(w, enterW, x, y);
	}

	override bool OnMouseEnter(Widget w, int x, int y) {
		if (super.OnMouseEnter(w, x, y))
			return true;
		return activePage && activePage.OnMouseEnter(w, x, y);
	}

	override bool OnMouseWheel(Widget w, int x, int y, int wheel) {
		if (super.OnMouseWheel(w, x, y, wheel))
			return true;
		return activePage && activePage.OnMouseWheel(w, x, y, wheel);
	}

	override bool OnFocus(Widget w, int x, int y) {
		if (super.OnFocus(w, x, y))
			return true;
		return activePage && activePage.OnFocus(w, x, y);
	}

	override bool OnFocusLost(Widget w, int x, int y) {
		if (super.OnFocusLost(w, x, y))
			return true;
		return activePage && activePage.OnFocusLost(w, x, y);
	}

	void KeyDown(int key) {
		if (activePage)
			activePage.KeyDown(key);
	}

	override bool OnKeyPress(Widget w, int x, int y, int key) {
		if (super.OnKeyPress(w, x, y, key))
			return true;
		return activePage && activePage.OnKeyPress(w, x, y, key);
	}

	override bool OnKeyDown(Widget w, int x, int y, int key) {
		bool ok = super.OnKeyDown(w, x, y, key);
		return (activePage && activePage.OnKeyDown(w, x, y, key)) || ok;
	}

}