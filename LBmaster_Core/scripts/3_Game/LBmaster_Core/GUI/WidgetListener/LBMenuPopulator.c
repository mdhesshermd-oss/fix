class LBMenuPopulator : ScriptedWidgetEventHandler {

	Widget root;

	void OnWidgetScriptInit(Widget w) {
		root = w;
		root.SetHandler(this);
		CheckChildren(w);
	}

	void CheckChildren(Widget start) {
		Widget w = start.GetChildren();
		while (w) {
			CheckChildren(w);
			CheckChild(w);
			w = w.GetSibling();
		}
	}

	void CheckChild(Widget child) {
		if (ButtonWidget.Cast(child) && !child.GetChildren()) {
			Widget overlay = LBLayoutManager.Get().CreateLayout("ButtonOverlay", child);
			string txt = "";
			ButtonWidget.Cast(child).GetText(txt);
			if ((txt == "#lb_save" || child.GetName() == "btn_save") && overlay)
				overlay.SetColor(ARGB(255, 0, 255, 0));
		} else if (EditBoxWidget.Cast(child) && !child.GetChildren()) {
			LBLayoutManager.Get().CreateLayout("EditBoxOverlay", child);
		}
	}

	override bool OnMouseEnter(Widget w, int x, int y) {
		if (EditBoxWidget.Cast(w) || CheckBoxWidget.Cast(w) || MultilineEditBoxWidget.Cast(w)) {
			SetFocus(w);
		}
		return super.OnMouseEnter(w, x, y);
	}
}