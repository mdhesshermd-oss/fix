class LBValidator : ScriptedWidgetEventHandler {

	Widget root;

	void OnWidgetScriptInit(Widget w) {
		root = w;
		if (g_Game)
			g_Game.GetUpdateQueue(CALL_CATEGORY_SYSTEM).Insert(CheckValid);
	}

	void CheckValid(float timeslice) {
		EditBoxWidget txt = EditBoxWidget.Cast(root);
		if (txt) {
			string text = txt.GetText();
			bool valid = IsValid(text);
			int color = 0;
			if (GetColor(valid, color))
				txt.SetTextColor(color);
			string newValue = GetSanitizedValue(text);
			if (newValue != text) {
				txt.SetText(newValue);
				txt.Update();
			}
		}
	}

	bool GetColor(bool valid, out int color) {
		if (valid)
			color = -16711936;
		else
			color = -55256;
		return true;
	}

	bool IsValid(string value);
	string GetSanitizedValue(string value) {
		return value;
	}

}