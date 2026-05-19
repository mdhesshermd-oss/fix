class LBListDragEntry : ScriptedWidgetEventHandler {

	TextListboxWidget root;
	TextWidget dragWidget;
	string text;
	int row, rowSelected;

	void OnWidgetScriptInit(Widget w) {
		root = TextListboxWidget.Cast(w);
		root.SetHandler(this);
	}

	override bool OnDrag(Widget w, int x, int y) {
		if (w == root) {
			CancelWidgetDragging();
			row = root.GetSelectedRow();
			if (row < 0 || row >= root.GetNumItems())
				return true;
			root.SelectRow(rowSelected);
			root.GetItemText(row, 0, text);
			dragWidget = TextWidget.Cast(LBLayoutManager.Get().CreateLayout("DragListEntry"));
			dragWidget.SetText(text);
			dragWidget.SetScreenPos(x, y);
			g_Game.GetUpdateQueue(CALL_CATEGORY_SYSTEM).Insert(UpdateDragWidget);
			return true;
		}
		return super.OnDrag(w, x, y);
	}

	override bool OnMouseButtonDown(Widget w, int x, int y, int button) {
		if (w == root) {
			rowSelected = root.GetSelectedRow();
		}
		return super.OnMouseButtonDown(w, x, y, button);
	}

	void UpdateDragWidget(float timeslice) {
		int mouseX, mouseY;
		GetMousePos(mouseX, mouseY);
		if (root.GetSelectedRow() != rowSelected)
			root.SelectRow(rowSelected);
		dragWidget.SetScreenPos(mouseX, mouseY);
		if (!(GetMouseState(MouseState.LEFT) & MB_PRESSED_MASK)) {
			DropWidget();
		}
	}

	void DropWidget() {
		LBDragEvent event_;
		dragWidget.GetScript(event_);
		if (event_) {
			event_.text = text;
			event_.row = row;
			event_.origin = root;
		}
		dragWidget.Show(false);
		Widget under = GetWidgetUnderCursor();
		if (under) {
			LBAdmin_Menu adminMenu = LBAdmin_Menu.Cast(LBMenuManager.Get().GetOpenLBMenu());
			if (adminMenu && adminMenu.activePage) {
				int mouseX, mouseY;
				GetMousePos(mouseX, mouseY);
				adminMenu.activePage.OnDropReceived(under, mouseX, mouseY, dragWidget);
			}
		}
		g_Game.GetUpdateQueue(CALL_CATEGORY_SYSTEM).Remove(UpdateDragWidget);
	}

}