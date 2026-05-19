class LBWidgetUtils {

	const static int V_LEFT = 0x00;
	const static int V_CENTER = 0x140;
	const static int V_RIGHT = 0x100;
	const static int V_BITS = 0x1C0;
	const static int H_TOP = 0x00;
	const static int H_CENTER = 0xA00;
	const static int H_BOTTOM = 0x800;
	const static int H_BITS = 0xE00;
	const static int CLEAR_FLAGS = 0xFC0;

	const static int TEXT_CLEAR_FLAGS = 0x500000;
	const static int TEXT_LEFT = 0x0;
	const static int TEXT_RIGHT = 0x400000;
	const static int TEXT_CENTER = 0x100000;

	static int screenWidth;
	static int screenHeight;
	static float heightScale;
	static float widthScale;

	static ref ScriptInvoker Event_OnScreenSizeChanged = new ScriptInvoker();

	static ref map<string, bool> isChildCache = new map<string, bool>();

	static void DeleteAllChildren(Widget widget) {
		if (!widget)
			return;
		Widget child = widget.GetChildren();
		while (child) {
			child.Unlink();
			child = widget.GetChildren();
		}
	}

	static Object SpawnAndSetItemPreview(ItemPreviewWidget preview, string itemname, int flags = ECE_LOCAL) {
		if (!LBItemHelper.CanSpawnLocalPreview(itemname)) {
			preview.SetItem(null);
			return null;
		}
		Object obj = g_Game.CreateObjectEx(itemname, vector.Zero, flags);
		SetItemPreviewUnsafe_Ignore_Errors(preview, obj);
		return obj;
	}

	static float GetAspectRatio() {
		return ((float) screenWidth) / screenHeight;
	}

	//! https://feedback.bistudio.com/T169315
	static void SetItemPreviewUnsafe_Ignore_Errors(ItemPreviewWidget preview, Object obj) { // Don't ask to fix this warning! This cannot be fixed unless DayZ supports showing Objects in the ItemPreviewWidget by default, then this error will automatically disappear.
		preview.SetItem(obj); // We need to do the unsafe down-casting here, because it's impossible to show items not inheriting from EntityAI otherwise. This method is called whenever we need to down-cast Object to EntityAI to show in a ItemPreview.
	}

	static bool CheckSelectedInRange(XComboBoxWidget combo, out int row, LBLinkedVarHandler linked = null) {
		row = combo.GetCurrentItem();
		if (row >= 0 && row < combo.GetNumItems()) {
			if (linked)
				row = linked.SearchedIndexToListIndex(combo, row);
			return true;
		}
		return false;
	}

	static bool CheckSelectedInRange(TextListboxWidget list, out int row, LBLinkedVarHandler linked = null) {
		row = list.GetSelectedRow();
		if (row >= 0 && row < list.GetNumItems()) {
			if (linked)
				row = linked.SearchedIndexToListIndex(list, row);
			return true;
		}
		return false;
	}

	static bool CheckSelectedInRangle(XComboBoxWidget combo, out int row) {
		row = combo.GetCurrentItem();
		return row >= 0 && row < combo.GetNumItems();
	}

	static void DrawSimpleCircle(MapWidget mapWidget, vector center, float radius, int color = -1) {
		if (radius <= 0)
			return;
		int markerCount = 5 + (radius / 10) / mapWidget.GetScale();
		for (int i = 0; i < markerCount; ++i) {
			float part = ((float) i) / markerCount * Math.PI2;
			float sin = Math.Sin(part) * radius;
			float cos = Math.Cos(part) * radius;
			mapWidget.AddUserMark(center + Vector(sin, 0, cos), "", color, "DZ\\gear\\navigation\\data\\map_bush_ca.paa");
		}
	}

	static void DrawSimpleMarker(MapWidget mapWidget, vector pos, string name = "", int color = -1, string icon = "DZ\\gear\\navigation\\data\\map_bush_ca.paa") {
		mapWidget.AddUserMark(pos, Widget.TranslateString(name), color, icon);
	}

	static void UpdateScreenDimensions() {
		GetScreenSize(screenWidth, screenHeight);
		heightScale = ((float) screenHeight) / 1080.0;
		widthScale = ((float) screenWidth) / 1920.0;
		Event_OnScreenSizeChanged.Invoke();
	}

	static bool IsChild(Widget parentWidget, Widget check, bool skipCache = false) {
		if (!check || !parentWidget)
			return false;
		string str = "" + parentWidget + "#" + check;
		bool is = false;
		if (!skipCache && isChildCache.Find(str, is))
			return is;
		is = IsChildInternal(parentWidget, check);
		isChildCache.Set(str, is);
		return is;
	}
	static bool IsChildInternal(Widget parentWidget, Widget check) {
		Widget next = parentWidget;
		while (next) {
			if (next == check)
				return true;
			if (IsChildInternal(next.GetChildren(), check))
				return true;
			next = next.GetSibling();
		}
		return false;
	}

	static bool IsUnderMouse(Widget w) {
		int mouseX, mouseY;
		GetMousePos(mouseX, mouseY);
		float wX, wY, wW, wH;
		w.GetScreenSize(wW, wH);
		w.GetScreenPos(wX, wY);

		return mouseX >= wX && mouseX <= wX + wW && mouseY >= wY && mouseY <= wY + wH;
	}

	static int WidthToPixel01(float value) {
#ifdef WORKBENCH
		UpdateScreenDimensions();
#endif
		return (int)(value * screenWidth);
	}

	static int HeightToPixel01(float value) {
#ifdef WORKBENCH
		UpdateScreenDimensions();
#endif
		return (int)(value * screenHeight);
	}

	static int WidthToPixel(float hdPixel) {
#ifdef WORKBENCH
		UpdateScreenDimensions();
#endif
		return (int)(hdPixel * widthScale);
	}

	static int HeightToPixel(float hdPixel) {
#ifdef WORKBENCH
		UpdateScreenDimensions();
#endif
		return (int)(hdPixel * heightScale);
	}

	// -1 = left, 0 = center, 1 = right
	static void SetTextAlign(Widget w, int type) {
		w.ClearFlags(TEXT_CLEAR_FLAGS);
		if (type == -1) {
			w.SetFlags(TEXT_LEFT);
		} else if (type == 0) {
			w.SetFlags(TEXT_CENTER);
		} else if (type == 1) {
			w.SetFlags(TEXT_RIGHT);
		}
		w.Update();
	}

	static void SetWidgetAlignment(Widget w, int align = 0) {
		if (!w)
			return;
		w.ClearFlags(CLEAR_FLAGS);
		w.SetFlags(align & CLEAR_FLAGS);
		w.Update();
	}

	static void SetWidgetAlignmentIndex(Widget w, int index = 0) {
		int value = FromIndex(index);
		SetWidgetAlignment(w, value);
	}

	static void SetWidgetPosition(Widget w, vector pos, int align = 0) {
		if (!w)
			return;
		float x = pos[0];
		float y = pos[1];
		if (!IsLeft(align))
			x = 1.0 - x;
		if (!IsTop(align))
			y = 1.0 - y;
		w.SetPos(x, y);
		w.Update();
	}

	static void SetWidgetPositionIndex(Widget w, vector pos, int index = 0) {
		int value = FromIndex(index);
		SetWidgetPosition(w, pos, value);
	}

	static bool IsLeft(int align) {
		return (align & V_BITS) == V_LEFT || align == 0;
	}

	static bool IsRight(int align) {
		return (align & V_BITS) == V_RIGHT;
	}

	static bool IsVCenter(int align) {
		return (align & V_BITS) == V_CENTER;
	}

	static bool IsTop(int align) {
		return (align & H_BITS) == H_TOP || align == 0;
	}

	static bool IsBottom(int align) {
		return (align & H_BITS) == H_BOTTOM;
	}

	static bool IsHCenter(int align) {
		return (align & H_BITS) == H_CENTER;
	}

	static int FromIndex(int index) {
		int h = index / 3;
		int v = index % 3;
		int result = 0;
		if (v == 0) {
			result |= V_LEFT;
		} else if (v == 1) {
			result |= V_CENTER;
		} else {
			result |= V_RIGHT;
		}
		if (h == 0) {
			result |= H_TOP;
		} else if (h == 1) {
			result |= H_CENTER;
		} else {
			result |= H_BOTTOM;
		}
		return result;
	}

	static int ToIndex(int value) {
		int result = 0;
		if (IsLeft(value)) {
			result += 0; // Not needed, but for the completeness
		} else if (IsVCenter(value)) {
			result += 1;
		} else {
			result += 2;
		}
		if (IsTop(value)) {
			result += 0;
		} else if (IsHCenter(value)) {
			result += 3;
		} else {
			result += 6;
		}
		return result;
	}

	static void TestIndexConversion() {
		for (int i = 0; i < 9; i++) {
			int value = FromIndex(i) + 8192;
			int index = ToIndex(value);
			if (index != i) {
				LBLogger.Debug("Failed test for Value: " + i + " Got: " + value + " and Index: " + index, "Core");
			} else {
				LBLogger.Debug("Test Passed for Value: " + i + " Got: " + value + " and Index: " + index, "Core");
			}
		}
	}

	static void CreateLines(ScrollWidget scollPanel, Widget parent, TStringArray lines) {
		float parentW, parentH;
		parent.GetScreenSize(parentW, parentH);
		array<Widget> sectionStack = new array<Widget>();
		sectionStack.Insert(parent);
		for (int i = 0; i < lines.Count(); ++i) {
			if (sectionStack.Count() == 0) {
				LBLogger.Error("Section Stack is empty! You closed more sections than you opened!", "Core");
				sectionStack.Insert(parent);
			}
			string text = LBStringTools.MultiTranslateString(lines.Get(i));
			TStringArray args = ProcessArgsToList(text);
			Widget contentParent = sectionStack.Get(sectionStack.Count() - 1);
			if (text == "") {
				MultilineTextWidget emptyLine = MultilineTextWidget.Cast(LBLayoutManager.Get().CreateLayout("LBMultiLineEmpty", contentParent));
				LBSectionInfo info = ApplyArgs(emptyLine, args, "");
				if (info && info.newSection) {
					OpenSection(emptyLine, sectionStack, info);
				} else if (info) {
					CloseSection(emptyLine, sectionStack);
				}
				continue;
			}
			MultilineTextWidget lineWidget = MultilineTextWidget.Cast(LBLayoutManager.Get().CreateLayout("LBMultiLine", contentParent));
			info = ApplyArgs(lineWidget, args, text);
			if (info && info.newSection) {
				OpenSection(lineWidget, sectionStack, info);
			} else if (info) {
				CloseSection(lineWidget, sectionStack);
			}
		}
		UpdateClipChildren(scollPanel);
	}

	static void OpenSection(Widget parent, array<Widget> sectionStack, LBSectionInfo info) {
		Widget parentContent = parent.GetParent();
		Widget newParent = LBLayoutManager.Get().CreateLayout("WrapSpacer", parentContent);
		Widget indent = newParent.FindAnyWidget("indent");
		indent.SetSize(info.indentPixels, 1);
		Widget content = newParent.FindAnyWidget("content");
		sectionStack.Insert(content);
		if (!info.opened)
			content.Show(false);
		new LBSectionHandler(parent, content);
	}

	static void CloseSection(Widget parent, array<Widget> sectionStack) {
		sectionStack.RemoveOrdered(sectionStack.Count() - 1);
	}

	static void UpdateClipChildren(ScrollWidget parent) {
		float current = parent.GetVScrollPos();
		float width, height;
		parent.GetScreenSize(width, height);
		float end = current + height;
		float currentHeight = 0;
		Widget line = parent.GetChildren();
		while (line) {
			bool doClip = true;
			line.GetScreenSize(width, height);
			if (currentHeight >= current && currentHeight + height <= end)
				doClip = false;
			line.ClearFlags(WidgetFlags.CLIPCHILDREN);
			if (doClip)
				line.SetFlags(WidgetFlags.CLIPCHILDREN);
			currentHeight += height;

			line = line.GetSibling();
		}
	}

	static TStringArray ProcessArgsToList(inout string text) {
		TStringArray argsList = new TStringArray();
		if (text.IndexOf("$args(") != 0)
			return argsList;
		int end = text.IndexOf(");");
		if (end == INDEX_NOT_FOUND)
			return argsList;
		string args = text.Substring(6, end - 6);
		text = text.Substring(end + 2, text.Length() - end - 2);
		TStringArray argsListRaw = new TStringArray();
		args.Split(",", argsListRaw);
		int open = 0;
		foreach (string argRaw : argsListRaw) {
			if (open == 0) {
				argsList.Insert(argRaw);
			} else {
				int last = argsList.Count() - 1;
				argsList.Set(last, argsList.Get(last) + "," + argRaw);
			}
			if (argRaw.IndexOf("(") != INDEX_NOT_FOUND)
				++open;
			if (argRaw.IndexOf(")") != INDEX_NOT_FOUND)
				--open;

		}
		return argsList;
	}

	static LBSectionInfo ApplyArgs(TextWidget widget, TStringArray argsList, string text) {
		widget.SetTextExactSize(16);
		float textSize = 16;
		bool underline = false;
		int alignment = V_LEFT;
		LBSectionInfo ret = null;
		foreach (string arg : argsList) {
			int openIndex = arg.IndexOf("(");
			string argArgs = "";
			if (openIndex == INDEX_NOT_FOUND)
				openIndex = arg.Length();
			else if (arg.Length() - 2 > openIndex)
				argArgs = arg.Substring(openIndex + 1, arg.Length() - 2 - openIndex);
			string argName = arg.Substring(0, openIndex).Trim();
			switch (argName) {
				case "sectionStart":
					TStringArray sectionArgs = new TStringArray();
					argArgs.Split(",", sectionArgs);
					int indent = 0;
					bool defaultOpen = true;
					if (sectionArgs.Count() > 0)
						indent = sectionArgs.Get(0).ToInt();
					if (sectionArgs.Count() > 1 && (sectionArgs.Get(1) == "0" || sectionArgs.Get(1) == "false"))
						defaultOpen = false;
					ret = new LBSectionInfo(true, indent, defaultOpen);
					break;
				case "sectionEnd":
					ret = new LBSectionInfo(false, 0, false);
					break;
				case "bold":
					widget.SetBold(true);
					break;
				case "underline":
					underline = true;
					break;
				case "italic":
					widget.SetItalic(true);
					break;
				case "argb":
					TStringArray rgbArgs = new TStringArray();
					argArgs.Split(",", rgbArgs);
					if (rgbArgs.Count() != 4)
						break;
					int a = rgbArgs.Get(0).Trim().ToInt();
					int r = rgbArgs.Get(1).Trim().ToInt();
					int g = rgbArgs.Get(2).Trim().ToInt();
					int b = rgbArgs.Get(3).Trim().ToInt();
					int color = ARGB(a, r, g, b);
					widget.SetColor(color);
					break;
				case "rgb":
					rgbArgs = new TStringArray();
					argArgs.Split(",", rgbArgs);
					if (rgbArgs.Count() != 3)
						break;
					r = rgbArgs.Get(0).Trim().ToInt();
					g = rgbArgs.Get(1).Trim().ToInt();
					b = rgbArgs.Get(2).Trim().ToInt();
					color = ARGB(255, r, g, b);
					widget.SetColor(color);
					break;
				case "size":
					int size = argArgs.Trim().ToInt();
					widget.SetTextExactSize(size);
					textSize = size;
					break;
				case "indent":
					indent = argArgs.Trim().ToInt();
					Widget parentContent = widget.GetParent();
					Widget newParent = LBLayoutManager.Get().CreateLayout("WrapSpacer", parentContent);
					Widget indentWidget = newParent.FindAnyWidget("indent");
					indentWidget.SetSize(indent, 1);
					Widget content = newParent.FindAnyWidget("content");
					content.AddChild(widget);
					break;
				case "left":
					LBWidgetUtils.SetTextAlign(widget, -1);
					alignment = V_LEFT;
					break;
				case "right":
					LBWidgetUtils.SetTextAlign(widget, 1);
					alignment = V_RIGHT;
					break;
				case "center":
					LBWidgetUtils.SetTextAlign(widget, 0);
					alignment = V_CENTER;
					break;
				case "outline":
					TStringArray outlineArgs = new TStringArray();
					argArgs.Split(",", outlineArgs);
					if (outlineArgs.Count() != 5)
						break;
					a = outlineArgs.Get(0).Trim().ToInt();
					r = outlineArgs.Get(1).Trim().ToInt();
					g = outlineArgs.Get(2).Trim().ToInt();
					b = outlineArgs.Get(3).Trim().ToInt();
					color = ARGB(a, r, g, b);
					size = outlineArgs.Get(4).Trim().ToInt();
					widget.SetOutline(size, color);
					break;
				case "shadow":
					TStringArray shadowArgs = new TStringArray();
					argArgs.Split(",", shadowArgs);
					if (shadowArgs.Count() < 5 || shadowArgs.Count() > 8)
						break;
					a = shadowArgs.Get(0).Trim().ToInt();
					r = shadowArgs.Get(1).Trim().ToInt();
					g = shadowArgs.Get(2).Trim().ToInt();
					b = shadowArgs.Get(3).Trim().ToInt();
					color = ARGB(a, r, g, b);
					size = shadowArgs.Get(4).Trim().ToInt();
					float opacity = 1;
					if (shadowArgs.Count() > 5)
						opacity = shadowArgs.Get(5).Trim().ToFloat();
					float offsetX = 0;
					if (shadowArgs.Count() > 6)
						offsetX = shadowArgs.Get(6).Trim().ToFloat();
					float offsetY = 0;
					if (shadowArgs.Count() > 7)
						offsetY = shadowArgs.Get(7).Trim().ToFloat();

					widget.SetShadow(size, color, opacity, offsetX, offsetY);
					break;
				case "img":
					TStringArray imgArgs = new TStringArray();
					argArgs.Split(",", imgArgs);
					if (imgArgs.Count() != 1 && imgArgs.Count() != 3)
						break;
					string path = imgArgs.Get(0).Trim();
					Widget parent = widget.GetParent();
					widget.Unlink();
					Widget imgParent = LBLayoutManager.Get().CreateLayout("Image", parent);
					ImageWidget img = ImageWidget.Cast(imgParent.FindAnyWidget("img"));
					if (!img.LoadImageFile(0, path)) {
						LBLogger.Error("Could not load image file: " + path + "! Exists: " + FileExist(path), "Core");
					}
					int imgX_, imgY_;
					float imgX, imgY;
					img.GetImageSize(0, imgX_, imgY_);
					imgX = imgX_;
					imgY = imgY_;
					if (imgX == 0 || imgY == 0)
						break;
					if (imgArgs.Count() == 3) {
						string w = imgArgs.Get(1).Trim();
						string h = imgArgs.Get(2).Trim();
						if (w != "a" && h == "a") {
							float scale = imgX;
							imgX = w.ToFloat();
							scale /= imgX;
							imgY /= scale;
						} else if (w == "a" && h != "a") {
							scale = imgY;
							imgY = h.ToFloat();
							if (imgY == 0) {
								scale = 1.0;
								imgX = imgY;
							} else {
								scale /= imgY;
								imgX /= scale;
							}
						} else if (w != "a" && h != "a") {
							imgX = w.ToFloat();
							imgY = h.ToFloat();
						}

					}
					img.SetScreenSize(imgX * LBWidgetUtils.heightScale, imgY * LBWidgetUtils.heightScale);
					imgParent.SetSize(1.0, imgY * LBWidgetUtils.heightScale);
					return ApplyImageArgs(img, argsList);
				case "qrcode":
					TStringArray qrArgs = new TStringArray();
					argArgs.Split(",", qrArgs);
					if (qrArgs.Count() < 2)
						break;
					path = qrArgs.Get(1);
					for (int i = 2; i < qrArgs.Count(); ++i) {
						path += qrArgs[i];
					}
					path = path.Trim();
					parent = widget.GetParent();
					widget.Unlink();
					Widget canvasParent = LBLayoutManager.Get().CreateLayout("EmptyCanvas", parent);
					CanvasWidget canvas = CanvasWidget.Cast(canvasParent.FindAnyWidget("canvas"));
					size = qrArgs.Get(0).Trim().ToInt() * LBWidgetUtils.heightScale;
					canvas.SetScreenSize(size, size);
					canvasParent.SetSize(1.0, size);
					LB_QRGenerator.generateQRCode(path, canvas);
					return ApplyQrCodeArgs(canvas, argsList);
			}
		}
		widget.SetText(text);
		if (underline) {
			widget.Update();
			int txtX, txtY;
			widget.GetTextSize(txtX, txtY);
			textSize *= LBWidgetUtils.heightScale;
			int lines = ((float) txtY) / textSize;
			for (int line = 1; line <= lines; ++line) {
				Widget underlineWidget = LBLayoutManager.Get().CreateLayout("Underline", widget);
				SetWidgetAlignment(underlineWidget, alignment);
				underlineWidget.SetPos(0, (textSize) * line);
				underlineWidget.SetScreenSize(txtX, 1);
				underlineWidget.SetColor(widget.GetColor());
			}
		}
		return ret;
	}

	private static LBSectionInfo ApplyQrCodeArgs(CanvasWidget widget, TStringArray argsList) {
		LBSectionInfo ret = null;
		foreach (string arg : argsList) {
			int openIndex = arg.IndexOf("(");
			string argArgs = "";
			if (openIndex == INDEX_NOT_FOUND)
				openIndex = arg.Length();
			else
				argArgs = arg.Substring(openIndex + 1, arg.Length() - 2 - openIndex);
			string argName = arg.Substring(0, openIndex).Trim();
			switch (argName) {
				case "sectionStart":
					TStringArray sectionArgs = new TStringArray();
					argArgs.Split(",", sectionArgs);
					int indent = 0;
					bool defaultOpen = true;
					if (sectionArgs.Count() > 0)
						indent = sectionArgs.Get(0).ToInt();
					if (sectionArgs.Count() > 1 && (sectionArgs.Get(1) == "0" || sectionArgs.Get(1) == "false"))
						defaultOpen = false;
					ret = new LBSectionInfo(true, indent, defaultOpen);
					break;
				case "sectionEnd":
					ret = new LBSectionInfo(false, 0, false);
					break;
				case "center":
					SetWidgetAlignment(widget, V_CENTER);
					break;
				case "left":
					SetWidgetAlignment(widget, V_LEFT);
					break;
				case "right":
					SetWidgetAlignment(widget, V_RIGHT);
					break;
			}
		}
		return ret;
	}
	private static LBSectionInfo ApplyImageArgs(ImageWidget widget, TStringArray argsList) {
		LBSectionInfo ret = null;
		foreach (string arg : argsList) {
			int openIndex = arg.IndexOf("(");
			string argArgs = "";
			if (openIndex == INDEX_NOT_FOUND)
				openIndex = arg.Length();
			else
				argArgs = arg.Substring(openIndex + 1, arg.Length() - 2 - openIndex);
			string argName = arg.Substring(0, openIndex).Trim();
			switch (argName) {
				case "sectionStart":
					TStringArray sectionArgs = new TStringArray();
					argArgs.Split(",", sectionArgs);
					int indent = 0;
					bool defaultOpen = true;
					if (sectionArgs.Count() > 0)
						indent = sectionArgs.Get(0).ToInt();
					if (sectionArgs.Count() > 1 && (sectionArgs.Get(1) == "0" || sectionArgs.Get(1) == "false"))
						defaultOpen = false;
					ret = new LBSectionInfo(true, indent, defaultOpen);
					break;
				case "sectionEnd":
					ret = new LBSectionInfo(false, 0, false);
					break;
				case "center":
					SetWidgetAlignment(widget, V_CENTER);
					break;
				case "left":
					SetWidgetAlignment(widget, V_LEFT);
					break;
				case "right":
					SetWidgetAlignment(widget, V_RIGHT);
					break;
				case "argb":
					TStringArray rgbArgs = new TStringArray();
					argArgs.Split(",", rgbArgs);
					if (rgbArgs.Count() != 4)
						break;
					int a = rgbArgs.Get(0).Trim().ToInt();
					int r = rgbArgs.Get(1).Trim().ToInt();
					int g = rgbArgs.Get(2).Trim().ToInt();
					int b = rgbArgs.Get(3).Trim().ToInt();
					int color = ARGB(a, r, g, b);
					widget.SetColor(color);
					break;
				case "rgb":
					rgbArgs = new TStringArray();
					argArgs.Split(",", rgbArgs);
					if (rgbArgs.Count() != 3)
						break;
					r = rgbArgs.Get(0).Trim().ToInt();
					g = rgbArgs.Get(1).Trim().ToInt();
					b = rgbArgs.Get(2).Trim().ToInt();
					color = ARGB(255, r, g, b);
					widget.SetColor(color);
					break;
			}
		}
		return ret;
	}

}