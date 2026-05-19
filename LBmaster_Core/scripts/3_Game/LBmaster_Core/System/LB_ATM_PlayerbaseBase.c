class LB_ATM_PlayerbaseBase {

	private Man man;

	void LB_ATM_PlayerbaseBase(Man pb) {
		this.man = pb;
	}

	Man GetMan() {
		return man;
	}

	// If Enhanced Banking is loaded or any other Banking mod overwrites these five methods
	bool IsBankingAvailable() {
		return false;
	}

	int GetATMMoney() {
		return 0;
	}

	int AddATMMoney(int amount) {
		return 0;
	}

	int AddableATMMoney() {
		return 0;
	}

	int RemoveATMMoney(int amount) {
		return 0;
	}
	// End Mod overwrites

	int GetTotalMoney() {
		int total = GetInventoryMoney();
		if (IsBankingAvailable())
			total += GetATMMoney();
		return total;
	}

	int RemoveTotalMoney(int amount, bool preferPlayerInventory = true) {
		int remaining = amount;
		LBLogger.Verbose("Remain1: " + remaining, "Core");
		if (preferPlayerInventory) {
			remaining -= RemoveInventoryMoney(remaining);
			LBLogger.Verbose("Remain2: " + remaining, "Core");
		}
		if (IsBankingAvailable()) {
			remaining -= RemoveATMMoney(remaining);
			LBLogger.Verbose("Remain3: " + remaining, "Core");
		}
		if (!preferPlayerInventory) {
			remaining -= RemoveInventoryMoney(remaining);
			LBLogger.Verbose("Remain4: " + remaining, "Core");
		}
		LBLogger.Verbose("Remain5: " + remaining, "Core");
		return amount - remaining;
	}

	int GetInventoryMoney() {
		return LBCurrencyConfig.Get.GetInventoryMoney(GetMan());
	}

	protected bool RemoveFromInventory(string itemClassname, int amount) {}

	int RemoveInventoryMoney(int currencyAmount) {}
	int CreateItemInInventory(string itemType, int amount, array<EntityAI> inventoryItems, out int failed) {}
	int AddInventoryMoney(int currencyAmount) {}

	protected void DeleteItemSave(EntityAI item) {
		if (item)
			g_Game.ObjectDelete(item);
	}

	protected static int GetItemMaxQuantity(string itemClassname) {
		string pathQuantityBar = CFG_VEHICLESPATH + " " + itemClassname + " quantityBar";
		string path = CFG_VEHICLESPATH + " " + itemClassname + " varQuantityMax";

		if (g_Game.ConfigIsExisting(pathQuantityBar) && g_Game.ConfigGetInt(path) == 1) {
			return 1;
		}
		if (g_Game.ConfigIsExisting(path)) {
			int quantMax = g_Game.ConfigGetInt(path);
			if (g_Game.ConfigIsExisting(CFG_VEHICLESPATH + " " + itemClassname + " varStackMax"))
				quantMax = g_Game.ConfigGetInt(CFG_VEHICLESPATH + " " + itemClassname + " varStackMax");
			return quantMax;
		}

		return 1;
	}
}