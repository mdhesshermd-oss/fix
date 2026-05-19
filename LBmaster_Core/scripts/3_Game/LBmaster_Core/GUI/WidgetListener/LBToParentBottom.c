class LBToParentBottom : ScriptedWidgetEventHandler {

	float width, height;
	Widget root;
	bool skipNextUpdate = false;

	void OnWidgetScriptInit(Widget w) {
		root = w;
		w.GetSize(width, height);
#ifdef WORKBENCH
		UpdateSize();
#endif
		w.SetHandler(this);
	}

	override bool OnUpdate(Widget w) {
		if (w == root) {
			UpdateSize();
		}
		return super.OnUpdate(w);
	}

	void UpdateSize() {
		if (skipNextUpdate) {
			skipNextUpdate = false;
			return;
		}
#ifdef WORKBENCH
		LBWidgetUtils.UpdateScreenDimensions();
#endif
		float parentW, parentH;
		float parentX = 0, parentY = 0;
		if (root.GetParent()) {
			root.GetParent().GetScreenSize(parentW, parentH);
			root.GetParent().GetScreenPos(parentX, parentY);
		} else {
			parentW = LBWidgetUtils.screenWidth;
			parentH = LBWidgetUtils.screenHeight;
		}
		if (parentW == 0 || parentH == 0)
			return;
		float posX, posY;
		root.GetScreenPos(posX, posY);
		float newH = 1.0 - ((posY - parentY) / parentH);
		root.SetSize(width, newH);
		skipNextUpdate = true;
		root.Update();
		skipNextUpdate = false;
	}
}