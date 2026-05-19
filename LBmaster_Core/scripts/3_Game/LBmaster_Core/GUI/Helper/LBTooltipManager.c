class LBTooltipManager {

	static ref LBTooltipManager g_LBTooltipManager;

	static LBTooltipManager Get() {
		if (!g_LBTooltipManager)
			g_LBTooltipManager = new LBTooltipManager();
		return g_LBTooltipManager;
	}

	bool active = false;
	Widget currentTarget;
	float x, y, w, h;
	MultilineTextWidget tooltip, tooltip2;

	static const int appearDelay = 300;
	static const int appearFadeTime = 100;
	int appearStart = -1;

	void LBTooltipManager() {
		InitWidget();
		g_Game.GetUpdateQueue(CALL_CATEGORY_SYSTEM).Insert(OnFrame);
	}

	void ~LBTooltipManager() {
		if (g_Game) {
			HideTooltip();
			if (tooltip)
				tooltip.Unlink();
			g_Game.GetUpdateQueue(CALL_CATEGORY_SYSTEM).Remove(OnFrame);
		}
	}

	void InitWidget() {
		if (!tooltip) {
			tooltip = MultilineTextWidget.Cast(LBLayoutManager.Get().CreateLayout("Tooltip"));
			tooltip2 = MultilineTextWidget.Cast(tooltip.FindAnyWidget("txt"));
		}
	}

	void DisplayTooltip(Widget target, LBTranslatedString text, Widget displayAt = null) {
		DisplayTooltip(target, text.Get(), displayAt);
	}

	void DisplayTooltip(Widget target, string text, Widget displayAt = null) {
		HideTooltip();
		InitWidget();
		if (!target || text == "" || !LBTooltipConfig.Get.enabled)
			return;
		if (!displayAt)
			displayAt = target;
		currentTarget = target;
		float dX, dY, dW, dH;
		displayAt.GetScreenPos(dX, dY);
		displayAt.GetScreenSize(dW, dH);
		target.GetScreenPos(x, y);
		target.GetScreenSize(w, h);
		tooltip.SetText(text);
		tooltip2.SetText(text);
		tooltip.Update();
		tooltip2.Update();
		float tW, tH;
		tooltip.GetScreenSize(tW, tH);
		float extra = 5.0 * LBWidgetUtils.heightScale;
		float tX = dX + dW + extra;
		float tY = dY + extra;
		if (tX + tW > LBWidgetUtils.screenWidth)
			tX = dX - tW - extra;
		if (tY + tH > LBWidgetUtils.screenHeight)
			tY = LBWidgetUtils.screenHeight - tH - extra;
		tooltip.SetScreenPos(tX, tY);
		tooltip2.SetScreenPos(tX, tY);
		active = true;
		appearStart = g_Game.GetTime();
	}

	void OnFrame() {
		if (!active)
			return;
		int time = g_Game.GetTime() - appearStart;
		if (!currentTarget || !g_Game.GetUIManager().IsCursorVisible() || !currentTarget.IsVisibleHierarchy()) {
			HideTooltip();
			return;
		}
		if (time < appearDelay || !tooltip) {
			return;
		}
		tooltip.Show(true);
		if (appearFadeTime > 0) {
			float f = time - appearDelay;
			float alpha = Math.Clamp(f / appearFadeTime, 0.0, 1.0);
			tooltip.SetAlpha(alpha);
		} else {
			tooltip.SetAlpha(1.0);
		}
		int mouseX, mouseY;
		GetMousePos(mouseX, mouseY);
		if (mouseX < x || mouseX > x + w || mouseY < y || mouseY > y + h) {
			HideTooltip();
		}
	}

	void HideTooltip() {
		active = false;
		currentTarget = null;
		x = 0;
		y = 0;
		w = 0;
		h = 0;
		if (tooltip)
			tooltip.Show(false);
	}

}