class LBPlayerUtils {

	static ref array<Man> players = new array<Man>();
	static ref array<EntityAI> lastPlayerInventoryItems = null;
	static int lastPlayerInventoryEnumeration = -1;

	[LBTestManager.StartTestLater(ScriptCaller.Create(PlayerUtilsTest))]
	static void PlayerUtilsTest() {
		Man player = Man.Cast(g_Game.CreateObject("SurvivorM_Peter", vector.Zero, true, false, false));
		LBTest<bool>.Assert(IsPlayerAlive(player), true);
		TStringArray find = {"Compass"};
		LBTest<bool>.Assert(HasItemsInInventory(player, find), false);
		player.GetInventory().CreateInInventory("Compass");
		LBTest<bool>.Assert(HasItemsInInventory(player, find), true);
		g_Game.ObjectDelete(player);
	}

	static int GetOnlinePlayerCount() {
#ifdef SERVER
		array<PlayerIdentity> identities = new array<PlayerIdentity>();
		g_Game.GetPlayerIndentities(identities);
		return identities.Count();
#else
		if (ClientData.m_PlayerList && ClientData.m_PlayerList.m_PlayerList) {
			return ClientData.m_PlayerList.m_PlayerList.Count();
		}
		return 1;
#endif
	}

	static TStringArray GetOnlineSteamids() {
		TStringArray arr = new TStringArray();
		if (ClientData.m_PlayerList && ClientData.m_PlayerList.m_PlayerList) {
			foreach (SyncPlayer player : ClientData.m_PlayerList.m_PlayerList) {
				if (player && player.m_Identity)
					arr.Insert(player.m_Identity.GetPlainId());
			}
		}
		return arr;
	}

	static TStringSet GetOnlineSteamidsSet() {
		TStringSet arr = new TStringSet();
		if (ClientData.m_PlayerList && ClientData.m_PlayerList.m_PlayerList) {
			foreach (SyncPlayer player : ClientData.m_PlayerList.m_PlayerList) {
				if (player && player.m_Identity)
					arr.Insert(player.m_Identity.GetPlainId());
			}
		}
		return arr;
	}

	static bool IsPlayerAlive(Man player) {
		return g_Game && player && player.IsAlive() && !player.IsUnconscious();
	}

	static bool IsClientPlayerAlive() {
		return g_Game && IsPlayerAlive(g_Game.GetPlayer());
	}

	static bool HasItemsInInventory(Man player, TStringArray findItems, int maxCacheMS = 1000) {
		if (!player)
			return false;
		array<EntityAI> items = EnumeratePlayerInventory(player, maxCacheMS);
		foreach (EntityAI item : items) {
			if (!item)
				continue;
			string type = item.GetType();
			if (findItems.Find(type) != -1)
				return true;
		}
		return false;
	}

	static bool HasItemsInInventory(TStringArray findItems, int maxCacheMS = 1000) {
#ifdef SERVER
		// Cannot Call this on server!
		return false;
#endif
		return HasItemsInInventory(g_Game.GetPlayer(), findItems, maxCacheMS);
	}

	static array<EntityAI> EnumeratePlayerInventory(Man player, int maxCacheMS) {
#ifndef SERVER
		// Cache only on client and never on server
		if (lastPlayerInventoryItems && g_Game.GetTime() - lastPlayerInventoryEnumeration < maxCacheMS)
			return lastPlayerInventoryItems;
#endif
		array<EntityAI> items = new array<EntityAI>();
		player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
#ifndef SERVER
		lastPlayerInventoryItems = items;
		lastPlayerInventoryEnumeration = g_Game.GetTime();
#endif
		return items;
	}

	static vector GetHeadPosition(Man player) {
		if (!player)
			return vector.Zero;
		int bone = player.GetBoneIndex("Spine2");
		return player.GetBonePositionWS(bone);
	}

	static PlayerIdentity GetPlayerIdentityById(string id) {
		array<PlayerIdentity> identities = new array<PlayerIdentity>();
		g_Game.GetPlayerIndentities(identities);

		if (id.Length() == 17) {
			foreach (PlayerIdentity ident : identities) {
				if (ident && ident.GetPlainId() == id)
					return ident;
			}
		} else {
			foreach (PlayerIdentity ident2 : identities) {
				if (ident2 && ident2.GetId() == id)
					return ident2;
			}
		}
		return null;
	}

	static Man GetPlayerById(string id) {
		PlayerIdentity identity = GetPlayerIdentityById(id);
		if (identity)
			return identity.GetPlayer();
		return null;
	}

	static PlayerIdentity GetPlayerIdentityByName(string name) {
		array<PlayerIdentity> identities = new array<PlayerIdentity>();
		g_Game.GetPlayerIndentities(identities);
		string nameLower = name;
		nameLower.ToLower();
		string nameUnderscore = nameLower;
		nameUnderscore.Replace(" ", "_");
		nameUnderscore.Replace("	", "_");
		foreach (PlayerIdentity ident : identities) {
			if (!ident)
				continue;
			string identName = ident.GetName();
			if (identName == name)
				return ident;
			identName.ToLower();
			if (identName == nameLower)
				return ident;
			identName.Replace(" ", "_");
			identName.Replace("	", "_");
			if (identName == nameUnderscore)
				return ident;
		}
		return null;
	}

}