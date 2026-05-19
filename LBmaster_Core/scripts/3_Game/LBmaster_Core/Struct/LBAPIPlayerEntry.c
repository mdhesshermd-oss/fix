class LBAPIPlayerEntry {

	string steamid;
	string name;
	string status;
	vector position = vector.Zero;
	float health;
	float blood;
	string vehicle;
	string itemInHands;

	void LBAPIPlayerEntry(PlayerIdentity ident) {
		steamid = ident.GetPlainId();
		name = ident.GetName();
		Man player = ident.GetPlayer();
		if (!player || !player.IsAlive()) {
			status = "DEAD";
		} else if (player.IsUnconscious()) {
			status = "UNCONSCIOUS";
		} else {
			status = "ALIVE";
		}
		if (player) {
			position = player.GetPosition();
			health = player.GetHealth();
			blood = player.GetHealth("", "Blood");
			Transport transport = Transport.Cast(player.GetParent());
			if (transport) {
				vehicle = transport.GetType();
			}
			EntityAI hands = player.GetHumanInventory().GetEntityInHands();
			if (hands)
				itemInHands = hands.GetType();
		}
	}

}