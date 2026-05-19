class LBInherit {

	private ref map<string, ref LBInheritEntry> allEntries = new map<string, ref LBInheritEntry>();
	private ref map<string, LBInheritEntry> cache = new map<string, LBInheritEntry>();
	private ref map<int, LBInheritEntry> hashCache = new map<int, LBInheritEntry>();
	private ref array<LBInheritEntry> roots = new array<LBInheritEntry>();
	private ref map<string, ref TStringArray> itemsIntoSlot = new map<string, ref TStringArray>();

	private static const ref TStringArray searchClasses = {"CfgVehicles", "CfgWeapons", "CfgAmmo", "CfgMagazines", "CfgNonAIVehicles"};
	static ref LBInherit instance;

	static LBInherit Get() {
		if (instance == null)
			Load();
		return instance;
	}

	private static void Load() {
		instance = new LBInherit();
		instance.Init();
	}

	private void Init() {
		int start = TickCount(0);
		CreateRoots();
		foreach (string category : searchClasses) {
			InitCategory(category);
		}
		hashCache.Clear();
		foreach (string classname, LBInheritEntry entry : allEntries) {
			entry.Init();
			hashCache.Insert(entry.GetHash(), entry);
		}
		LBLogger.Verbose("Loaded " + allEntries.Count() + " Inheritance Class entries in " + (TickCount(start) / 10000) + "ms (Cache Size: " + cache.Count() + ")", "Core");
		cache.Clear();
		if (LBLogger.Verbose())
			PrintEntries();
	}

	bool IsWeaponBroken(string itemname) {
		LBInheritEntry entry = GetEntry(itemname);
		return !entry || entry.weaponIsCrashing;
	}

	TIntArray GetItemnamesHashes(TStringArray itemnames) {
		TIntArray items = new TIntArray();
		foreach (string itemname : itemnames)
			items.Insert(LBStringTools.ToLowerString(itemname).Hash());
		return items;
	}

	TStringArray LoadItemnameesFromHashes(TIntArray hashes) {
		TStringArray itemnames = new TStringArray();
		foreach (int hash : hashes) {
			string itemname = HashToItemname(hash);
			if (itemname != "")
				itemnames.Insert(itemname);
		}
		return itemnames;
	}

	TIntSet GetItemnamesHashes(TStringSet itemnames) {
		TIntSet items = new TIntSet();
		foreach (string itemname : itemnames)
			items.Insert(LBStringTools.ToLowerString(itemname).Hash());
		return items;
	}

	TStringSet LoadItemnameesFromHashes(TIntSet hashes) {
		TStringSet itemnames = new TStringSet();
		foreach (int hash : hashes) {
			string itemname = HashToItemname(hash);
			if (itemname != "")
				itemnames.Insert(itemname);
		}
		return itemnames;
	}

	void CreateRoots() {
		string cat = "configFile";
		int childCount = g_Game.ConfigGetChildrenCount(cat);
		for (int i = 0; i < childCount; i++) {
			string childName;
			if (g_Game.ConfigGetChildName(cat, i, childName))
				CreateRoot(cat, childName);
		}
	}

	string HashToItemname(int hash) {
		LBInheritEntry entry;
		if (hashCache.Find(hash, entry))
			return entry.classname;
		return "";
	}

	void CreateRoot(string category_, string name_) {
		LBInheritEntry entry = new LBInheritEntry(category_, name_);
		entry.SetInitialized();
		AddRoot(entry);
		allEntries.Insert(entry.classname, entry);
	}

	void AddItemInSlot(string itemname, string slotName) {
		TStringArray arr = GetItemsIntoSlot(slotName);
		arr.Insert(itemname);
	}

	TStringArray GetItemsIntoSlot(string slotName) {
		TStringArray arr;
		slotName = LBStringTools.ToLowerString(slotName);
		if (!itemsIntoSlot.Find(slotName, arr)) {
			arr = new TStringArray();
			itemsIntoSlot.Insert(slotName, arr);
		}
		return arr;
	}

	void PrintEntries() {
		foreach (LBInheritEntry root : roots) {
			root.PrintChildren();
		}
	}

	private void InitCategory(string category) {
		int childCount = g_Game.ConfigGetChildrenCount(category);
		for (int i = 0; i < childCount; i++) {
			string childName;
			g_Game.ConfigGetChildName(category, i, childName);
			LBInheritEntry entry = new LBInheritEntry(category, childName);
			allEntries.Insert(entry.classname, entry);
		}
	}

	bool IsChildOf(string child, string parent) {
		LBInheritEntry parentEntry = GetEntry(parent);
		if (parentEntry == null) {
			LBLogger.Error("Could not Check If " + child + " is child of " + parent + ", because " + parent + " does not exist", "Core");
			return false;
		}
		return parentEntry.HasChild(child);
	}

	bool IsChildOf(string child, TStringArray parents) {
		foreach (string parent : parents) {
			if (IsChildOf(child, parent))
				return true;
		}
		return false;
	}

	bool IsChildOf(string child, TStringSet parents) {
		foreach (string parent : parents) {
			if (IsChildOf(child, parent))
				return true;
		}
		return false;
	}

	LBInheritEntry GetEntry(string name) {
		LBInheritEntry entry;
		if (cache.Find(name, entry) && entry)
			return entry;
		entry = allEntries.Get(LBStringTools.ToLowerString(name));
		cache.Insert(name, entry);
		return entry;
	}

	LBInheritEntry GetEntryNoCache(string name) {
		return allEntries.Get(LBStringTools.ToLowerString(name));
	}

	TStringSet GetAllChildren(TStringArray parents, bool lowerCase = false, bool recursive = false, bool includeParentClassname = false, int scope = -1) {
		TStringSet output = new TStringSet();
		if (parents) {
			foreach (string parent : parents) {
				LBInheritEntry entry = GetEntry(parent);
				if (entry)
					output.InsertSet(entry.GetChildren(lowerCase, recursive, includeParentClassname, scope));
				else
					LBLogger.Error("Could not find Inheritance entry of " + parent, "Core");
			}
		} else {
			foreach (LBInheritEntry root : roots) {
				output.InsertSet(root.GetChildren(lowerCase, recursive, includeParentClassname, scope));
			}
		}
		return output;
	}

	void AddRoot(LBInheritEntry entry) {
		roots.Insert(entry);
	}

}
