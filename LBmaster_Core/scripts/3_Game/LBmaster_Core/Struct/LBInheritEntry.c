class LBInheritEntry {

	private bool initialized = false;
	LBInheritEntry parent;
	string category;
	string classname, caseSensitiveClassname;
	ref array<LBInheritEntry> directChildren = new array<LBInheritEntry>();
	ref TStringArray inventorySlots = new TStringArray();
	int scope = -1;
	bool weaponIsCrashing = false;

	void LBInheritEntry(string category_, string name_) {
		this.category = category_;
		this.caseSensitiveClassname = name_;
		this.classname = LBStringTools.ToLowerString(name_);
	}

	void PrintChildren(string prefix = "") {
		LBLogger.Verbose(prefix + caseSensitiveClassname, "Core");
		foreach (LBInheritEntry child : directChildren) {
			child.PrintChildren(prefix + "\t");
		}
	}

	void Init() {
		if (initialized) {
			return;
		}
		SetInitialized();
		string parentName;
		g_Game.ConfigGetTextArray(category + " " + caseSensitiveClassname + " inventorySlot", inventorySlots);
		foreach (string slot : inventorySlots) {
			slot = LBStringTools.ToLowerString(slot);
			LBInherit.instance.AddItemInSlot(caseSensitiveClassname, slot);
		}
		scope = g_Game.ConfigGetInt(category + " " + caseSensitiveClassname + " scope");
		if (!g_Game.ConfigGetBaseName(category + " " + caseSensitiveClassname, parentName)) {
			LBInherit.instance.AddRoot(this);
			return;
		}
		parent = LBInherit.instance.GetEntry(parentName);
		if (!parent) {
			Error("Could not find parent: " + parentName + " of " + caseSensitiveClassname + " in category " + category);
		} else {
			parent.directChildren.Insert(this);
		}
		if (g_Game.ConfigIsExisting(category + " " + caseSensitiveClassname + " chamberableFrom")) {
			TStringArray chamberable = new TStringArray();
			g_Game.ConfigGetTextArray(category + " " + caseSensitiveClassname + " chamberableFrom", chamberable);
			foreach (string chamber : chamberable) {
				if (!g_Game.ConfigIsExisting("CfgMagazines " + chamber)) {
					weaponIsCrashing = true;
					if (caseSensitiveClassname != "FNX45_arrow" && caseSensitiveClassname != "PVCBow" && caseSensitiveClassname != "QuickieBow" && caseSensitiveClassname != "RecurveBow")
						LBLogger.Fatal("Broken Weapon Config found! " + caseSensitiveClassname + " has ammo in the chamberableFrom list in the config.cpp, which does not exist. Spawning this weapon with a mag will crash the server!", "Core");
					break;
				}
			}
		}
	}

	void SetInitialized() {
		initialized = true;
	}

	TStringSet GetChildren(bool lowerCase = false, bool recursive = false, bool includeParentClassname = false, int scope_ = -1) {
		TStringSet output = new TStringSet();
		if (includeParentClassname)
			AddToList(output, lowerCase, scope_);
		GetChildrenInternal(lowerCase, recursive, scope_, output);
		return output;
	}

	bool IsInherited(string other) {
		LBInheritEntry entry = LBInherit.instance.GetEntry(other);
		if (!entry) {
			Error("Could not find inheritance Entry for Classname " + other);
			return false;
		}
		return entry.HasChildInternal(classname);
	}

	bool HasChild(string name) {
		return HasChildInternal(LBStringTools.ToLowerString(name));
	}

	int GetHash() {
		return classname.Hash();
	}

	private bool HasChildInternal(string name) {
		if (classname == name)
			return true;
		foreach (LBInheritEntry entry : directChildren) {
			if (entry.HasChildInternal(name))
				return true;
		}
		return false;
	}

	private void GetChildrenInternal(bool lowerCase, bool recursive, int scope_, TStringSet output) {
		if (recursive) {
			foreach (LBInheritEntry entry : directChildren) {
				entry.AddToList(output, lowerCase, scope_);
				entry.GetChildrenInternal(lowerCase, true, scope_, output);
			}
		} else {
			foreach (LBInheritEntry entry2 : directChildren) {
				entry2.AddToList(output, lowerCase, scope_);
			}
		}
	}

	void AddToList(TStringSet output, bool lowerCase, int scope_) {
		if (scope_ != -1 && scope_ != scope)
			return;
		if (lowerCase)
			output.Insert(classname);
		else
			output.Insert(caseSensitiveClassname);
	}


}