class LBCopiedPopup {

	static const float fadeInTime = 0.1;
	static const float showTime = 1.0;
	static const float fadeOutTime = 0.2;

	static Widget currentWidget;
	static float showStartTime = 0;

	static TextWidget txt_message;

	static void ShowAtWidget(Widget widget, string text = "Copied!") {
		float x, y, w, h;
		widget.GetScreenPos(x, y);
		widget.GetScreenSize(w, h);
		Show(x + w, y + (h / 2), text);
	}

	static void ShowAtCursor(string text = "Copied!") {
		int x, y;
		GetMousePos(x, y);
		Show(x, y, text);
	}

	static void Show(int x, int y, string text = "Copied!") {
		Hide();
		currentWidget = LBLayoutManager.Get().CreateLayout("LBCopiedPopup");
		Update(0.0);
		currentWidget.SetScreenPos(x, y);
		txt_message = TextWidget.Cast(currentWidget.FindAnyWidget("txt_message"));
		txt_message.SetText(text);
	}

	static void Hide() {
		if (currentWidget)
			currentWidget.Unlink();
		currentWidget = null;
		showStartTime = 0;
	}

	static void Update(float timeslice) {
		if (!currentWidget)
			return;
		showStartTime += timeslice;
		if (showStartTime < fadeInTime) {
			float percent = showStartTime / fadeInTime;
			currentWidget.SetAlpha(percent);
		} else if (showStartTime < fadeInTime + showTime) {
			currentWidget.SetAlpha(1.0);
		} else if (showStartTime < fadeInTime + showTime + fadeOutTime) {
			percent = 1.0 - (showStartTime - fadeInTime - showTime) / fadeOutTime;
			currentWidget.SetAlpha(percent);
		} else {
			Hide();
		}
	}

}