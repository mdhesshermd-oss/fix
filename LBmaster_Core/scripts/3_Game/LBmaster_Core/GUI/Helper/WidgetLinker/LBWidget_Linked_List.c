class LBWidget_Linked_List : LBWidget_Linker {

	private ref array<Widget> allowedFrom;
	private EditBoxWidget searchBar;
	private Widget list;
	private ButtonWidget add, remove;
	private EditBoxWidget input;
	private string funcOnLoadStart, funcOnLoadEntry, funcOnLoadFinish;
	private bool loadFunctionsSet = false;
	private int columnCount = 1;
	private ref array<Widget> searchRefreshTrigger;
	private ref TStringSet inListSet = null;
	private ref TIntSet inListColumns = null;
	private ref TStringArray selectorList = null;
	private int colorInSet, colorNotInSet;
	private bool inListIgnoreCase, addIsItemSelector, addIsListSelector, itemSelectorScopeFilter = true, itemDoSetup = false, inListSplitAtSpace;
	private string lastItemName = "", itemSelectorAttachmentsOf = "";
	private ref LBListManagerBase listMgr;
	private bool playerSelector = false, playerOnlyOnline = false, preventReordering = false, preventDuplication = false, confirmDelete = false;

	private string lastDialogText;
	private bool lastDialogChecked;
	private int lastDialogRow;

	void LBWidget_Linked_List(string varPath_, Widget listOutput, ButtonWidget addButton, ButtonWidget removeButton, EditBoxWidget inputField, Class parent_, LBLinkedVarHandler handler_, bool updateParentOnChange_) {
		this.handler = handler_;
		this.mainParent = parent_;
		this.updateParentOnChange = updateParentOnChange_;
		SetVarPath(varPath_);
		LoadReloadTrigger();
		this.list = listOutput;
		this.add = addButton;
		this.remove = removeButton;
		this.input = inputField;
	}

	LBWidget_Linked_List SetListManager(LBListManagerBase listManager, bool doSetup = false) {
		listMgr = listManager;
		itemDoSetup = doSetup;
		SetListManagerArray();
		return this;
	}

	LBWidget_Linked_List EnableDeleteConfirmation(bool enabled = true) {
		confirmDelete = enabled;
		return this;
	}

	bool CanRemove() {
		return remove != null;
	}

	bool IsPreventReordering() {
		return preventReordering;
	}

	bool IsPreventDuplication() {
		return preventDuplication;
	}

	Widget GetList() {
		return list;
	}

	bool GetCopyString(out string str) {
		if (!listMgr)
			return false;
		int selected = GetCurrentSelectedIndex(list);
		selected = GetEntryIndex(selected);
		if (selected >= 0) {
			str = listMgr.GetCopyString(selected);
			return true;
		}
		return false;
	}

	bool PasteCopiedValue() {
		if (!listMgr)
			return false;
		int selected = GetCurrentSelectedIndex(list);
		selected = GetEntryIndex(selected);
		if (listMgr.PasteCopiedValue(selected)) {
			Load();
			if (SelectEntry(list, selected + 1))
				handler.OnVarChange(list, -1);
			TriggerOnChange(list);
			UpdateParentOnChange();
			return true;
		}
		return false;
	}

	bool DeleteSelectedEntry() {
		RemoveEntry();
		return true;
	}

	bool SetListManagerArray() {
		if (listMgr.IsValid())
			return true;
		Class arr = GetConfigVarAsClass();
		LBLogger.Spam("Setting the Array for " + listMgr + " to " + arr, "Core");
		g_Game.GameScript.CallFunction(listMgr, "SetArr", null, arr);
		return listMgr.IsValid();
	}

	void InvalidateManagerList() {
		if (listMgr)
			listMgr.Invalidate();
	}

	override bool OnMouseEnter(Widget w, int x, int y) {
		if (w == list) {
			Class parent = GetConfigVarParent();
			string comment;
			if (!parent) {
				typename tn;
				if (GetConfigVar(tn) && tn == Class) {
					string type = GetConfigVarAsClass(false).ClassName();
					if (LBStringTools.EndsWith(type, "_"))
						type = type.Substring(0, type.Length() - 1);
					if (!LBConfigInfo.Get().GetClassComment(type, comment))
						return true;
				} else {
					return true;
				}
			} else {
				type = parent.ClassName();
				if (LBStringTools.EndsWith(type, "_"))
					type = type.Substring(0, type.Length() - 1);
				if (!LBConfigInfo.Get().GetVariableComment(type, varPartLast, comment))
					return true;
			}
			LBTooltipManager.Get().DisplayTooltip(list, comment);
			return 2;
		} else if (w == add) {
			if (input) {
				LBTooltipManager.Get().DisplayTooltip(w, "First enter the value and then click on the Add button");
			} else {
				LBTooltipManager.Get().DisplayTooltip(w, "Create a new entry. You can modify it after adding a new entry");
			}
		} else if (w == remove) {
			if (listMgr)
				LBTooltipManager.Get().DisplayTooltip(w, "Remove the selected entry. You can also use the DELETE key to faster delete entries");
			else
				LBTooltipManager.Get().DisplayTooltip(w, "Remove the selected entry");
		} else if (w == input) {
			LBTooltipManager.Get().DisplayTooltip(w, "You first need to enter a value here and then add it using the add button");
		}
		return false;
	}

	LBWidget_Linked_List EnableInListSearch(TStringSet itemList, int colorFound = -16711936, int colorNotFound = -55256, bool ignoreCase = true, bool splitAtSpace = false, TIntSet columns = null) {
		this.inListSet = itemList;
		this.colorInSet = colorFound;
		this.colorNotInSet = colorNotFound;
		this.inListIgnoreCase = ignoreCase;
		this.inListSplitAtSpace = splitAtSpace;
		this.inListColumns = columns;
		if (this.inListIgnoreCase) {
			this.inListSet = new TStringSet();
			foreach (string str : itemList) {
				this.inListSet.Insert(LBStringTools.ToLowerString(str));
			}
		}
		return this;
	}

	LBWidget_Linked_List EnableItemSelector(bool scopeFilter = true, string baseClass = "", bool doSetup = false, string attachmentsOf = "") {
		this.addIsItemSelector = true;
		this.lastItemName = baseClass;
		this.itemSelectorScopeFilter = scopeFilter;
		this.itemSelectorAttachmentsOf = attachmentsOf;
		this.itemDoSetup = doSetup;
		return this;
	}

	LBWidget_Linked_List EnableListSelector(TStringArray selectList) {
		this.addIsListSelector = true;
		this.selectorList = selectList;
		return this;
	}

	LBWidget_Linked_List PreventReordering(bool prevent = true) {
		this.preventReordering = prevent;
		return this;
	}

	LBWidget_Linked_List PreventDuplicating(bool prevent = true) {
		this.preventDuplication = prevent;
		return this;
	}

	LBWidget_Linked_List SetAllowedFromDrop(Widget allowed1, Widget allowed2 = null, Widget allowed3 = null, Widget allowed4 = null, Widget allowed5 = null, Widget allowed6 = null) {
		allowedFrom = new array<Widget>();
		allowedFrom.Insert(allowed1);
		if (allowed2)
			allowedFrom.Insert(allowed2);
		if (allowed3)
			allowedFrom.Insert(allowed3);
		if (allowed4)
			allowedFrom.Insert(allowed4);
		if (allowed5)
			allowedFrom.Insert(allowed5);
		if (allowed6)
			allowedFrom.Insert(allowed6);
		return this;
	}

	LBWidget_Linked_List EnablePlayerSelector(bool onlineOnly = false) {
		playerSelector = true;
		playerOnlyOnline = onlineOnly;
		return this;
	}

	LBListManagerBase GetListManager() {
		return listMgr;
	}

	override bool CheckReloadTrigger(Widget w) {
		if (super.CheckReloadTrigger(w)) {
			InvalidateManagerList();
			return true;
		}
		if (searchRefreshTrigger) {
			foreach (Widget trigg : searchRefreshTrigger) {
				if (w == trigg) {
					InvalidateManagerList();
					return true;
				}
			}
		}
		if (w == searchBar && w != null) {
			int selected = GetCurrentSelectedIndex(list);
			if (selected > 0)
				SelectEntry(list, 0);
			InvalidateManagerList();
			return true;
		}
		return false;
	}

	LBWidget_Linked_List SetLoadFunctions(string funcOnLoadStart_, string funcOnLoadEntry_, string funcOnLoadFinish_ = "") {
		this.funcOnLoadStart = funcOnLoadStart_;
		this.funcOnLoadEntry = funcOnLoadEntry_;
		this.funcOnLoadFinish = funcOnLoadFinish_;
		this.loadFunctionsSet = true;
		return this;
	}

	LBWidget_Linked_List SetSearchBar(EditBoxWidget searchBar_, array<Widget> searchRefreshTrigger_ = null) {
		this.searchBar = searchBar_;
		this.searchRefreshTrigger = searchRefreshTrigger_;
		LBClearInput.AddClearButton(searchBar, ScriptCaller.Create(ClearSearch));
		return this;
	}

	void ClearSearch(EditBoxWidget search) {
		Load();
	}

	LBWidget_Linked_List SetColumnCount(int count) {
		if (count > 0) {
			columnCount = count;
		}
		return this;
	}

	void AddEntryPlayer(PlayerNameSave_ name) {
		if (!name)
			return;
		string steamid = name.GetSteamid();
		AddEntryStr(steamid, true);
	}

	void AddEntryItemname(string value) {
		LBItemSelectorDialog dialog = LBItemSelectorDialog.Cast(LBMenuManager.Get().GetOpenLBMenu());
		if (dialog) {
			lastDialogText = dialog.edit_search.GetText();
			lastDialogChecked = dialog.chckbx_enable_preview.IsChecked();
			lastDialogRow = dialog.list_items.GetSelectedRow();
		}
		AddEntryStr(value, itemDoSetup);
	}

	override bool OnKeyDown(Widget w, int x, int y, int key) {
		if (key == KeyCode.KC_D && GetWidgetUnderCursor() == list && g_Game.IsLeftCtrlDown()) {
			DuplicateEntry();
		}
		return super.OnKeyDown(w, x, y, key);
	}

	void MoveEntryUp() {
		int row = GetCurrentSelectedIndex(list);
		row = GetEntryIndex(row);
		LBLogger.Info("Trying to move Entry up at " + row + " List: " + list, "Core");
		if (preventReordering || !listMgr)
			return;
		int arrayIndex1 = GetEntryIndex(row);
		int arrayIndex2 = GetEntryIndex(row + 1);
		if (listMgr.SwapEntries(arrayIndex1, arrayIndex2)) {
			LoadRow(row + 1, arrayIndex2);
			LoadRow(row, arrayIndex1);
			UpdateParentOnChange();
		}
	}

	void MoveEntryDown() {
		int row = GetCurrentSelectedIndex(list);
		row = GetEntryIndex(row);
		LBLogger.Info("Trying to move Entry down at " + row + " List: " + list, "Core");
		if (preventReordering || !listMgr)
			return;
		int arrayIndex1 = GetEntryIndex(row);
		int arrayIndex2 = GetEntryIndex(row - 1);
		if (listMgr.SwapEntries(arrayIndex1, arrayIndex2)) {
			LoadRow(row - 1, arrayIndex2);
			LoadRow(row, arrayIndex1);
			UpdateParentOnChange();
		}
	}

	int GetEntryIndex(int index) {
		TextListboxWidget listbox = TextListboxWidget.Cast(list);
		if (!listbox || index < 0 || index >= listbox.GetNumItems())
			return index;
		Param1<int> param;
		listbox.GetItemData(index, 0, param);
		return param.param1;
	}

	void DuplicateEntry() {
		int index = GetCurrentSelectedIndex(list);
		int row = GetEntryIndex(index);
		LBLogger.Info("Trying to duplicate Entry at " + row + " List: " + list, "Core");
		if (preventDuplication || !listMgr)
			return;
		row = listMgr.DuplicateEntry(row);
		LBLogger.Debug("Duplicated Entry at " + row, "Core");
		Load();
		if (SelectEntry(list, index + 1))
			handler.OnVarChange(list, -1);
		TriggerOnChange(list);
		UpdateParentOnChange();
	}

	void AddEntryStr(string value, bool doSetup) {
		Class parent = GetConfigVarParent();
		if (listMgr) {
			if (SetListManagerArray())
				listMgr.AddEntry(value, doSetup);
		} else {
			Class arr = GetConfigVarAsClass();
			TStringArray txtArr = TStringArray.Cast(arr);
			if (txtArr) {
				txtArr.Insert(value);
			}
			TStringSet txtSet = TStringSet.Cast(arr);
			if (txtSet) {
				txtSet.Insert(value);
			}
		}
		Load();
		if (!list)
			return;
		if (SelectEntry(list))
			handler.OnVarChange(list, -1);
		TriggerOnChange(list);
		UpdateParentOnChange();
	}

	override bool OnVarChanged(Widget w, int key) {
		if (w == add) {
			if (addIsItemSelector) {
				UIScriptedMenu current = g_Game.GetUIManager().GetMenu();
				LBItemSelectorDialog dialog = LBItemSelectorDialog.Cast(LBMenuManager.Get().OpenMenu("LBItemSelectorDialog", new LBMenuDataItemSelector(ScriptCaller.Create(AddEntryItemname), itemSelectorScopeFilter, lastItemName, "Select an item", "#lb_select", itemSelectorAttachmentsOf), false, current));
				dialog.chckbx_enable_preview.SetChecked(lastDialogChecked);
				dialog.edit_search.SetText(lastDialogText);
				dialog.list_items.SelectRow(lastDialogRow);
				dialog.list_items.EnsureVisible(lastDialogRow);
				dialog.FillItemList();
			} else if (playerSelector) {
				current = g_Game.GetUIManager().GetMenu();
				LBMenuManager.Get().OpenMenu("LBPlayerSelectorDialog", new LBMenuDataPlayerSelector(ScriptCaller.Create(AddEntryPlayer), playerOnlyOnline), false, current);
			} else if (addIsListSelector) {
				current = g_Game.GetUIManager().GetMenu();
				LBMenuManager.Get().OpenMenu("LBListSelectorDialog", new LBMenuDataListSelector(ScriptCaller.Create(AddEntryItemname), selectorList), false, current);
			} else {
				AddEntry();
			}
			return true;
		} else if (w == remove) {
			if (confirmDelete) {
				LBWarningPopup.Get().Show("#delete", "Are you sure you want to delete this entry?", ScriptCaller.Create(RemoveEntry));
			} else {
				RemoveEntry();
			}
			return true;
		} else if (w == list && list) {
			if (key == -1) {
				if (g_Game.LBIsKeyDown(KeyCode.KC_UP) && GetWidgetUnderCursor() == list && g_Game.LBIsLShiftHolding()) {
					MoveEntryUp();
				} else if (g_Game.LBIsKeyDown(KeyCode.KC_DOWN) && GetWidgetUnderCursor() == list && g_Game.LBIsLShiftHolding()) {
					MoveEntryDown();
				}
			}
			TriggerOnChange(list);
		}
		return false;
	}

	private string GetSearchString() {
		if (searchBar == null)
			return "";
		return searchBar.GetText();
	}

	private bool IsSearched(int index, string search, Class parent_) {
		if ((search == "" && searchRefreshTrigger == null) || !listMgr)
			return true;
		return listMgr.IsSearched(index, search, searchRefreshTrigger);
	}

	private void CallOnLoadStart() {
		if (!loadFunctionsSet)
			return;
		g_Game.GameScript.CallFunction(mainParent, funcOnLoadStart, null, null);
	}

	private void CallOnEntryLoaded(int index) {
		if (!loadFunctionsSet)
			return;
		g_Game.GameScript.CallFunctionParams(mainParent, funcOnLoadEntry, null, new Param1<int>(index));
	}

	private void CallOnLoadFinishd() {
		if (!loadFunctionsSet)
			return;
		g_Game.GameScript.CallFunctionParams(mainParent, funcOnLoadFinish, null, null);
	}

	void LoadRow(int row, int index) {
		XComboBoxWidget combo = XComboBoxWidget.Cast(list);
		TextListboxWidget listbox = TextListboxWidget.Cast(list);
		Class parent = GetConfigVarParent();
		if (combo) {
			string str = GetEntry(parent, index, 0);
			combo.SetItem(row, str);
		}
		if (listbox) {
			for (int col = 0; col < columnCount; col++) {
				string str2 = GetEntry(parent, index, col);
				listbox.SetItem(row, str2, new Param1<int>(index), col);
			}
		}
	}

	override bool Load() {
		LBLogger.Spam("Loading List: " + list + " with Path " + varPath, "Core");
		if (!list)
			return true;

		XComboBoxWidget combo = XComboBoxWidget.Cast(list);
		TextListboxWidget listbox = TextListboxWidget.Cast(list);
		Class parent = GetConfigVarParent();
		int lastSelected = GetCurrentSelectedIndex(list);
		int listSize = Count(parent);
		string search = GetSearchString();
		CallOnLoadStart();
		array<int> searchIndices;
		if (searchBar) {
			searchIndices = handler.GetSearchIndices(list);
			searchIndices.Clear();
		}
		if (combo) {
			combo.ClearAll();
			LBLogger.Spam("Cleared all Items and loading new items...", "Core");
			for (int i = 0; i < listSize; i++) {
				if (!IsSearched(i, search, parent))
					continue;
				string str = GetEntry(parent, i, 0);
				combo.AddItem(str);
				CallOnEntryLoaded(i);
				if (searchBar)
					searchIndices.Insert(i);
			}

		}
		if (listbox) {
			listbox.ClearItems();
			LBLogger.Spam("Cleared all Items and loading new items...", "Core");
			for (i = 0; i < listSize; i++) {
				int row = 0;
				if (!IsSearched(i, search, parent))
					continue;
				for (int col = 0; col < columnCount; col++) {
					string str2 = GetEntry(parent, i, col);
					if (col == 0)
						row = listbox.AddItem(str2, new Param1<int>(i), 0);
					else
						listbox.SetItem(row, str2, new Param1<int>(i), col);
					if (inListSet && (!inListColumns || inListColumns.Find(col) != -1)) {
						string findStr = str2 + "";
						if (inListIgnoreCase)
							findStr = LBStringTools.ToLowerString(findStr);
						if (inListSplitAtSpace && findStr.Contains(" "))
							findStr = findStr.Substring(0, findStr.IndexOf(" "));
						if (inListSet.Find(findStr) != -1)
							listbox.SetItemColor(row, col, colorInSet);
						else
							listbox.SetItemColor(row, col, colorNotInSet);
					}
					CallOnEntryLoaded(i);
				}
				if (searchBar)
					searchIndices.Insert(i);
			}
		}
		if (lastSelected < 0) {
			lastSelected = 0;
		}
		if (lastSelected >= listSize) {
			lastSelected = listSize - 1;
		}
		CallOnLoadFinishd();
		if (SelectEntry(list, lastSelected))
			handler.OnVarChange(list, -1);
		TriggerOnChange(list);
		return true;
	}

	private void RemoveEntry() {
		if (!list)
			return;
		int index = GetCurrentSelectedIndex(list);
		int row = GetEntryIndex(index);
		if (row < 0)
			return;
		if (listMgr) {
			if (SetListManagerArray())
				listMgr.RemoveEntry(row);
		} else {
			Class arr = GetConfigVarAsClass();
			TStringArray txtArr = TStringArray.Cast(arr);
			if (txtArr) {
				txtArr.RemoveOrdered(row);
			}
			TStringSet txtSet = TStringSet.Cast(arr);
			if (txtSet) {
				txtSet.Remove(row);
			}
		}
		Load();
		UpdateParentOnChange();
		if (index > 0)
			SelectEntry(list, index - 1);
	}

	override void LoadLine() {
		if (listMgr) {
			int row = GetCurrentSelectedIndex(list);
			int arrayIndex = GetEntryIndex(row);
			LBLogger.Debug("Loading single line: " + row + " at " + arrayIndex + " Count: " + Count(null), "Core");
			LoadRow(row, arrayIndex);
		}
	}

	private void AddEntry() {
		string text = "";
		if (input) {
			text = input.GetText();
			input.SetText("");
		}
		AddEntryStr(text, input != null || itemDoSetup);
	}

	private int Count(Class parent) {
		int count;
		if (listMgr) {
			if (!SetListManagerArray())
				return 0;
			return listMgr.CountEntries();
		} else {
			Class arr = GetConfigVarAsClass();
			TStringArray txtArr = TStringArray.Cast(arr);
			if (txtArr) {
				return txtArr.Count();
			}
			TStringSet txtSet = TStringSet.Cast(arr);
			if (txtSet) {
				return txtSet.Count();
			}
		}
		return count;
	}

	override bool OnDropReceived(Widget w, int x, int y, Widget reciever) {
		if (super.OnDropReceived(w, x, y, reciever))
			return true;
		if (w == list && allowedFrom && allowedFrom.Find(reciever) != -1) {
			LBDragEvent event_;
			reciever.GetScript(event_);
			if (event_) {
				AddEntryItemname(event_.text);
				return true;
			}
		} else if (w == list && reciever == list && !preventReordering) {
			return true;
		}
		return false;
	}

	private string GetEntry(Class parent, int index, int column) {

		string str = "NULL";
		if (listMgr) {
			if (!SetListManagerArray())
				return "";
			return listMgr.GetEntry(index, column);
		} else {
			Class arr = GetConfigVarAsClass();
			TStringArray txtArr = TStringArray.Cast(arr);
			if (txtArr) {
				str = txtArr.Get(index);
			}
			TStringSet txtSet = TStringSet.Cast(arr);
			if (txtSet) {
				str = txtSet.Get(index);
			}
		}
		return str;
	}
}