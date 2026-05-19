class LBSimpleInputPopup : ScriptedWidgetEventHandler {

	static ref LBSimpleInputPopup g_LBWarningPopup;

	private Widget rootWidget;
	private MultilineTextWidget infoText;
	private TextWidget text_cancel, desc_cancel, text, desc;
	private ButtonWidget cancelInput, confirmInput;
	private EditBoxWidget input_text;
	private ref ScriptCaller confirmCaller, cancelCaller;
	private bool checkControls_ = false, spacePressed = true;

	private void LBSimpleInputPopup() {
	}

	void ~LBSimpleInputPopup() {
		if (rootWidget) {
			rootWidget.Unlink();
		}
	}

	static LBSimpleInputPopup Get() {
		if (!g_LBWarningPopup) {
			g_LBWarningPopup = new LBSimpleInputPopup();
			g_LBWarningPopup.Init();
		}
		return g_LBWarningPopup;
	}

	static void Delete() {
		g_LBWarningPopup = null;
	}

	void Init() {
		rootWidget = LBLayoutManager.Get().CreateLayout("SimplePopupMessage", null);

		ConnectClassWidgetVariables(this, rootWidget);

		rootWidget.SetHandler(this);
		Hide();
	}

	void Show(string buttonText, string message, string input, ScriptCaller onConfirm = null, ScriptCaller onCancel = null, bool checkControls = false, string cancelText = "#cancel") {
		if (!rootWidget)
			Init();
		if (!rootWidget) {
			Error("Could not initialize Input Popup Widget");
			return;
		}
		input_text.SetText(input);
		text_cancel.SetText(cancelText);
		checkControls_ = checkControls;
		infoText.SetText(message);
		infoText.Update();
		rootWidget.Show(true);
		text.SetText(buttonText);
		confirmInput.Show(buttonText.Length() > 0);
		confirmCaller = onConfirm;
		cancelCaller = onCancel;
		SetFocus(input_text);

		if (checkControls) {
			g_Game.GetUIManager().ShowUICursor(true);
			Mission mission = g_Game.GetMission();
			if (mission) {
				mission.AddActiveInputExcludes({"movement", "aiming", "menu"});
				GetElevatorGame().ShowIngameHud(false);
			}
		}
		rootWidget.Update();
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
			} else if (!spacePressed && g_Game.LBIsKeyDown(KeyCode.KC_SPACE) && GetFocus() != input_text) {
				Confirm();
			}
			if (GetFocus() != input_text)
				spacePressed = g_Game.LBIsKeyDown(KeyCode.KC_SPACE);
			else
				spacePressed = false;
		}
	}

	void Confirm() {
		if (!IsVisible())
			return;
		if (confirmCaller)
			confirmCaller.Invoke(input_text.GetText());
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Call(Hide);
	}

	override bool OnChange(Widget w, int x, int y, bool finished) {
		if (super.OnChange(w, x, y, finished))
			return true;
		if (w == input_text && finished) {
			Confirm();
			return true;
		}
		return false;
	}

	override bool OnClick(Widget w, int x, int y, int button) {
		if (w == cancelInput) {
			if (cancelCaller)
				cancelCaller.Invoke();
			Hide();
			return true;
		} else if (w == confirmInput) {
			Confirm();
			return true;
		} else if (w == rootWidget) {
			if (cancelCaller)
				cancelCaller.Invoke();
			Hide();
			return true;
		}
		return super.OnClick(w, x, y, button);
	}

}