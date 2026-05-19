class LBCurrencyConfig : LBConfigLoader<LBCurrencyConfig_> {

	[SetPriority(LBConfigPriority.HIGHEST)]
	override void InitVars() {
		InitVarsInternal("Common", "Currencies.json", LBConfigType.CONFIG, true, "core.change.currencies"); // (easy)
	}

}

// Shared config between all LBmaster Mods for all setting up money items

class LBCurrencyConfig_ : LBConfigBase {

	static const int CURRENT_VERSION = 3;

	private string thousandsSeparator = "."; // String printed every thousands value. Can be empty to only display the number. Some countryies use a dot or a comma as the seperator between each thousand value
	private string currencyPrefix = "$"; // Some currencies like $ put the symbol before the number. This can be set here
	private string currencySuffix = ""; // Some currencies like € put the symbol after the number. This can be set here
	private ref array<ref LBCurrencyConfigEntry> currencyValues = new array<ref LBCurrencyConfigEntry>(); // List of items, which can be used as currencies (make sure the Item has a quantity and can be stacked. Otherwise the Item will not be recognized)

	[NonSerialized()]
	private ref map<string, int> currencyValueCache = new map<string, int>();

	override void LoadDefault() {
		currencyValues.Insert(LBCurrencyConfigEntry.Init("MoneyRuble1", 1));
		currencyValues.Insert(LBCurrencyConfigEntry.Init("MoneyRuble5", 5));
		currencyValues.Insert(LBCurrencyConfigEntry.Init("MoneyRuble10", 10));
		currencyValues.Insert(LBCurrencyConfigEntry.Init("MoneyRuble25", 25));
		currencyValues.Insert(LBCurrencyConfigEntry.Init("MoneyRuble50", 50));
		currencyValues.Insert(LBCurrencyConfigEntry.Init("MoneyRuble100", 100));
	}

	override void UpdateVersion() {
		if (version <= 0) {
			currencyPrefix = "$";
		}
	}

	override bool OnLoad() {
		SortCurrencyValues();
		CheckCurrenciesSpawnable();
		currencyValueCache = new map<string, int>();
		bool save = false;
		for (int i = 0; i < currencyValues.Count(); ++i) {
			LBCurrencyConfigEntry entry = currencyValues.Get(i);
			string lower = LBStringTools.ToLowerString(entry.itemclassname);
			if (currencyValueCache.Contains(lower)) {
				currencyValues.Remove(i);
				--i;
				save = true;
				LBLogger.Error("Found duplicate Currency Classname. Removing " + entry.itemclassname + " Value " + entry.value + " from the currencies!", "Core");
				continue;
			}
			currencyValueCache.Insert(lower, entry.value);
		}
		return save;
	}

	override void OnReceivedFromRPC(PlayerIdentity sender) {
		OnLoad();
	}

	void RemoveEntry(int index) {
		currencyValues.RemoveOrdered(index);
	}

	void InsertNewEntry(string text) {
		string lower = LBStringTools.ToLowerString(text);
		if (currencyValueCache.Contains(lower))
			return;
		LBCurrencyConfigEntry entry = LBCurrencyConfigEntry.Init(text, -1);
		currencyValues.Insert(entry);
		currencyValueCache.Insert(lower, entry.value);
	}

	int CountEntries() {
		return currencyValues.Count();
	}

	string GetEntry(int index, int column) {
		return currencyValues.Get(index).itemclassname;
	}

	LBCurrencyConfigEntry GetEntryClass(int index) {
		return currencyValues.Get(index);
	}

	void CheckCurrenciesSpawnable() {
		bool save = false;
		foreach (LBCurrencyConfigEntry currency : currencyValues) {
			string path = "CfgVehicles " + currency.itemclassname;
			TStringArray fullPath = new TStringArray();
			g_Game.ConfigGetFullPath(path, fullPath);
			if (fullPath.Count() <= 0) {
				LBLogger.Fatal("Could not find Currency with the Itemname: \"" + currency.itemclassname + "\" (" + currency.value + "). Check your currencies in the " + LBCurrencyConfig.Loader.GetFilePath(), "Core");
				continue;
			}
			string last = fullPath.Get(0);
			if (last != currency.itemclassname) {
				LBLogger.Info("Updated Currency Itemname from " + currency.itemclassname + " (" + currency.value + ") to " + last, "Core");
				currency.itemclassname = last;
				save = true;
			} else {
				LBLogger.Info("Currency " + currency.itemclassname + " (" + currency.value + ") was configured properly", "Core");
			}
		}
		if (save)
			LBCurrencyConfig.Loader.Save();
	}

	array<ref LBCurrencyConfigEntry> GetDepositableMoney() {
		return currencyValues;
	}

	array<ref LBCurrencyConfigEntry> GetWithdrawableMoney() {
		array<ref LBCurrencyConfigEntry> arr = new array<ref LBCurrencyConfigEntry>();
		foreach (LBCurrencyConfigEntry entry : currencyValues) {
			if (!entry.depositableOnly)
				arr.Insert(entry);
		}
		return arr;
	}

	void ClearValues() {
		this.currencyValues.Clear();
	}

	void AddValue(LBCurrencyConfigEntry entry) {
		currencyValues.Insert(entry);
	}

	void SortCurrencyValues() {
		bool sorted = false;
		while (!sorted) {
			sorted = true;
			for (int i = 0; i < currencyValues.Count() - 1; i++) {
				LBCurrencyConfigEntry first = currencyValues.Get(i);
				LBCurrencyConfigEntry second = currencyValues.Get(i + 1);
				if (first.value > second.value) {
					currencyValues.Set(i, second);
					currencyValues.Set(i + 1, first);
					sorted = false;
				}
			}
		}
	}

	int GetCurrencyValue(string classname) {
		int value = 0;
		string lower = LBStringTools.ToLowerString(classname);
		if (currencyValueCache.Find(lower, value))
			return value;
		return 0;
	}

	string GetFormattedMoneyString(int amount) {
		return currencyPrefix + FormatAmount(amount) + currencySuffix;
	}

	private string FormatAmount(int amount) {
		string str = "" + amount;
		for (int i = 3; i < str.Length(); i += 3 + thousandsSeparator.Length()) {
			if (str.Length() > i) {
				str = str.Substring(0, str.Length() - i) + thousandsSeparator + str.Substring(str.Length() - i, i);
			}
		}
		return str;
	}

	string GetCurrencyPrefix() {
		return currencyPrefix;
	}

	string GetCurrencySuffix() {
		return currencySuffix;
	}

	void SetPrefix(string prefix) {
		currencyPrefix = prefix;
	}

	void SetSuffix(string suffix) {
		currencySuffix = suffix;
	}

	int GetPlayerMoney() {
		return GetInventoryMoney(g_Game.GetPlayer());
	}

	string GetPlayerMoneyFormatted() {
		return GetFormattedMoneyString(GetPlayerMoney());
	}

	int GetInventoryMoney(EntityAI inventoryOwner) {
		return GetInventoryMoney(inventoryOwner.GetInventory());
	}

	int GetInventoryMoney(GameInventory inventory) {
		set<EntityAI> checked = new set<EntityAI>();

		array<EntityAI> itemsArray = new array<EntityAI>;
		inventory.EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);

		int currencyAmount = 0;
		EntityAI owner = inventory.GetInventoryOwner();
		Man player = Man.Cast(owner);
		foreach (EntityAI item : itemsArray) {
			int value = GetItemValue(item);
			if (value == 0)
				continue;
			if (checked.Find(item) != -1) {
#ifdef SERVER
				if (player)
					LBLogger.Error("Player " + LBLogger.FormatPlayerIdentity(player) + " at " + player.GetPosition() + " has duped Money in his inventory: " + item + " (" + item.GetType() + ") Value: " + value, "Core");
				else if (owner)
					LBLogger.Error("Entity " + owner + " at " + owner.GetPosition() + " has duped Money in his inventory: " + item + " (" + item.GetType() + ") Value: " + value, "Core");
#endif
				continue;
			}
			checked.Insert(item);
			currencyAmount += value;
		}

		return currencyAmount;
	}

	int GetItemValue(EntityAI item) {
		if (item)
			return GetItemCount(item) * GetCurrencyValue(item.GetType());
		return 0;
	}

	static int GetItemCount(EntityAI item) {
		if (item) {
			if (item.ConfigGetBool("quantityBar"))
				return 1;
			int max = item.GetQuantityMax();
			if (max > 0)
				return item.GetQuantity();
			return 1;
		}
		return 0;
	}

}
