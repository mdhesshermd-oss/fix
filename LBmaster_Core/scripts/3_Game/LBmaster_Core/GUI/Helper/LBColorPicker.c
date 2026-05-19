class LBColorPicker : ScriptedWidgetEventHandler {

	Widget rootWidget;
	SliderWidget slider_a, slider_r, slider_g, slider_b;
	GridSpacerWidget rootPanel, panel_color;
	Widget panel_alpha;
	ButtonWidget btn_cancel, btn_ok;
	EditBoxWidget edit_a, edit_r, edit_g, edit_b, edit_hex;

	int a, r, g, b;
	bool allowAlpha = true, wasBackground = false;
	ScriptCaller callback;
	LBColorConfig colorCfg;

	void ~LBColorPicker() {
		if (rootWidget) {
			rootWidget.Unlink();
		}
	}

	void LBColorPicker(Widget parent, ScriptCaller selectCallback, int color_ = -1, LBColorConfig colorCfg_ = null, bool allowAlpha_ = true) {
		this.callback = selectCallback;
		this.allowAlpha = allowAlpha_;
		this.colorCfg = colorCfg_;
		if (colorCfg) {
			color_ = colorCfg.GetColorARGB();
		}
		rootWidget = LBLayoutManager.Get().CreateLayout("ColorPicker");
		if (!parent) {
			LBWidgetUtils.SetWidgetAlignment(rootWidget, LBWidgetUtils.H_CENTER);
			LBWidgetUtils.SetWidgetAlignment(rootWidget, LBWidgetUtils.V_CENTER);
		}
		rootWidget.SetHandler(this);
		ConnectClassWidgetVariables(this, rootWidget, {"rootWidget"});
		if (!allowAlpha)
			panel_alpha.Show(false);
		rootPanel.Update();
		float tW, tH, x, y, w, h;
		rootPanel.GetScreenSize(tW, tH);
		parent.GetScreenPos(x, y);
		parent.GetScreenSize(w, h);
		float tX = x;
		float tY = y + h;
		if (tY + tH > LBWidgetUtils.screenHeight)
			tY = y - tH;
		if (tX + tW > LBWidgetUtils.screenWidth)
			tX = LBWidgetUtils.screenWidth - tW;
		rootPanel.SetScreenPos(tX, tY);
		rootPanel.SetSort(1000);

		LoadColor(color_);
	}

	void LoadColor(int color_) {
		LBConverter.ARGBToComponents(color_, a, r, g, b);
		slider_a.SetCurrent(a);
		edit_a.SetText("" + a);
		slider_r.SetCurrent(r);
		edit_r.SetText("" + r);
		slider_g.SetCurrent(g);
		edit_g.SetText("" + g);
		slider_b.SetCurrent(b);
		edit_b.SetText("" + b);
		UpdateColorPanel();
	}

	string GetHexColor() {
		string rest = LBConverter.IntToHex2(r) + LBConverter.IntToHex2(g) + LBConverter.IntToHex2(b);
		if (allowAlpha) {
			return LBConverter.IntToHex2(a) + rest;
		}
		return rest;
	}

	bool ParseColorHex(out int val) {
		string str = edit_hex.GetText();
		if (str == "")
			return false;
		int len = 8;
		if (!allowAlpha)
			len = 6;
		string parseStr = str + "";
		if (str[0] == "#") {
			++len;
			parseStr = str.Substring(1, str.Length() - 1);
		}
		if (str.Length() > len) {
			str = str.Substring(0, len);
			edit_hex.SetText(str);
		}
		if (parseStr.Length() == 3 && !allowAlpha) {
			parseStr = parseStr[0] + parseStr[0] + parseStr[1] + parseStr[1] + parseStr[2] + parseStr[2];
			val = LBConverter.ParseHex(parseStr);
			return true;
		} else if (parseStr.Length() == 4 && allowAlpha) {
			parseStr = parseStr[0] + parseStr[0] + parseStr[1] + parseStr[1] + parseStr[2] + parseStr[2] + parseStr[3] + parseStr[3];
			val = LBConverter.ParseHex(parseStr);
			return true;
		} else if (str.Length() == len) {
			val = LBConverter.ParseHex(parseStr);
			return true;
		}
		return false;
	}

	override bool OnChange(Widget w, int x, int y, bool finished) {
		if (super.OnChange(w, x, y, finished))
			return true;
		if (w == edit_a) {
			OnEditChange(edit_a, slider_a);
			a = edit_a.GetText().ToInt();
			UpdateColorPanel();
			return true;
		} else if (w == edit_r) {
			OnEditChange(edit_r, slider_r);
			r = edit_r.GetText().ToInt();
			UpdateColorPanel();
			return true;
		} else if (w == edit_g) {
			OnEditChange(edit_g, slider_g);
			g = edit_g.GetText().ToInt();
			UpdateColorPanel();
			return true;
		} else if (w == edit_b) {
			OnEditChange(edit_b, slider_b);
			b = edit_b.GetText().ToInt();
			UpdateColorPanel();
			return true;
		} else if (w == slider_a) {
			a = slider_a.GetCurrent();
			edit_a.SetText("" + a);
			UpdateColorPanel();
			return true;
		} else if (w == slider_r) {
			r = slider_r.GetCurrent();
			edit_r.SetText("" + r);
			UpdateColorPanel();
			return true;
		} else if (w == slider_g) {
			g = slider_g.GetCurrent();
			edit_g.SetText("" + g);
			UpdateColorPanel();
			return true;
		} else if (w == slider_b) {
			b = slider_b.GetCurrent();
			edit_b.SetText("" + b);
			UpdateColorPanel();
			return true;
		} else if (w == edit_hex) {
			int colorVal;
			if (ParseColorHex(colorVal)) {
				LoadColor(colorVal);
			}
			return true;
		}
		return false;
	}

	void OnEditChange(EditBoxWidget edit, SliderWidget slider) {
		int value = edit.GetText().ToInt();
		if (value < 0) {
			value = 0;
			edit.SetText("" + value);
		} else if (value > 255) {
			value = 255;
			edit.SetText("" + value);
		}
		slider.SetCurrent(value);
	}

	override bool OnMouseEnter(Widget w, int x, int y) {
		if (super.OnMouseEnter(w, x, y)) {
			return true;
		}
		if (w != edit_hex) {
			int colorVal;
			if (ParseColorHex(colorVal)) {
				LoadColor(colorVal);
			}
		}
		SetFocus(w);
		return false;
	}

	override bool OnClick(Widget w, int x, int y, int button) {
		if (super.OnClick(w, x, y, button))
			return true;
		if (w == btn_cancel) {
			delete this;
		} else if (w == btn_ok) {
			if (callback) {
				if (colorCfg)
					colorCfg.Set(a, r, g, b);
				callback.Invoke(GetColor());
			}
			delete this;
		}
		return false;
	}

	override bool OnMouseButtonDown(Widget w, int x, int y, int button) {
		if (!g_Game.IsAppActive())
			wasBackground = true;
		return super.OnMouseButtonDown(w, x, y, button);
	}

	override bool OnMouseButtonUp(Widget w, int x, int y, int button) {
		if (super.OnMouseButtonUp(w, x, y, button))
			return true;
		if (w == rootWidget && !wasBackground) {
			delete this;
		}
		wasBackground = false;
		return false;
	}

	int GetColor() {
		if (allowAlpha)
			return ARGB(a, r, g, b);
		return ARGB(255, r, g, b);
	}

	void UpdateColorPanel() {
		panel_color.SetColor(GetColor());
		if (GetFocus() != edit_hex)
			edit_hex.SetText(GetHexColor());
	}

}