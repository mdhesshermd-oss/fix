class LBGapHandler : ScriptedWidgetEventHandler {
	reference int gapHorizontal;
	reference int gapVertical;
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

	void SetSize(float newW, float newH) {
		width = newW;
		height = newH;
		UpdateSize();
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
		if (root.GetParent()) {
			root.GetParent().GetScreenSize(parentW, parentH);
		} else {
			parentW = LBWidgetUtils.screenWidth;
			parentH = LBWidgetUtils.screenHeight;
		}
		if (parentW == 0 || parentH == 0)
			return;
		float newW = width - ((float) gapHorizontal) * LBWidgetUtils.heightScale / parentW;
		float newH = height - ((float) gapVertical) * LBWidgetUtils.heightScale / parentH;
		root.SetSize(newW, newH);
		skipNextUpdate = true;
		root.Update();
		skipNextUpdate = false;
	}
}