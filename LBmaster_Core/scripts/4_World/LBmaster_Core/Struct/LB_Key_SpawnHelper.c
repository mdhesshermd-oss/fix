class LB_Key_SpawnHelper {

	static void SetRandomQuantity(ItemBase itembs, float quantity) {
		if (!itembs)
			return;
		float maxQuantity = itembs.GetQuantityMax();
		Magazine mag;
		if (Class.CastTo(mag, itembs)) {
			maxQuantity = mag.GetAmmoMax();
		}
		if (quantity == -2)
			quantity = maxQuantity;

		if (quantity < 0)
			return;
		quantity = Math.Ceil(quantity * maxQuantity);
		LBLogger.Verbose("Setting Random Amount of " + itembs + " to " + quantity, "Core");
		if (mag) {
#ifdef SERVER
			mag.ServerSetAmmoCount(quantity);
#else
			mag.LocalSetAmmoCount(quantity);
#endif
		} else {
			itembs.SetQuantity(quantity, true, false, true);
		}
	}

	static int currentQuickbarIndex = 0;

	static void AddToQuickbar(PlayerBase player, EntityAI item, bool addToHotbar = true) {
#ifdef SERVER
		if (!addToHotbar || !player || !item)
			return;
		if (currentQuickbarIndex >= 10)
			return;
		LBLogger.Verbose("Setting Quickbar Entry of " + LBLogger.FormatPlayerIdentity(player) + " at " + currentQuickbarIndex + " to " + item, "Core");
		player.SetQuickBarEntityShortcut(item, currentQuickbarIndex, false);
		currentQuickbarIndex++;
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Call(ResetQuickbarIndex);
#endif
	}

	static void ResetQuickbarIndex() {
		currentQuickbarIndex = 0;
	}

}