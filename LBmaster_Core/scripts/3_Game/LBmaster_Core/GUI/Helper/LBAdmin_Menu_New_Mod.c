class LBAdmin_Menu_New_Mod : ScriptedWidgetEventHandler {

	ImageWidget buttonImg;
	Widget buttonHighlight;
	Widget buttonWidgets;
	Widget rootPanel;
	const float fadeTime = 0.1;
	float fadeProgress = 0.0;
	bool fadeIn = false;
	bool fadeOut = true;
	float contentHeight = 2;

	private ref LBNewModInfo modInfo;
	ref map<Widget, ref ScriptCaller> buttonActions = new map<Widget, ref ScriptCaller>();

	void LBAdmin_Menu_New_Mod(Widget parent, LBNewModInfo modInfo_) {
		modInfo = modInfo_;
		if (LBNewModsConfig.Get.ignoredMods.Find(modInfo.cfgName) != -1)
			return;
		buttonImg = ImageWidget.Cast(LBLayoutManager.Get().CreateLayout("AdminButtonImage", parent));
		buttonHighlight = buttonImg.FindAnyWidget("panel_selected");
		string iconPath = "LBmaster_Core/logo/other/" + modInfo.cfgName + ".paa";
		if (!FileExist(iconPath)) {
			iconPath = "LBmaster_Core/logo/other/unknown.paa";
		}
		buttonImg.LoadImageFile(0, iconPath);
		buttonImg.SetHandler(this);
		rootPanel = LBLayoutManager.Get().CreateLayout("AdminButtonDropdown", buttonImg);
		rootPanel.SetHandler(this);
		float imgW, imgH;
		buttonImg.GetSize(imgW, imgH);
		rootPanel.SetPos(0, imgH);
		buttonWidgets = rootPanel.FindAnyWidget("content");
		SetHeight(0);
		LBLogger.Verbose("Created new Available Mod " + iconPath + ": " + buttonImg, "Core");
		buttonHighlight.SetColor(ARGB(255, 255, 0, 0));
		AddLine(modInfo.displayName, ScriptCaller.Create(OpenModPage));
		AddLine("Visit Website", ScriptCaller.Create(OpenModPage));
		AddLine("Not interested (Hide)", ScriptCaller.Create(OnNotInterested));
	}

	void OnNotInterested() {
		buttonImg.Unlink();
		LBNewModsConfig.Get.ignoredMods.Insert(modInfo.cfgName);
		LBNewModsConfig.Loader.Save();
		delete this;
	}

	void OpenModPage() {
		g_Game.OpenURL(modInfo.url);
	}

	void FadeIn() {
		if (fadeIn)
			return;
		fadeIn = true;
		fadeOut = false;
		fadeProgress = 0.0;
		OnUpdate(0.001);
		LBTooltipManager.Get().DisplayTooltip(buttonImg, modInfo.description, buttonWidgets);
	}

	void FadeOut() {
		if (fadeOut)
			return;
		fadeOut = true;
		fadeIn = false;
		fadeProgress = fadeTime;
	}

	void CheckMousePos() {
		int x, y;
		GetMousePos(x, y);
		float posX, posY, sizeW, sizeH;
		buttonImg.GetScreenSize(sizeW, sizeH);
		buttonImg.GetScreenPos(posX, posY);
		if (posX <= x && posX + sizeW >= x && posY <= y && posY + sizeH >= y) {
			FadeIn();
			return;
		}
		rootPanel.GetScreenSize(sizeW, sizeH);
		rootPanel.GetScreenPos(posX, posY);
		float mult = LBWidgetUtils.heightScale;
		posX -= 10 * mult;
		sizeW += 20 * mult;
		sizeH += 10 * mult;
		if (posX <= x && posX + sizeW >= x && posY <= y && posY + sizeH >= y) {
			return;
		}
		FadeOut();
	}

	void OnUpdate(float timeslice) {
		if (!buttonImg)
			return;
		CheckMousePos();

		if (fadeIn)
			fadeProgress += timeslice;
		if (fadeOut)
			fadeProgress -= timeslice;
		SetHeight(fadeProgress / fadeTime);
	}

	void SetHeight(float height) { // Sets the height to 0 - 1 of the final content height
		height = Math.Clamp(height, 0, 1);
		float w, h;
		rootPanel.GetSize(w, h);
		float x, y;
		rootPanel.GetScreenPos(x, y);
		if (LBWidgetUtils.screenWidth < w + x) {
			rootPanel.SetScreenPos(LBWidgetUtils.screenWidth - w, y);
		}
		rootPanel.SetSize(w, height * contentHeight);
		if (height > 0)
			rootPanel.Show(true);
		else
			rootPanel.Show(false);
	}

	override bool OnClick(Widget w, int x, int y, int button) {
		if (super.OnClick(w, x, y, button))
			return true;
		ScriptCaller caller = buttonActions.Get(w);
		if (caller)
			caller.Invoke();
		return true;
	}

	void AddLine(string text, ScriptCaller action) {
		Widget parent = LBLayoutManager.Get().CreateLayout("AdminMenuButton", buttonWidgets);
		ButtonWidget btn = ButtonWidget.Cast(parent.FindAnyWidget("btn"));
		btn.SetText(text);
		float btnW, btnH;
		parent.GetSize(btnW, btnH);
		contentHeight += btnH;
		buttonActions.Insert(btn, action);
	}

}
