class LBMenuBase : UIScriptedMenu {

	bool justInit = true;
	bool forceMenuOpen = false;
	LBMenuData openParameter;
	ref map<string, ref Param2<float, float>> originalWidgetSize = new map<string, ref Param2<float, float>>();

	void SetForceOpen(bool forceOpen) {
		forceMenuOpen = forceOpen;
	}

	override Widget Init() {
		justInit = true;
		if (layoutRoot)
			return layoutRoot;
		LBLogger.Debug("Initializing menu: " + this, "Core");
		layoutRoot = LBLayoutManager.Get().CreateLayout(GetLayoutName());
		TStringArray ignore = GetConnectIgnoredFields();
		TStringArray rename = GetConnectRenameFields();
		if (layoutRoot)
			ConnectClassWidgetVariables(this, layoutRoot, ignore, rename);
		else
			LBLogger.Fatal("Could not create layout " + GetLayoutName() + " for menu " + this + "!", "Core");
		OnInit();
		return layoutRoot;
	}

	void OnDataReceived() {}
	void OnDataReceivedFirst() {
		OnDataReceived();
	}
	void OnInit() {}
	string GetTooltip(Widget w) {}

	bool CanShowMenu() {
		return true;
	}

	Widget GetCloseButton() {
		return null;
	}
	string GetLayoutName() {
		return "";
	}
	string GetCloseUAInput() {
		return "";
	}
	int GetPriority() {
		return 100;
	}
	bool LBCloseOnEscape() {
		return true;
	}
	bool DisableCharacterControl() {
		return true;
	}
	bool DisableHud() {
		return true;
	}
	bool DisableCursor() {
		return false;
	}
	bool CanCloseWithESC() {
		return true;
	}
	bool SavePersistentState() {
		return false;
	}
	float GetBlur() {
		return 0.0;
	}
	void CloseMe(bool forceClose = false) {
		if (forceClose || !forceMenuOpen) {
			SetForceOpen(false);
			if (SavePersistentState()) {
				UIScriptedMenu parent = UIScriptedMenu.Cast(GetParentMenu());
				g_Game.GetUIManager().HideScriptedMenu(this);
				if (parent)
					g_Game.GetUIManager().ShowScriptedMenu(parent, parent.GetParentMenu());
			} else {
				Close();
			}
		}
	}
	TStringArray GetConnectIgnoredFields() {
		return new TStringArray();
	}
	TStringArray GetConnectRenameFields() {
		return new TStringArray();
	}

	void OnRPC(Object target, int rpc_type, ParamsReadContext ctx);

	void OnClickLB(Widget w) {}

	override bool OnClick(Widget w, int x, int y, int button) {
		if (super.OnClick(w, x, y, button))
			return true;
		if (w && w == GetCloseButton()) {
			CloseMe();
			return true;
		}
		OnClickLB(w);
		return false;
	}

	int mapDownTime, mapDownX, mapDownY;

	override bool OnMouseButtonDown(Widget w, int x, int y, int button) {
		super.OnMouseButtonDown(w, x, y, button);
		if (w.IsInherited(MapWidget)) {
			mapDownTime = g_Game.GetTime();
			mapDownX = x;
			mapDownY = y;
		}
		return false;
	}

	override bool OnMouseButtonUp(Widget w, int x, int y, int button) {
		super.OnMouseButtonUp(w, x, y, button);
		if (w.IsInherited(MapWidget)) {
			if (g_Game.GetTime() - mapDownTime < 300) {
				int dist = (x - mapDownX) * (x - mapDownX) + (y - mapDownY) * (y - mapDownY);
				LBLogger.Verbose("Distance between button down and up: " + dist, "Core");
				if (dist < 25) {
					mapDownTime = 0;
					return OnClick(w, x, y, button);
				}
			}
		}
		return false;
	}

	override bool OnMouseEnter(Widget w, int x, int y) {
		if (super.OnMouseEnter(w, x, y))
			return true;
		string text = GetTooltip(w);
		if (text != "")
			LBTooltipManager.Get().DisplayTooltip(w, text);
		return false;
	}

	private void GetOriginalImageSize(ImageWidget widget, out float width, out float height, int overwriteWidth, int overwriteHeight) {
		string name = widget.ToString();
		Param2<float, float> dimensions;
		if (originalWidgetSize.Find(name, dimensions)) {
			width = dimensions.param1;
			height = dimensions.param2;
		} else {
			bool visible = widget.IsVisible();
			widget.Show(true);
			widget.GetScreenSize(width, height);
			widget.Show(visible);
			dimensions = new Param2<float, float>(width, height);
			if (overwriteWidth != -1)
				dimensions.param1 = overwriteWidth;
			if (overwriteHeight != -1)
				dimensions.param2 = overwriteHeight;
			originalWidgetSize.Insert(name, dimensions);
		}

		if (overwriteWidth != -1)
			width = overwriteWidth;
		if (overwriteHeight != -1)
			height = overwriteHeight;
	}

	bool SetImageFit(ImageWidget img, string path, bool hideWhenNotFound = true, int overwriteWidth = -1, int overwriteHeight = -1) {
		bool found = false;
		if (FileExist(path) && path != "")
			found = true;
		LBLogger.Verbose("Setting Image " + path + " for " + img + ". Found ? " + found, "Core");
		if (found) {
			img.Show(true);
			img.LoadImageFile(0, path);
			UpdateImageProportions(img, overwriteWidth, overwriteHeight);
			return true;
		} else if (hideWhenNotFound) {
			img.Show(false);
		}
		return false;
	}

	private void UpdateImageProportions(ImageWidget img, int overwriteWidth = -1, int overwriteHeight = -1) {
		int width, height;
		img.GetImageSize(0, width, height);
		float nWidth, nHeight, screenWidth, screenHeight;
		GetOriginalImageSize(img, screenWidth, screenHeight, overwriteWidth, overwriteHeight);
		LBLogger.Verbose("Image Size: " + width + "x" + height + " Image Widget Size: " + screenWidth + "x" + screenHeight, "Core");
		if (width <= screenWidth && height <= screenHeight) {
			img.SetScreenSize(width, height);
		} else if (width <= screenWidth && height >= screenHeight) {
			nWidth = ((float) width) * screenHeight / height;
			img.SetScreenSize(nWidth, screenHeight);
		} else if (height <= screenHeight && width >= screenWidth) {
			nHeight = ((float) height) * screenWidth / width;
			img.SetScreenSize(screenWidth, nHeight);
		} else {
			float scaleW = screenWidth / width;
			float scaleH = screenHeight / height;
			float min = Math.Min(scaleW, scaleH);
			img.SetScreenSize(width * min, height * min);
		}
	}

	override void OnShow() {
		justInit = true;
		super.OnShow();
		LBLogger.Verbose("Showing Menu " + ClassName() + "...", "Core");
		g_Game.GetUIManager().ShowUICursor(!DisableCursor());
		Mission mission = g_Game.GetMission();
		if (mission) {
			if (DisableCharacterControl()) {
				mission.AddActiveInputExcludes({"movement", "aiming", "menu"});
			}
			if (DisableHud()) {
				GetElevatorGame().ShowIngameHud(false);
			}
		}
		justInit = true;
		PPEffects.SetBlurMenu(GetBlur());
	}

	override void OnHide() {
		super.OnHide();
		LBLogger.Verbose("Hiding Menu " + ClassName() + " force open: " + forceMenuOpen, "Core");
		Mission mission = g_Game.GetMission();
		if (mission) {
			if (DisableHud()) {
				GetElevatorGame().ShowIngameHud(true);
			}
			if (DisableCharacterControl()) {
				mission.PlayerControlEnable(true);
				mission.RemoveActiveInputExcludes({"menu", "movement", "aiming"}, true);
				mission.RemoveActiveInputRestriction(EInputRestrictors.INVENTORY);
				mission.RefreshExcludes();
			}
		}
		PPEffects.SetBlurMenu(0);
		if (forceMenuOpen) {
			LBMenuManager.Get().ForceOpenMenu(GetPriority(), ClassName(), this);
		} else if (SavePersistentState()) {
			LBMenuManager.Get().menuInstances.RemoveItem(this);
		}
	}

	override void Update(float timeslice) {
		super.Update(timeslice);
		if (CanCloseWithESC() && g_Game && g_Game.GetInput() && g_Game.GetInput().LocalPress("UAUIBack", false)) {
			CloseMe(false);
		}
		if (!justInit && GetCloseUAInput() != "" && g_Game && g_Game.GetInput() && g_Game.GetInput().LocalPress(GetCloseUAInput(), false)) {
			CloseMe(false);
		}
		justInit = false;
	}

}