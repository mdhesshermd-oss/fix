class LBPlayer_ {

	string steamid;
	Man player;
	PlayerIdentity identity;

	string GetSteamId() {
		return steamid;
	}

	Man GetPlayer() {
		if (player)
			return player;
		if (identity) {
			player = identity.GetPlayer();
			return player;
		}
		player = LBPlayerUtils.GetPlayerById(steamid);
		return player;
	}

	PlayerIdentity GetIdentity() {
		if (identity)
			return identity;
		return LBPlayerUtils.GetPlayerIdentityById(steamid);
	}

	bool IsValid() {
		return steamid.Length() == 17 || player || identity;
	}

	bool IsOnline() {
		return GetPlayer() != null;
	}
}

LBPlayer_ LBPlayer(string steamid) {
	LBPlayer_ inst = new LBPlayer_();
	inst.steamid = steamid;
	return inst;
}
LBPlayer_ LBPlayer(PlayerIdentity identity) {
	LBPlayer_ inst = new LBPlayer_();
	inst.identity = identity;
	if (inst.identity) {
		inst.steamid = identity.GetPlainId();
	}
	return inst;
}
LBPlayer_ LBPlayer(Man player) {
	LBPlayer_ inst = new LBPlayer_();
	inst.player = player;
	inst.identity = player.GetIdentity();
	if (inst.identity) {
		inst.steamid = inst.identity.GetPlainId();
	}
	return inst;
}
