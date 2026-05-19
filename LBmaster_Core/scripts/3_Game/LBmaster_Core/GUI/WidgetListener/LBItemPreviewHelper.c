class LBItemPreviewHelper : ScriptedWidgetEventHandler {
	private int rotationX, rotationY;
	private vector rotationVector = "0 0 0";

	private Widget preview;

	reference bool allowVertical = true;
	reference bool allowHorizontal = true;

	void OnWidgetScriptInit(Widget w) {
		preview = w;
		preview.SetHandler(this);
	}

	void SetAllowed(bool horizontal, bool vertical) {
		allowVertical = vertical;
		allowHorizontal = horizontal;
	}

	override bool OnMouseButtonDown(Widget w, int x, int y, int button) {
		if (w == preview) {
			g_Game.GetDragQueue().Call(this, "UpdateRotation");
			GetMousePos(rotationX, rotationY);
		}
		return super.OnMouseButtonDown(w, x, y, button);
	}

	void UpdateRotation(int mouse_x, int mouse_y, bool is_dragging) {
		vector o = rotationVector;
		if (allowVertical)
			o[0] = o[0] + (rotationY - mouse_y);
		if (allowHorizontal)
			o[1] = o[1] - (rotationX - mouse_x);
		ItemPreviewWidget itemPreview = ItemPreviewWidget.Cast(preview);
		if (itemPreview)
			itemPreview.SetModelOrientation(o);
		PlayerPreviewWidget playerPreview = PlayerPreviewWidget.Cast(preview);
		if (playerPreview)
			playerPreview.SetModelOrientation(o);

		if (!is_dragging) {
			rotationVector = o;
		}
	}

}