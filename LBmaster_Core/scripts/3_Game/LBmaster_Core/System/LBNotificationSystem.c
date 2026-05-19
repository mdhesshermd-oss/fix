class LBNotificationSystem {

	static void LBSendNotificationTranslated(PlayerIdentity target, float duration, string title, LBTranslatedString message, string icon) {
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(duration);
		rpc.Write(title);
		rpc.Write(message);
		rpc.Write(icon);
		rpc.Send(null, LBmaster_Core_RPCs.NOTIFICATION_TRANSLATED, true, target);
	}

	static void LBSendNotificationTranslatedMan(Man player, float duration, string title, LBTranslatedString message, string icon) {
		if (!player || !player.GetIdentity())
			return;
		LBSendNotificationTranslated(player.GetIdentity(), duration, title, message, icon);
	}

	static void LBSendChatTranslated(PlayerIdentity target, LBTranslatedString message, string style = "colorAction") {
		ScriptRPC rpc = new ScriptRPC();
		rpc.Write(message);
		rpc.Write(style);
		rpc.Send(null, LBmaster_Core_RPCs.CHAT_TRANSLATED, true, target);
	}

	static void LBSendChatTranslatedMan(Man player, LBTranslatedString message, string style = "colorAction") {
		if (!player || !player.GetIdentity())
			return;
		LBSendChatTranslated(player.GetIdentity(), message, style);
	}

}