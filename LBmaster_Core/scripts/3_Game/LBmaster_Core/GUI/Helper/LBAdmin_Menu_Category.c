class LBAdmin_Menu_Category : ScriptedWidgetEventHandler {

	ref array<ref LBAdmin_Menu_Page> pages = new array<ref LBAdmin_Menu_Page>();
	ImageWidget buttonImg;
	Widget buttonHighlight;
	Widget buttonWidgets;
	Widget rootPanel;
	const float fadeTime = 0.1;
	float fadeProgress = 0.0;
	bool fadeIn = false;
	bool fadeOut = true;
	float contentHeight = 2;

	void LBAdmin_Menu_Category(Widget parent, string iconPath) {
		buttonImg = ImageWidget.Cast(LBLayoutManager.Get().CreateLayout("AdminButtonImage", parent));
		buttonHighlight = buttonImg.FindAnyWidget("panel_selected");
		buttonImg.LoadImageFile(0, iconPath);
		buttonImg.SetHandler(this);
		rootPanel = LBLayoutManager.Get().CreateLayout("AdminButtonDropdown", buttonImg);
		rootPanel.SetHandler(this);
		float imgW, imgH;
		buttonImg.GetSize(imgW, imgH);
		rootPanel.SetPos(0, imgH);
		buttonWidgets = rootPanel.FindAnyWidget("content");
		SetHeight(0);
		LBLogger.Verbose("Created new Category " + iconPath + ": " + buttonImg, "Core");
	}

	void FadeIn() {
		if (fadeIn)
			return;

		fadeIn = true;
		fadeOut = false;
		fadeProgress = 0.0;
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
		rootPanel.SetSize(w, height * contentHeight);
		if (height > 0)
			rootPanel.Show(true);
		else
			rootPanel.Show(false);
	}

	void SetSelected(bool selected) {
		if (selected)
			buttonHighlight.SetColor(ARGB(255, 0, 255, 0));
		else
			buttonHighlight.SetColor(ARGB(255, 255, 255, 255));
	}

	void AddPage(string pageType, Widget pageWidgets, string buttonName, string showPermission, string pboPrefix) {
		foreach (LBAdmin_Menu_Page page : pages) {
			if (page.ClassName() == pageType)
				return;
		}
		typename pageTn = pageType.ToType();
		if (!pageTn) {
			LBLogger.Fatal("Could not find Classname " + pageTn + " to create Admin Page", "Core");
		}
		page = LBAdmin_Menu_Page.Cast(pageTn.Spawn());
		if (!page) {
			LBLogger.Fatal("Could not create Admin Menu Page: " + pageType, "Core");
			return;
		}
		pages.Insert(page);
		page.SetPBOPrefix(pboPrefix);
		page.Init(pageWidgets, buttonName, showPermission, this);
		Widget parent = LBLayoutManager.Get().CreateLayout("AdminMenuButton", buttonWidgets);
		ButtonWidget btn = ButtonWidget.Cast(parent.FindAnyWidget("btn"));
		btn.SetText(buttonName);
		page.buttonWidget = btn;
		if (page.layoutRoot)
			page.layoutRoot.Show(false);
		if (page.CanShowButton() && page.HasShowPermissions()) {
			float btnW, btnH;
			parent.GetSize(btnW, btnH);
			contentHeight += btnH;
		} else {
			parent.Show(false);
		}
	}

	LBAdmin_Menu_Page GetPage(string page) {
		foreach (LBAdmin_Menu_Page _page : pages) {
			if (_page.ClassName() == page)
				return _page;
		}
		return null;
	}

}
