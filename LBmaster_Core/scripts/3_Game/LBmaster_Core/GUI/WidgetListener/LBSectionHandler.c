class LBSectionHandler : ScriptedWidgetEventHandler {

	static ref array<ref LBSectionHandler> allHandlers = new array<ref LBSectionHandler>();

	Widget main, content, overlay;
	bool hovering = false;

	void LBSectionHandler(Widget main_, Widget content_) {
		allHandlers.Insert(this);
		main_.SetHandler(this);
		main = main_;
		content = content_;
		overlay = LBLayoutManager.Get().CreateLayout("ButtonOverlay", main);
		overlay.Show(false);
		LBWidgetUtils.SetWidgetAlignment(overlay, LBWidgetUtils.H_BOTTOM);
		overlay.SetSize(1.0, 2.0);
	}

	override bool OnMouseButtonUp(Widget w, int x, int y, int button) {
		if (super.OnMouseButtonUp(w, x, y, button))
			return true;
		if (hovering) {
			if (content.IsVisible()) {
				content.Show(false);
			} else {
				content.Show(true);
			}
			return true;
		}
		return false;
	}

	override bool OnMouseEnter(Widget w, int x, int y) {
		if (super.OnMouseEnter(w, x, y))
			return true;
		hovering = true;
		overlay.Show(true);
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y) {
		if (super.OnMouseLeave(w, enterW, x, y))
			return true;
		hovering = false;
		overlay.Show(false);
		return false;
	}

}