class LBWarningPopup : ScriptedWidgetEventHandler {

	static ref LBWarningPopup g_LBWarningPopup;

	private Widget rootWidget;
	private Widget warningMessage;
	private MultilineTextWidget warningText;
	private ButtonWidget cancelWarn, confirmWarn;
	private TextWidget text, text_cancel;
	private ref ScriptCaller confirmCaller, cancelCaller;
	private bool checkControls_ = false, spacePressed = true;

	private void LBWarningPopup() {
	}

	void ~LBWarningPopup() {
		if (rootWidget) {
			rootWidget.Unlink();
		}
	}

	static LBWarningPopup Get() {
		if (!g_LBWarningPopup) {
			g_LBWarningPopup = new LBWarningPopup();
			g_LBWarningPopup.Init();
		}
		return g_LBWarningPopup;
	}

	static void Delete() {
		g_LBWarningPopup = null;
	}

	void Init() {
		rootWidget = LBLayoutManager.Get().CreateLayout("WarningMessage", null);
		ConnectClassWidgetVariables(this, rootWidget);
		rootWidget.SetHandler(this);
		Hide();
	}

	void Show(string buttonText, string warning, ScriptCaller onConfirm = null, ScriptCaller onCancel = null, bool checkControls = false, string cancelText = "#cancel") {
		if (!rootWidget)
			Init();
		if (!rootWidget) {
			Error("Could not initialize Warning Popup Widget");
			return;
		}
		text_cancel.SetText(cancelText);
		checkControls_ = checkControls;
		warningText.SetText(warning);
		warningText.Update();
		rootWidget.Show(true);
		text.SetText(buttonText);
		confirmWarn.Show(buttonText.Length() > 0);
		confirmCaller = onConfirm;
		cancelCaller = onCancel;

		if (checkControls) {
			g_Game.GetUIManager().ShowUICursor(true);
			Mission mission = g_Game.GetMission();
			if (mission) {
				mission.AddActiveInputExcludes({"movement", "aiming", "menu"});
				GetElevatorGame().ShowIngameHud(false);
			}
		}
	}

	bool IsVisible() {
		return rootWidget && rootWidget.IsVisible();
	}

	void Hide() {
		if (rootWidget)
			rootWidget.Show(false);
		if (checkControls_ && !g_Game.GetUIManager().GetMenu()) {
			Mission mission = g_Game.GetMission();
			if (mission) {
				GetElevatorGame().ShowIngameHud(true);
				mission.PlayerControlEnable(true);
				mission.RemoveActiveInputExcludes({"menu", "movement", "aiming"}, true);
				mission.RefreshExcludes();
			}
			g_Game.GetUIManager().ShowUICursor(false);
		}
	}

	static void OnFrame() {
		if (g_LBWarningPopup)
			g_LBWarningPopup.OnFrameLocal();
	}

	void OnFrameLocal() {
		if (IsVisible() && GetUApi()) {
			if (GetUApi().GetInputByName("UAUIBack").LocalPress()) {
				GetUApi().GetInputByName("UAUIBack").Supress();
				Hide();
			} else if (!spacePressed && g_Game.LBIsKeyDown(KeyCode.KC_SPACE)) {
				Confirm();
			}
			spacePressed = g_Game.LBIsKeyDown(KeyCode.KC_SPACE);
		}
	}

	void Confirm() {
		if (!confirmWarn.IsVisible())
			return;
		if (confirmCaller)
			confirmCaller.Invoke();
		Hide();
	}

	override bool OnClick(Widget w, int x, int y, int button) {
		if (super.OnClick(w, x, y, button)) {
			return true;
		}
		if (w == cancelWarn) {
			if (cancelCaller)
				cancelCaller.Invoke();
			Hide();
			return true;
		} else if (w == confirmWarn) {
			Confirm();
			return true;
		} else if (w == rootWidget) {
			if (cancelCaller)
				cancelCaller.Invoke();
			Hide();
			return true;
		}
		return false;
	}

}