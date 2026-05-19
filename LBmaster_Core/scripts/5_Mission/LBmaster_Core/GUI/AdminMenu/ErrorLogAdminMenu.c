[DayZGame.RegisterLBAdminPage("ErrorLogAdminMenu", "LBmaster_Core/logo/logo.paa", "Server Error Logs", "core.view.errorlogs")]
class ErrorLogAdminMenu : LBAdmin_Menu_Page {

	TextWidget txt_info;
	TextListboxWidget list_logs;

	int currentIndex = -1;
	int count = -1, receivedCount = 0;
	bool waiting = false;

	override void OnShow() {
		super.OnShow();
		currentIndex = -1;
		count = -1;
		list_logs.ClearItems();
		txt_info.SetText(" Loading Initial Log lines...");
		RequestCount();
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(RequestCount, 1000, true);
	}

	override TStringArray GetEditedConfigs() {}

	override void OnRPC(Object target, int rpc_type, ParamsReadContext ctx) {
		if (rpc_type == LBmaster_Core_RPCs.LOGS_COUNT) {
			int oldCount = count;
			ctx.Read(count);
			LBLogger.Verbose("Received info for " + count + " error log lines. OldCount: " + oldCount + " current: " + currentIndex, "Core");
			if (oldCount == -1) {
				int start = count - 50;
				if (RequestLines(start, count))
					currentIndex = start;
			} else {
				if (count > oldCount && list_logs.GetSelectedRow() <= 0) {
					start = currentIndex + receivedCount;
					RequestLines(start, start + 20);
				}
			}
			txt_info.SetText(" Selected " + (list_logs.GetNumItems() - list_logs.GetSelectedRow() + currentIndex) + " of " + count + " Log lines. Loaded: " + receivedCount);
		} else if (rpc_type == LBmaster_Core_RPCs.LOGS_GET) {
			TStringArray lines;
			int startIndex;
			if (!ctx.Read(startIndex) || !ctx.Read(lines))
				return;
			LBLogger.Debug("Loaded " + lines.Count() + " new lines starting at " + startIndex + " current: " + currentIndex, "Core");
			if (startIndex > currentIndex) {
				for (int i = 0; i < lines.Count(); ++i) {
					list_logs.AddItem(lines.Get(i), null, 0, 0);
				}
			} else {
				currentIndex = startIndex;
				for (i = lines.Count() - 1; i >= 0; --i) {
					list_logs.AddItem(lines.Get(i), null, 0);
				}
			}
			receivedCount += lines.Count();
			waiting = false;
			txt_info.SetText(" Selected " + (list_logs.GetNumItems() - list_logs.GetSelectedRow() + currentIndex) + " of " + count + " Log lines. Loaded: " + receivedCount);
		}
	}

	void RequestCount() {
		g_Game.RPCSingleParam(null, LBmaster_Core_RPCs.LOGS_COUNT, new Param1<bool>(true), true);
	}

	override bool OnItemSelected(Widget w, int x, int y, int row, int column,	int oldRow, int oldColumn) {
		if (super.OnItemSelected(w, x, y, row, column, oldRow, oldColumn))
			return true;
		txt_info.SetText(" Selected " + (list_logs.GetNumItems() - list_logs.GetSelectedRow() + currentIndex) + " of " + count + " Log lines. Loaded: " + receivedCount);
		if (count > 0 && row + 1 == receivedCount) {
			int start = currentIndex - 50;
			int end = currentIndex;
			RequestLines(start, end);
		} else if (row == 0) {
			start = currentIndex + receivedCount;
			RequestLines(start, start + 20);
		}
		return false;
	}

	bool RequestLines(inout int start, int end) {
		if (waiting)
			return false;
		waiting = true;
		if (start < 0)
			start = 0;
		if (end >= count)
			end = count;

		LBLogger.Debug("Requesting Log lines from " + start + " to " + end, "Core");
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(start);
		rpc.Write(end);
		rpc.Send(null, LBmaster_Core_RPCs.LOGS_GET, true);
		return true;
	}

}