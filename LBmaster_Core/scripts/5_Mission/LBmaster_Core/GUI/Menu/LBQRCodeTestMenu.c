class LBQRCodeTestMenu : LBMenuBase {

	CanvasWidget canvas;
	EditBoxWidget edit_text;

	override string GetLayoutName() {
		return "LBQRCodeTestMenu";
	}

	override bool OnChange(Widget w, int x, int y, bool finished) {
		if (super.OnChange(w, x, y, finished))
			return true;
		if (w == edit_text) {
			LB_QRGenerator.generateQRCode(edit_text.GetText(), canvas);
			return true;
		}
		return false;
	}

}