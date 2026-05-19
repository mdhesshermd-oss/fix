[DayZGame.RegisterLBAdminPage("LBUpdateCheckerMenu", "LBmaster_Core/logo/logo.paa", "#lb_admin_updates", "")]
class LBUpdateCheckerMenu : LBAdmin_Menu_Page {

	ref TStringArray messagesList = new TStringArray();

	RichTextWidget messages;

	override void OnShow() {
		super.OnShow();
		LBLogger.Info("Sending Update Check Request to server...", "Core");
		g_Game.RPCSingleParam(null, LBmaster_Core_RPCs.UPDATE_CHECK_INFO, new Param1<bool>(true), true);
	}

	override array<string> GetEditedConfigs() {}

	override void OnRPC(Object target, int rpc_type, ParamsReadContext ctx) {
		if (rpc_type == LBmaster_Core_RPCs.UPDATE_CHECK_INFO) {
			ctx.Read(messagesList);
			LBLogger.Verbose("Received update messages: " + messagesList, "Core");
			LoadMessages();
		}
	}

	void LoadMessages() {
		string all = "";
		foreach (string message : messagesList)
			all = all + message + "\n";
		messages.SetText(all);
	}
}