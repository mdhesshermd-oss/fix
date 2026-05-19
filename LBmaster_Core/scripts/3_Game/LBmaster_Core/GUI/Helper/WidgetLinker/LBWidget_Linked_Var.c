class LBWidget_Linked_Var : LBWidget_Linker {

	private ImageWidget outputIcon;
	private Widget linkedTo, colorOutput, openFileSelectorTrigger, openListSelectorTrigger, descriptionText;
	private bool invertBool_ = false, loadOnly = false;
	private string lastPath, colorPA, colorPR, colorPG, colorPB;
	private bool fileSelectorEnabled, listSelectorEnabled;
	private ref TStringArray fileSelectorFilter, listSelectorList;
	private typename enumType;
	private bool enumPretty = false, useEnumType = false, updateParentListmanager = false;
	private ref array<ref Param2<string, string>> comboValues = null;
	private ref LBColorPicker colorPicker = null;
	private int allowAlpha = true, enableColorPicker = false;
	private string listBrowseHeader;

	void LBWidget_Linked_Var(string varPath_, Widget linkedTo_, Class parent_, LBLinkedVarHandler handler_, bool updateParentOnChange_) {
		this.mainParent = parent_;
		this.handler = handler_;
		this.updateParentOnChange = updateParentOnChange_;
		SetVarPath(varPath_);
		LoadReloadTrigger();
		this.linkedTo = linkedTo_;
		this.descriptionText = FindDescriptionText();
	}

	LBWidget_Linked_Var EnableListBrowse(TStringArray list, Widget openButton, string header = "Select an entry from the list") {
		listSelectorEnabled = true;
		listSelectorList = list;
		openListSelectorTrigger = openButton;
		listBrowseHeader = header;
		return this;
	}

	LBWidget_Linked_Var EnableFileSelector(TStringArray filter, ButtonWidget openTrigger) {
		fileSelectorEnabled = true;
		fileSelectorFilter = filter;
		openFileSelectorTrigger = openTrigger;
		return this;
	}

	LBWidget_Linked_Var EnableIconOutput(ImageWidget outputIcon_) {
		outputIcon = outputIcon_;
		return this;
	}

	LBWidget_Linked_Var EnableColorPicker(bool allowAlpha_, string strA = "", string strR = "", string strG = "", string strB = "", Widget colorOutput_ = null) {
		enableColorPicker = true;
		colorOutput = colorOutput_;
		allowAlpha = allowAlpha_;
		colorPA = strA;
		colorPR = strR;
		colorPG = strG;
		colorPB = strB;
		return this;
	}

	Widget GetLinkedTo() {
		return linkedTo;
	}

	Widget FindDescriptionText() {
		Widget parent = linkedTo.GetParent();
		if (!parent)
			return null;
		float myPosX, myPosY;
		linkedTo.GetScreenPos(myPosX, myPosY);
		Widget child = parent.GetChildren();
		while (child) {
			if (!TextWidget.Cast(child)) {
				child = child.GetSibling();
				continue;
			}
			float childX, childY;
			child.GetScreenPos(childX, childY);
			if (Math.AbsFloat(childY - myPosY) > 1.0) {
				child = child.GetSibling();
				continue;
			}
			float childW, childH;
			child.GetScreenSize(childW, childH);
			float wDiff = Math.AbsFloat(childX + childW - myPosX);
			if (wDiff < 10.0) {
				child.ClearFlags(WidgetFlags.IGNOREPOINTER);
				return child;
			}
			child = child.GetSibling();
		}
		return null;
	}

	override bool OnMouseEnter(Widget w, int x, int y) {
		if (w == linkedTo || w == descriptionText) {
			Class parent = GetConfigVarParent();
			string comment;
			if (!parent) {
				LBListManagerBase parentList = GetParentListManager();
				if (parentList) {
					string type = parentList.GetType();
					if (LBStringTools.EndsWith(type, "_"))
						type = type.Substring(0, type.Length() - 1);
					if (!LBConfigInfo.Get().GetVariableComment(type, varPartLast, comment))
						return true;
				} else {
					typename tn;
					if (GetConfigVar(tn) && tn == Class) {
						type = GetConfigVarAsClass(false).ClassName();
						if (LBStringTools.EndsWith(type, "_"))
							type = type.Substring(0, type.Length() - 1);
						if (!LBConfigInfo.Get().GetClassComment(type, comment))
							return true;
					} else {
						return true;
					}
				}
			} else {
				type = parent.ClassName();
				if (LBStringTools.EndsWith(type, "_"))
					type = type.Substring(0, type.Length() - 1);
				if (!LBConfigInfo.Get().GetVariableComment(type, varPartLast, comment))
					return true;
			}
			LBTooltipManager.Get().DisplayTooltip(w, comment, linkedTo);
			return 2;
		}
		return false;
	}

	void OnFileSelected(string value) {
		LBLogger.Verbose("File " + value + " was selected", "Core");
		lastPath = value;
		SetStringValue(value);
		Load();
	}

	LBWidget_Linked_Var SetComboValues(TStringArray values) {
		comboValues = new array<ref Param2<string, string>>();
		foreach (string avail : values)
			comboValues.Insert(new Param2<string, string>(avail, avail));
		return this;
	}

	LBWidget_Linked_Var SetComboValues(array<ref Param2<string, string>> values) {
		comboValues = values;
		return this;
	}

	LBWidget_Linked_Var SetEnum(typename name, bool printPretty = false) {
		this.enumType = name;
		this.enumPretty = printPretty;
		this.useEnumType = true;
		return this;
	}

	LBWidget_Linked_Var SetInvertedBool(bool inverted) {
		this.invertBool_ = inverted;
		return this;
	}

	LBWidget_Linked_Var SetPermission(string perm, array<Widget> hiddenOnHoPerm = null) {
		if (perm == "")
			return this;
		if (LBAdmins.Get().HasPermission(perm))
			return this;
		if (linkedTo)
			linkedTo.Show(false);
		if (!hiddenOnHoPerm)
			return this;
		foreach (Widget w : hiddenOnHoPerm) {
			w.Show(false);
		}
		return this;
	}

	LBWidget_Linked_Var SetLoadOnly(bool b) {
		this.loadOnly = b;
		return this;
	}

	void OnColorPicked(int color) {
		typename tn;
		if (colorPA == "" && colorPR == "" && colorPG == "" && colorPB == "") {
			if (GetConfigVar(tn)) {
				if (tn == int) {
					EnScript.SetClassVar(GetConfigVarParent(), varPartLast, 0, color);
				}
			}
		} else {
			int a, r, g, b;
			LBConverter.ARGBToComponents(color, a, r, g, b);
			Class parent = GetConfigVarParent();
			if (colorPA != "")
				EnScript.SetClassVar(parent, varPartLast + colorPA, 0, a);
			if (colorPR != "")
				EnScript.SetClassVar(parent, varPartLast + colorPR, 0, r);
			if (colorPG != "")
				EnScript.SetClassVar(parent, varPartLast + colorPG, 0, g);
			if (colorPB != "")
				EnScript.SetClassVar(parent, varPartLast + colorPB, 0, b);
		}
		if (colorOutput) {
			colorOutput.SetColor(color);
		}
		TriggerOnChange(linkedTo);
	}

	override bool OnVarChanged(Widget w, int key) {
		if (w == openFileSelectorTrigger && fileSelectorEnabled) {
			LBMenuManager.Get().OpenMenu("LBFileSelectorDialog", new LBMenuDataFileSelector(lastPath, "#lb_file_selector", "#lb_select", fileSelectorFilter, ScriptCaller.Create(OnFileSelected)), false, LBMenuManager.Get().GetOpenLBMenu());
			return true;
		}
		if (w == openListSelectorTrigger && listSelectorEnabled) {
			LBMenuManager.Get().OpenMenu("LBListSelectorDialog", new LBMenuDataListSelector(ScriptCaller.Create(OnFileSelected), listSelectorList, listBrowseHeader), false, LBMenuManager.Get().GetOpenLBMenu());
			return true;
		}
		if (w == linkedTo && enableColorPicker) {
			typename tn;
			if (colorPA == "" && colorPR == "" && colorPG == "" && colorPB == "") {
				if (GetConfigVar(tn)) {
					LBColorConfig cfg;
					if (tn == int) {
						colorPicker = new LBColorPicker(w, ScriptCaller.Create(OnColorPicked), GetConfigVarAsInt(false), null, allowAlpha);
						UpdateParentOnChange();
						return true;
					} else if (tn == Class && Class.CastTo(cfg, GetConfigVarAsClass(false))) {
						colorPicker = new LBColorPicker(w, ScriptCaller.Create(OnColorPicked), 0, cfg, allowAlpha);
						UpdateParentOnChange();
						return true;
					}
				}
			} else {
				int a = 0, r = 0, g = 0, b = 0;
				if (GetConfigVar(tn, colorPA) && tn == int) {
					a = GetConfigVarAsInt(false);
				} else if (colorPA == "")
					a = 255;
				if (GetConfigVar(tn, colorPR) && tn == int) {
					r = GetConfigVarAsInt(false);
				}
				if (GetConfigVar(tn, colorPG) && tn == int) {
					g = GetConfigVarAsInt(false);
				}
				if (GetConfigVar(tn, colorPB) && tn == int) {
					b = GetConfigVarAsInt(false);
				}
				colorPicker = new LBColorPicker(w, ScriptCaller.Create(OnColorPicked), ARGB(a, r, g, b), null, allowAlpha);
				UpdateParentOnChange();
				return true;
			}
		}
		if (w != linkedTo || GetConfigVarParent() == null || loadOnly)
			return false;
		CheckBoxWidget chckbx = CheckBoxWidget.Cast(linkedTo);
		if (chckbx) {
			LoadCheckboxValue(chckbx, invertBool_);
			TriggerOnChange(w);
			UpdateParentOnChange();
			return true;
		}
		MultilineEditBoxWidget editLine = MultilineEditBoxWidget.Cast(linkedTo);
		if (editLine) {
			LoadMultilineEditBoxValue(editLine);
			TriggerOnChange(w);
			UpdateParentOnChange();
			return true;
		}
		EditBoxWidget editbx = EditBoxWidget.Cast(linkedTo);
		if (editbx) {
			LoadEditBoxValue(editbx);
			TriggerOnChange(w);
			UpdateParentOnChange();
			return true;
		}
		SliderWidget slider = SliderWidget.Cast(linkedTo);
		if (slider) {
			LoadSliderValue(slider);
			TriggerOnChange(w);
			UpdateParentOnChange();
			return true;
		}
		XComboBoxWidget combo = XComboBoxWidget.Cast(linkedTo);
		if (combo) {
			LoadComboValue(combo);
			TriggerOnChange(w);
			UpdateParentOnChange();
			return true;
		}
		return false;
	}

	bool LoadComboValue(XComboBoxWidget combo) {
		int val = combo.GetCurrentItem();
		float fVal = val;
		string strVal = val.ToString();
		if (comboValues) {
			Param2<string, string> value = comboValues.Get(val);
			val = value.param2.ToInt();
			fVal = value.param2.ToFloat();
			strVal = value.param2;
		}
		typename varType;
		if (!GetConfigVar(varType))
			return false;
		if (varType == int) {
			EnScript.SetClassVar(GetConfigVarParent(), varPartLast, 0, val);
		} else if (varType == float) {
			EnScript.SetClassVar(GetConfigVarParent(), varPartLast, 0, fVal);
		} else if (varType == string) {
			SetStringValue(strVal);
		} else {
			Error("Could not find matching Variable for " + varPath + " and value " + val);
			return false;
		}
		return true;
	}

	override protected void SetComboBoxValue(XComboBoxWidget combo, int selected) {
		if (selected < 0 || selected >= combo.GetNumItems())
			return;
		if (comboValues) {
			string str = GetConfigVarAsString();
			selected = 0;
			foreach (Param2<string, string> value : comboValues) {
				if (value.param2 == str) {
					combo.SetCurrentItem(selected);
					return;
				}
				++selected;
			}
		}
		if (!EditBoxWidget.Cast(linkedTo))
			combo.SetCurrentItem(GetConfigVarAsInt());
		else
			combo.SetItem(selected, GetConfigVarAsString());
	}

	override protected bool LoadVar(Widget w, bool invertBool) {
		if (enableColorPicker && colorOutput) {
			typename tn;
			if (colorPA == "" && colorPR == "" && colorPG == "" && colorPB == "") {
				if (GetConfigVar(tn)) {
					LBColorConfig cfg;
					if (tn == int) {
						colorOutput.SetColor(GetConfigVarAsInt(false));
					} else if (tn == Class && Class.CastTo(cfg, GetConfigVarAsClass(false))) {
						colorOutput.SetColor(cfg.GetColorARGB());
					}
				}
			} else {
				int a = 0, r = 0, g = 0, b = 0;
				if (GetConfigVar(tn, colorPA) && tn == int) {
					a = GetConfigVarAsInt(false);
				} else if (colorPA == "")
					a = 255;
				if (GetConfigVar(tn, colorPR) && tn == int) {
					r = GetConfigVarAsInt(false);
				}
				if (GetConfigVar(tn, colorPG) && tn == int) {
					g = GetConfigVarAsInt(false);
				}
				if (GetConfigVar(tn, colorPB) && tn == int) {
					b = GetConfigVarAsInt(false);
				}
				colorOutput.SetColor(ARGB(a, r, g, b));
			}
			return true;
		}
		return super.LoadVar(w, invertBool);
	}

	override bool Load() {
		if (comboValues) {
			XComboBoxWidget combo = XComboBoxWidget.Cast(linkedTo);
			combo.ClearAll();
			foreach (Param2<string, string> value : comboValues) {
				combo.AddItem(value.param1);
			}
		}
		bool ok = LoadVar(linkedTo, invertBool_);
		if (ok)
			TriggerOnChange(linkedTo);
		if (outputIcon) {
			EditBoxWidget edit = EditBoxWidget.Cast(linkedTo);
			if (edit)
				outputIcon.LoadImageFile(0, edit.GetText());
			else
				outputIcon.LoadImageFile(0, "");
		}
		return ok;
	}

}