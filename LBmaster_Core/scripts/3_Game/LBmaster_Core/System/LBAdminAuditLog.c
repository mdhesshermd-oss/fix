class LBAdminAuditLog { // Implemented by the LBmaster Admin Tool (does not do anything without it)

	static ref LBAdminAuditLog Get;
	bool enabled = false;

	static void RemoteLog(string action, string message, array<Object> targets = null) {
		g_Game.RPCSingleParam(null, LBmaster_Core_RPCs.AUDIT_LOG, new Param3<string, string, array<Object>>(action, message, targets), true);
	}

	void LogAction(string steamid, string action, string message, vector position = vector.Zero, array<Object> targets = null);
	void LogAction(PlayerIdentity identity, string action, string message, array<Object> targets = null) {
		if (!enabled)
			return;
		if (identity) {
			Man player = identity.GetPlayer();
			if (player)
				LogAction(identity.GetPlainId(), action, message, player.GetPosition(), targets);
			else
				LogAction(identity.GetPlainId(), action, message, vector.Zero, targets);
		}
	}
	void LogAction(Man admin, string action, string message, array<Object> targets = null) {
		if (!enabled)
			return;
		if (admin && admin.GetIdentity()) {
			LogAction(admin.GetIdentity().GetPlainId(), action, message, admin.GetPosition(), targets);
		}
	}
}