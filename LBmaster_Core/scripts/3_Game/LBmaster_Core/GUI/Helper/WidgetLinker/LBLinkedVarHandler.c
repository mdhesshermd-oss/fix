class LBLinkedVarHandler {

	private ref array<ref LBWidget_Linker> linkedObjs = new array<ref LBWidget_Linker>();
	private ref map<string, ref array<int>> searchIdices = new map<string, ref array<int>>();
	private Class parent;

	void LBLinkedVarHandler(Class parentClass) {
		this.parent = parentClass;
	}

	void LoadLinkedVars() {
		foreach (LBWidget_Linker linked : linkedObjs) {
			linked.Load();
		}
	}

	void TestConfigInfo() {
		foreach (LBWidget_Linker linked : linkedObjs) {
			LBWidget_Linked_List list = LBWidget_Linked_List.Cast(linked);
			if (list && list.OnMouseEnter(list.GetList(), 0, 0) != 2)
				LBLogger.Debug("list " + linked.GetVarPath() + " does not have a comment", "Core");
			LBWidget_Linked_Var var = LBWidget_Linked_Var.Cast(linked);
			if (var && var.OnMouseEnter(var.GetLinkedTo(), 0, 0) != 2)
				LBLogger.Debug("variable " + linked.GetVarPath() + " does not have a comment", "Core");
		}
	}

	bool CopySelectedListEntry(out string str) {
		Widget cursor = GetWidgetUnderCursor();
		Widget focus = GetFocus();
		foreach (LBWidget_Linker linked : linkedObjs) {
			LBWidget_Linked_List list = LBWidget_Linked_List.Cast(linked);
			if (list && (list.GetList() == cursor || list.GetList() == focus) && list.GetList() != null && list.GetCopyString(str)) {
				g_Game.CopyToClipboard(str);
				LBCopiedPopup.ShowAtCursor();
				return true;
			}

		}
		return false;
	}

	bool PasteCopiedValue() {
		Widget cursor = GetWidgetUnderCursor();
		Widget focus = GetFocus();
		foreach (LBWidget_Linker linked : linkedObjs) {
			LBWidget_Linked_List list = LBWidget_Linked_List.Cast(linked);
			if (list && !list.IsPreventDuplication() && (list.GetList() == cursor || list.GetList() == focus) && list.GetList() != null && list.PasteCopiedValue()) {
				LBCopiedPopup.ShowAtCursor("Pasted!");
				return true;
			}

		}
		return false;
	}

	void DeleteSelectedListEntry() {
		Widget cursor = GetWidgetUnderCursor();
		Widget focus = GetFocus();
		foreach (LBWidget_Linker linked : linkedObjs) {
			LBWidget_Linked_List list = LBWidget_Linked_List.Cast(linked);
			if (list && list.CanRemove() && (list.GetList() == cursor || list.GetList() == focus) && list.GetList() != null && list.DeleteSelectedEntry()) {
				return;
			}

		}
	}

	LBWidget_Linked_Var RegisterLinkedVar(string varPath, Widget linkedTo, bool updateParentOnChange = false) {
		LBWidget_Linked_Var var = new LBWidget_Linked_Var(varPath, linkedTo, parent, this, updateParentOnChange);
		linkedObjs.Insert(var);
		return var;
	}

	LBWidget_Linked_Binary RegisterLinkedBinaryVar(string varPath, Widget list, CheckBoxWidget change) {
		LBWidget_Linked_Binary var = new LBWidget_Linked_Binary(varPath, list, change, parent, this);
		linkedObjs.Insert(var);
		return var;
	}

	LBWidget_Linked_List RegisterLinkedList(string varPath, Widget listOutput, ButtonWidget addButton = null, ButtonWidget removeButton = null, EditBoxWidget inputField = null, bool updateParentOnChange = false) {
		LBWidget_Linked_List list = new LBWidget_Linked_List(varPath, listOutput, addButton, removeButton, inputField, parent, this, updateParentOnChange);
		linkedObjs.Insert(list);
		return list;
	}

	bool OnVarChange(Widget w, int key = -1) {
		bool ok = false;
		if (!w)
			return false;
		foreach (LBWidget_Linker linked : linkedObjs) {
			if (linked.CheckReloadTrigger(w)) {
				linked.Load();
				ok = true;
			} else if (linked.OnVarChanged(w, key)) {
				ok = true;
			}
		}
		LBAutoComplete.Get().OnChange(w, -1, -1, false);
		return ok;
	}

	bool OnKeyDown(Widget w, int x, int y, int key) {
		LBAutoComplete.Get().OnKeyDown(w, x, y, key);
		foreach (LBWidget_Linker linked : linkedObjs) {
			if (linked.OnKeyDown(w, x, y, key))
				return true;
		}
		return false;
	}

	bool OnKeyUp(Widget w, int x, int y, int key) {
		LBAutoComplete.Get().OnKeyUp(w, x, y, key);
		foreach (LBWidget_Linker linked : linkedObjs) {
			if (linked.OnKeyUp(w, x, y, key))
				return true;
		}
		return false;
	}

	bool OnDropReceived(Widget w, int x, int y, Widget reciever) {
		foreach (LBWidget_Linker linked : linkedObjs) {
			if (linked.OnDropReceived(w, x, y, reciever))
				return true;
		}
		return false;
	}

	bool OnMouseEnter(Widget w, int x, int y) {
		foreach (LBWidget_Linker linked : linkedObjs) {
			if (linked.OnMouseEnter(w, x, y)) {
				return false;
			}
		}
		return false;
	}

	void TriggerLoad(string varPath) {
		foreach (LBWidget_Linker linked : linkedObjs) {
			if (linked.IsVarPath(varPath)) {
				linked.Load();
			}
		}
	}

	void TriggerLoadLine(string varPath) {
		foreach (LBWidget_Linker linked : linkedObjs) {
			if (linked.IsVarPath(varPath)) {
				linked.LoadLine();
			}
		}
	}

	Widget FindWidget(string name) {
		Widget widget;
		EnScript.GetClassVar(parent, name, 0, widget);
		return widget;
	}

	LBListManagerBase GetListManager(string varPath) {
		foreach (LBWidget_Linker linked : linkedObjs) {
			if (linked.IsVarPath(varPath)) {
				LBWidget_Linked_List list = LBWidget_Linked_List.Cast(linked);
				LBLogger.Verbose("Found Linker with path " + varPath + " " + linked, "Core");
				if (list)
					LBLogger.Verbose("Manager for " + linked + " is " + list.GetListManager(), "Core");
				if (list && list.GetListManager()) {
					list.SetListManagerArray();
					return list.GetListManager();
				}
			}
		}
		return null;
	}

	array<int> GetSearchIndices(Widget widget_, bool addNew = true) {
		if (!widget_)
			return null;
		string name = widget_.GetName();
		array<int> found;
		if (searchIdices.Find(name, found) || !addNew)
			return found;
		found = new array<int>();
		searchIdices.Insert(name, found);
		return found;
	}

	int SearchedIndexToListIndex(Widget widget_, int searchIndex) {
		array<int> list = GetSearchIndices(widget_, false);
		if (!list)
			return searchIndex;
		if (list.Count() > searchIndex)
			return list[searchIndex];
		return -1;
	}

	int ListIndexToSearchedIndex(Widget widget_, int listIndex) {
		array<int> list = GetSearchIndices(widget_, false);
		if (!list)
			return listIndex;
		return list.Find(listIndex);
	}

}