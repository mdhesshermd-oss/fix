
class LBWidget_Linker {

	LBLinkedVarHandler handler;

	protected string varPath; // can be a.b.c or only a
	protected ref TStringArray varParts = new TStringArray();
	protected string varPartLast = "";
	protected Class mainParent;
	protected Widget reloadTrigger;
	protected string onChangeTrigger = "", possibleChangeTrigger;
	protected bool updateParentOnChange = false;
	private string setterFunction = "";

	private string string_;
	private int int_;
	private float float_;
	private bool bool_;
	private Class class_;

	bool IsVarPath(string other) {
		return varPath == other;
	}

	string GetVarPath() {
		return varPath;
	}

	void LoadReloadTrigger() {
		int len = varParts.Count() - 1;
		int rem = varPath.Length();
		for (int i = len; i >= 0; i--) {
			string varName = varParts.Get(i);
			int start = varName.LastIndexOf("[");
			int end = varName.LastIndexOf("]");
			rem -= varName.Length() + 1;
			if (start != -1 && end != -1) {
				string widgetName = varName.Substring(start + 1, end - start - 1);
				Widget found = handler.FindWidget(widgetName);
				rem += start + 1;
				possibleChangeTrigger = varPath.Substring(0, rem);
				LBLogger.Verbose("Automatically setting reload trigger for " + varPath + " to " + widgetName + " Found: " + found + " triggering Change for " + possibleChangeTrigger, "Core");
				SetReloadTrigger(found);
				return;
			}
		}
	}

	void UpdateParentOnChange() {
		if (updateParentOnChange) {
			LBLogger.Debug("Loading Parent selected entry: " + possibleChangeTrigger, "Core");
			handler.TriggerLoadLine(possibleChangeTrigger);
		}
	}

	protected void SetVarPath(string varPath_) {
		this.varPath = varPath_;
		this.varParts = LBStringTools.Split(varPath, ".");
		this.varPartLast = varParts.Get(varParts.Count() - 1);
	}

	protected Class GetConfigVarParent() {
		Class found = mainParent;
		int len = varParts.Count() - 1;
		string searchedPath = "";
		for (int i = 0; i < len; i++) {
			string varName = varParts.Get(i);
			Class temp = null;
			if (varName.IndexOf("()") == varName.Length() - 2 && varName.Length() > 1) {
				varName = varName.Substring(0, varName.Length() - 2);
				g_Game.GameScript.CallFunction(found, varName, temp, null);
			} else if (varName.LastIndexOf("[") > 0 && varName.LastIndexOf("]") == varName.Length() - 1 && varName.Length() > 0) {
				int start = varName.LastIndexOf("[");
				int end = varName.Length() - 1;
				string name = varName.Substring(0, start);
				string indexName = varName.Substring(start + 1, varName.Length() - start - 2);
				int index = GetCurrentIndex(indexName);
				string searchFor = searchedPath + "." + name;
				if (searchedPath == "")
					searchFor = name;
				LBListManagerBase listMgr = handler.GetListManager(searchFor);
				LBLogger.Verbose("List Manager for " + searchFor + " is " + listMgr + " Me: " + varPath + " current index: " + index, "Core");
				temp = listMgr.GetValue(index);
			} else {
				EnScript.GetClassVar(found, varName, 0, temp);
			}

			if (searchedPath == "")
				searchedPath = varParts.Get(i);
			else
				searchedPath += "." + varParts.Get(i);
			found = temp;
			if (temp == null)
				return null;
		}
		return found;
	}

	LBListManagerBase GetParentListManager() {
		int len = varParts.Count() - 1;
		string searchedPath = "";
		for (int i = 0; i < len; i++) {
			string varName = varParts.Get(i);
			if (varName.IndexOf("()") == varName.Length() - 2 && varName.Length() > 1) {
				return null;
			} else if (varName.LastIndexOf("[") > 0 && varName.LastIndexOf("]") == varName.Length() - 1 && varName.Length() > 0) {
				int start = varName.LastIndexOf("[");
				string name = varName.Substring(0, start);
				string searchFor = searchedPath + "." + name;
				if (searchedPath == "")
					searchFor = name;
				return handler.GetListManager(searchFor);
			}
		}
		return null;
	}

	int GetCurrentIndex(string widgetName) {
		Widget widget = handler.FindWidget(widgetName);
		BaseListboxWidget listBox = BaseListboxWidget.Cast(widget);
		if (listBox)
			return handler.SearchedIndexToListIndex(widget, listBox.GetSelectedRow());
		XComboBoxWidget combo = XComboBoxWidget.Cast(widget);
		if (combo)
			return handler.SearchedIndexToListIndex(widget, combo.GetCurrentItem());
		return -1;
	}

	bool OnKeyDown(Widget w, int x, int y, int key) {
		return false;
	}

	bool OnKeyUp(Widget w, int x, int y, int key) {
		return false;
	}

	bool OnMouseEnter(Widget w, int x, int y) {
		return false;
	}

	protected bool GetConfigVar(out typename type_, string lastAdd = "") {
		Class parent = GetConfigVarParent();
		if (parent == null)
			return false;
		typename parentType = parent.Type();
		string last = varPartLast + lastAdd;
		if (last.IndexOf("()") == last.Length() - 2 && last.Length() > 2) {
			string funcName = last.Substring(0, last.Length() - 2);
			string value;
			g_Game.GameScript.CallFunction(parent, funcName, value, null);
			type_ = string;
			string_ = value;
			return true;
		}
		int varIndex = GetVariableIndex(parentType, last);
		if (varIndex == -1) {
			return false;
		}
		type_ = parentType.GetVariableType(varIndex);
		if (type_ == int) {
			EnScript.GetClassVar(parent, last, 0, int_);
			return true;
		} else if (type_ == string) {
			EnScript.GetClassVar(parent, last, 0, string_);
			return true;
		} else if (type_ == float) {
			EnScript.GetClassVar(parent, last, 0, float_);
			return true;
		} else if (type_ == bool) {
			EnScript.GetClassVar(parent, last, 0, bool_);
			return true;
		} else if (type_ == vector) {
			vector vec;
			EnScript.GetClassVar(parent, last, 0, vec);
			string_ = "" + vec;
			type_ = string;
			return true;
		} else {
			EnScript.GetClassVar(parent, last, 0, class_);
			type_ = Class;
			return true;
		}
		return false;
	}

	protected Class GetConfigVarAsClass(bool checkType = true) {
		if (!checkType)
			return class_;
		typename type;
		if (!GetConfigVar(type))
			return null;
		if (type.IsInherited(Class))
			return class_;
		return null;
	}

	protected string GetConfigVarAsString(bool checkType = true) {
		if (!checkType)
			return string_;
		typename type;
		if (!GetConfigVar(type))
			return "";
		if (type == string)
			return string_;
		if (type == int)
			return int_.ToString();
		if (type == float)
			return float_.ToString();
		if (type == bool)
			return bool_.ToString();
		return class_.ToString();
	}

	protected bool GetConfigVarAsBool(bool checkType = true) {
		if (!checkType)
			return bool_;
		typename type;
		if (!GetConfigVar(type))
			return false;
		if (type == bool)
			return bool_;
		if (type == int)
			return int_;
		if (type == float)
			return float_;
		if (type == string)
			return string_ == "true" || string_ == "1";
		return false;
	}

	protected int GetConfigVarAsInt(bool checkType = true) {
		if (!checkType)
			return int_;
		typename type;
		if (!GetConfigVar(type))
			return false;
		if (type == bool)
			return bool_;
		if (type == int)
			return int_;
		if (type == float)
			return float_;
		if (type == string)
			return string_.ToInt();
		return 0;
	}

	protected float GetConfigVarAsFloat(bool checkType = true) {
		if (!checkType)
			return float_;
		typename type;
		if (!GetConfigVar(type))
			return false;
		if (type == bool)
			return bool_;
		if (type == int)
			return int_;
		if (type == float)
			return float_;
		if (type == string)
			return string_.ToFloat();
		return 0;
	}

	LBWidget_Linker SetSetter(string function) {
		setterFunction = function;
		if (setterFunction.IndexOf("()") == setterFunction.Length() - 2) {
			setterFunction = setterFunction.Substring(0, setterFunction.Length() - 2);
		}
		return this;
	}

	protected void LoadCheckboxValue(CheckBoxWidget chckbx, bool invertBool) {
		bool selected = chckbx.IsChecked();
		if (invertBool)
			selected = !selected;
		SetBoolValue(selected);
	}

	protected bool LoadMultilineEditBoxValue(MultilineEditBoxWidget editbx) {
		string text;
		editbx.GetText(text);
		typename varType;
		if (!GetConfigVar(varType))
			return false;
		if (varType == int) {
			int intVal = text.ToInt();
			EnScript.SetClassVar(GetConfigVarParent(), varPartLast, 0, intVal);
		} else if (varType == float) {
			float floatVal = text.ToFloat();
			EnScript.SetClassVar(GetConfigVarParent(), varPartLast, 0, floatVal);
		} else if (varType == string) {
			SetStringValue(text);
		} else {
			Error("Could not find matching Variable for " + varPath + " and text " + text);
			return false;
		}
		return true;
	}

	protected bool LoadEditBoxValue(EditBoxWidget editbx) {
		string text = editbx.GetText();
		typename varType;
		if (!GetConfigVar(varType))
			return false;
		if (varType == int) {
			int intVal = text.ToInt();
			EnScript.SetClassVar(GetConfigVarParent(), varPartLast, 0, intVal);
		} else if (varType == float) {
			float floatVal = text.ToFloat();
			EnScript.SetClassVar(GetConfigVarParent(), varPartLast, 0, floatVal);
		} else if (varType == string) {
			SetStringValue(text);
		} else {
			Error("Could not find matching Variable for " + varPath + " and text " + text);
			return false;
		}
		return true;
	}

	protected bool LoadSliderValue(SliderWidget slider) {
		float value = slider.GetCurrent();
		typename varType;
		if (!GetConfigVar(varType))
			return false;
		if (varType == int) {
			SetIntValue((int) value);
		} else {
			SetFloatValue(value);
		}
		return true;
	}

	protected void SetSliderValue(SliderWidget slider) {
		slider.SetCurrent(GetConfigVarAsFloat());
	}

	protected void SetCheckboxValue(CheckBoxWidget chckbx, bool invertBool) {
		bool checked = GetConfigVarAsBool();
		if (invertBool)
			checked = !checked;
		chckbx.SetChecked(checked);
	}

	protected void SetEditBoxValue(EditBoxWidget editbx) {
		string value = GetConfigVarAsString();
		editbx.SetText(value);
	}

	protected void SetComboBoxValue(XComboBoxWidget combo, int selected) {
		if (selected < 0 || selected >= combo.GetNumItems())
			return;
		combo.SetItem(selected, GetConfigVarAsString());
	}

	protected void SetTextListboxValue(TextListboxWidget txtList, int row) {
		if (row < 0 || row >= txtList.GetNumItems())
			return;
		Class userData;
		txtList.GetItemData(row, 0, userData);
		txtList.SetItem(row, GetConfigVarAsString(), userData, 0);
	}

	protected void SetTextValue(TextWidget text) {
		text.SetText(GetConfigVarAsString());
	}

	protected bool LoadVar(Widget w, bool invertBool) {
		if (!w)
			return false;
		CheckBoxWidget chckbx = CheckBoxWidget.Cast(w);
		if (chckbx) {
			SetCheckboxValue(chckbx, invertBool);
			return true;
		}
		EditBoxWidget editbx = EditBoxWidget.Cast(w);
		if (editbx) {
			SetEditBoxValue(editbx);
			return true;
		}
		XComboBoxWidget combo = XComboBoxWidget.Cast(w);
		if (combo) {
			SetComboBoxValue(combo, combo.GetCurrentItem());
			return true;
		}
		TextListboxWidget txtList = TextListboxWidget.Cast(w);
		if (txtList) {
			SetTextListboxValue(txtList, txtList.GetSelectedRow());
			return true;
		}
		SliderWidget slider = SliderWidget.Cast(w);
		if (slider) {
			SetSliderValue(slider);
			return true;
		}
		TextWidget text = TextWidget.Cast(w);
		if (text) {
			SetTextValue(text);
			return true;
		}
		return false;
	}

	protected void SetBoolValue(bool val) {
		if (setterFunction != "") {
			g_Game.GameScript.CallFunctionParams(GetConfigVarParent(), setterFunction, null, new Param1<bool>(val));
		} else {
			int index = varPartLast.IndexOf("()");
			if (index != -1 && index == varPartLast.Length() - 2) {
				string funcName = varPartLast.Substring(0, varPartLast.Length() - 2);
				g_Game.GameScript.CallFunction(GetConfigVarParent(), funcName, null, val);
			} else {
				EnScript.SetClassVar(GetConfigVarParent(), varPartLast, 0, val);
			}
		}
	}

	protected void SetIntValue(int val) {
		if (setterFunction != "") {
			g_Game.GameScript.CallFunctionParams(GetConfigVarParent(), setterFunction, null, new Param1<int>(val));
		} else {
			int index = varPartLast.IndexOf("()");
			if (index != -1 && index == varPartLast.Length() - 2) {
				string funcName = varPartLast.Substring(0, varPartLast.Length() - 2);
				g_Game.GameScript.CallFunctionParams(GetConfigVarParent(), funcName, null, new Param1<int>(val));
			} else {
				EnScript.SetClassVar(GetConfigVarParent(), varPartLast, 0, val);
			}
		}
	}

	protected void SetFloatValue(float val) {
		if (setterFunction != "") {
			g_Game.GameScript.CallFunctionParams(GetConfigVarParent(), setterFunction, null, new Param1<float>(val));
		} else {
			int index = varPartLast.IndexOf("()");
			if (index != -1 && index == varPartLast.Length() - 2) {
				string funcName = varPartLast.Substring(0, varPartLast.Length() - 2);
				g_Game.GameScript.CallFunctionParams(GetConfigVarParent(), funcName, null, new Param1<float>(val));
			} else {
				EnScript.SetClassVar(GetConfigVarParent(), varPartLast, 0, val);
			}
		}
	}

	protected void SetStringValue(string val) {
		if (setterFunction != "") {
			g_Game.GameScript.CallFunctionParams(GetConfigVarParent(), setterFunction, null, new Param1<string>(val));
		} else {
			int index = varPartLast.IndexOf("()");
			if (index != -1 && index == varPartLast.Length() - 2) {
				string funcName = varPartLast.Substring(0, varPartLast.Length() - 2);
				g_Game.GameScript.CallFunctionParams(GetConfigVarParent(), funcName, null, new Param1<string>(val));
			} else {
				EnScript.SetClassVar(GetConfigVarParent(), varPartLast, 0, val);
			}
		}
	}

	protected int GetVariableIndex(typename parent, string name) {
		for (int i = 0; i < parent.GetVariableCount(); i++) {
			if (parent.GetVariableName(i) == name) {
				return i;
			}
		}
		return -1;
	}

	bool CheckReloadTrigger(Widget w) {
		return (w == reloadTrigger && w != null);
	}

	void TriggerOnChange(Widget w) {
		if (w)
			LBAutoComplete.Get().OnChange(w, -1, -1, false);
		if (onChangeTrigger == "")
			return;
		TStringArray triggerParts = new TStringArray();
		onChangeTrigger.Split(".", triggerParts);
		Class found = mainParent;
		int len = triggerParts.Count();
		for (int i = 0; i < len; i++) {
			string varName = triggerParts.Get(i);
			Class temp = null;
			if (varName.IndexOf("()") == varName.Length() - 2) {
				varName = varName.Substring(0, varName.Length() - 2);
				g_Game.GameScript.CallFunction(found, varName, temp, null);
			} else {
				EnScript.GetClassVar(found, varName, 0, temp);
			}
			found = temp;
			if (temp == null)
				return;
		}
	}

	bool OnDropReceived(Widget w, int x, int y, Widget reciever) {
		return false;
	}

	LBWidget_Linker SetReloadTrigger(Widget reloadTrigger_) {
		this.reloadTrigger = reloadTrigger_;
		return this;
	}

	LBWidget_Linker SetChangeTrigger(string calledFunction) {
		onChangeTrigger = calledFunction;
		if (onChangeTrigger.Length() < 2 || onChangeTrigger.Substring(onChangeTrigger.Length() - 2, 2) != "()") {
			onChangeTrigger = onChangeTrigger + "()";
		}
		return this;
	}

	protected static bool SelectEntry(Widget list, int num = -2) {
		XComboBoxWidget combo = XComboBoxWidget.Cast(list);
		if (combo) {
			if (num == -2)
				combo.SetCurrentItem(combo.GetNumItems() - 1);
			else
				combo.SetCurrentItem(num);
			return true;
		}
		TextListboxWidget listbox = TextListboxWidget.Cast(list);
		if (listbox) {
			if (num == -2) {
				listbox.SelectRow(listbox.GetNumItems() - 1);
				listbox.EnsureVisible(listbox.GetNumItems() - 1);
			} else {
				listbox.SelectRow(num);
				listbox.EnsureVisible(num);
			}
			return false;
		}
		return false;
	}

	protected static int GetCurrentSelectedIndex(Widget list) {
		XComboBoxWidget combo = XComboBoxWidget.Cast(list);
		if (combo) {
			return combo.GetCurrentItem();
		}
		TextListboxWidget listbox = TextListboxWidget.Cast(list);
		if (listbox) {
			return listbox.GetSelectedRow();
		}
		return -1;
	}

	bool OnVarChanged(Widget w, int key);
	bool Load();
	void LoadLine();

}