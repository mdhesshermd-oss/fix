class LBClearInput : ScriptedWidgetEventHandler {

	Widget root;
	ref ScriptCaller callback;

	void OnWidgetScriptInit(Widget w) {
		root = w;
		w.SetHandler(this);
	}

	static void AddClearButton(Widget parent, ScriptCaller callback_) {
		Widget button = LBLayoutManager.Get().CreateLayout("ClearButton", parent);
		LBClearInput input;
		button.GetScript(input);
		if (!input)
			return;
		input.callback = callback_;
	}

	static void OnKeyClearCombo() {
		EditBoxWidget focus = EditBoxWidget.Cast(GetFocus());
		if (!focus)
			return;
		Widget child = focus.GetChildren();
		while (child) {
			ButtonWidget button = ButtonWidget.Cast(child);
			child = child.GetSibling();
			if (!button)
				continue;
			LBClearInput clear;
			button.GetScript(clear);
			if (clear) {
				focus.SetText("");
				if (clear.callback)
					clear.callback.Invoke(focus);
				return;
			}
		}
	}

	override bool OnUpdate(Widget w) {
		if (w == root) {
			Widget parent = w.GetParent();
			if (parent) {
				float pw, ph;
				parent.GetScreenSize(pw, ph);
				w.SetScreenSize(ph, ph);
			}
		}
		return super.OnUpdate(w);
	}

	override bool OnClick(Widget w, int x, int y, int button) {
		if (w == root) {
			EditBoxWidget parent = EditBoxWidget.Cast(w.GetParent());
			if (!parent)
				return true;
			parent.SetText("");
			if (callback)
				callback.Invoke(parent);
			return true;
		}
		return false;
	}

}