class LBPlayerNameObfuscation {

	static int obfuscationInit = 0;

	static void InitObfuscationRandom() {
		obfuscationInit = TickCount(0) ^ Math.RandomInt(0, 2000000000);
	}

	static void SendToPlayer(PlayerIdentity sender) {
		g_Game.RPCSingleParam(null, LBmaster_Core_RPCs.OBFUSCATION_RANDOM_SYNC, new Param1<int>(obfuscationInit), true, sender);
	}

	static string GetObfuscatedName(string name, int steamidHash) {
		return "" + (((name + steamidHash) + obfuscationInit).Hash() * 54654545);
	}

	static string GetObfuscatedName(string name, string steamid) {
		return GetObfuscatedName(name, steamid.Hash());
	}

	static string GetObfuscatedName(PlayerIdentity identity) {
		return GetObfuscatedName(identity.GetName(), identity.GetPlainId());
	}

	static string GetObfuscatedName(Man player) {
		return GetObfuscatedName(player.GetIdentity());
	}

}