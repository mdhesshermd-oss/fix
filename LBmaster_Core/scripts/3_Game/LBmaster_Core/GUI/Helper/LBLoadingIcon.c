class LBLoadingIcon : Managed {

	private static ImageWidget widget;
	private static int active = 0;
	private static float startTime;
	private static int lastImage = 0;
	private static const float frameTime = 0.033; // 33 FPS for 16 images
	private static float timeout = 0;

	static void Update(float timeslice) {
		if (!active)
			return;
		UpdatePos();
		startTime += timeslice;
		int nextImage = ((int)(startTime / frameTime)) % 16;
		if (nextImage != lastImage) {
			widget.LoadImageFile(0, "set:lb_loading image:" + nextImage);
			lastImage = nextImage;
		}
		if (startTime > timeout)
			Hide();
	}

	static void UpdatePos() {
		int mouseX, mouseY;
		GetMousePos(mouseX, mouseY);
		widget.SetScreenPos(mouseX + 10 * LBWidgetUtils.heightScale, mouseY - 5 * LBWidgetUtils.heightScale);
	}

	static void Hide() {
		if (active > 0)
			--active;
		if (!active && widget)
			widget.Show(false);
	}

	static void Show(float timeout_ = 5.0) {
		timeout = timeout_;
		if (!widget)
			widget = ImageWidget.Cast(LBLayoutManager.Get().CreateLayout("LBLoadingImage"));
		widget.SetSort(2000);
		widget.Show(true);
		UpdatePos();
		++active;
		startTime = 0;
		lastImage = 0;
		widget.LoadImageFile(0, "set:lb_loading image:0");
	}

}