class LBColorConfig {

	int a = 255, r = 255, g = 255, b = 255;

	int GetColorARGB() {
		return ARGB(a, r, g, b);
	}

	int GetColorRGB() {
		return ARGB(255, r, g, b);
	}

	static LBColorConfig Init(int a1, int r1, int g1, int b1) {
		LBColorConfig cfg = new LBColorConfig();
		cfg.Set(a1, r1, g1, b1);
		return cfg;
	}

	void Set(int a1, int r1, int g1, int b1) {
		a = a1;
		r = r1;
		g = g1;
		b = b1;
	}

	void Set(int color) {
		LBConverter.ARGBToComponents(color, a, r, g, b);
	}

}