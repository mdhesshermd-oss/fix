class LB_QRGenerator {

	static void generateQRCode(string text, CanvasWidget widget, bool clearCanvas = true) {
		// Convert string to array<int> (representing bytes)
		// Create QR code
		LB_QrCode qr = LB_QrCode.encodeText(text);

		// Draw to canvas
		drawQRToCanvas(qr, qr.version, widget, clearCanvas);
	}

	static void drawQRToCanvas(LB_QrCode qr, int version, CanvasWidget widget, bool clear) {
		float canvasWidth, canvasHeight;
		widget.GetScreenSize(canvasWidth, canvasHeight);
		if (clear)
			widget.Clear();

		int qrSize = version * 4 + 17;
		int border = 4;

		// Calculate module size to fit canvas
		float availableSize = min(canvasWidth, canvasHeight);
		float moduleSize = (availableSize - 2.0 * border) / (float)qrSize;

		// Center the QR code
		float startX = (canvasWidth - qrSize * moduleSize) / 2.0;
		float startY = (canvasHeight - qrSize * moduleSize) / 2.0;
		widget.DrawLine(startX - border, startY + availableSize / 2.0 - border, startX + availableSize - border, startY + availableSize / 2.0 - border, availableSize, -1);

		// Draw QR modules
		for (int y = 0; y < qrSize; y++) {
			for (int x = 0; x < qrSize; x++) {
				if (qr.getModule(x, y)) {
					float x1 = startX + x * moduleSize;
					float y1 = startY + y * moduleSize;
					float x2 = x1 + moduleSize;
					float y2 = y1 + moduleSize;

					// Draw filled rectangle using lines
					for (float i = 0; i < moduleSize; i += 1.0) {
						widget.DrawLine(x1, y1 + i, x2, y1 + i, 1.0, ARGB(255, 0, 0, 0)); // Black
					}
				}
			}
		}
	}

	static float min(float a, float b) {
		if (a < b)
			return a;
		return b;
	}
}