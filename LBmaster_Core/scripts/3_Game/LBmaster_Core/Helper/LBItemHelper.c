class LBItemHelper {

	static string GetConfigPrefix(string type) {
		if (g_Game.ConfigIsExisting("CfgVehicles " + type))
			return "CfgVehicles";
		if (g_Game.ConfigIsExisting("CfgWeapons " + type))
			return "CfgWeapons";
		if (g_Game.ConfigIsExisting("CfgMagazines " + type))
			return "CfgMagazines";
		if (g_Game.ConfigIsExisting("CfgAmmo " + type))
			return "CfgAmmo";
		return "";
	}

	[LBTestManager.StartTestLater(ScriptCaller.Create(GetConfigPrefixTest))]
	static void GetConfigPrefixTest() {
		LBTest<string>.Assert(GetConfigPrefix("Apple"), "CfgVehicles");
		LBTest<string>.Assert(GetConfigPrefix("M4A1"), "CfgWeapons");
		LBTest<string>.Assert(GetConfigPrefix("Mag_STANAG_60Rnd"), "CfgMagazines");
		LBTest<string>.Assert(GetConfigPrefix("Ammo_357"), "CfgMagazines");
		LBTest<string>.Assert(GetConfigPrefix("Bullet_762x39"), "CfgAmmo");
		LBTest<string>.Assert(GetConfigPrefix("Peter"), "");
	}

	static string GetDisplayName(string type) {
		string prefix = GetConfigPrefix(type);
		if (prefix == "")
			return "";
		return g_Game.ConfigGetTextOut(prefix + " " + type + " displayName");
	}

	static bool CanSpawnLocalPreview(string itemname) {
		string prefix = LBItemHelper.GetConfigPrefix(itemname);
		if (!g_Game.ConfigIsExisting(prefix + " " + itemname + " model")) {
			return false;
		}
		string model = g_Game.ConfigGetTextOut(prefix + " " + itemname + " model");
		if (model.IndexOf("\\") != -1)
			model.Replace("\\", "/");
		while (model.IndexOf("/") == 0)
			model = model.Substring(1, model.Length() - 1);
		if (!LBStringTools.EndsWith(model, ".p3d"))
			model = model + ".p3d";
		if (!FileExist(model)) {
			if (g_Game.ConfigGetInt(prefix + " " + itemname + " lbIgnoreNoModel") != 1)
				return false;
		}
		TStringArray chamberable = new TStringArray();
		if (prefix == "CfgWeapons") {
			if (!g_Game.ConfigIsExisting("CfgWeapons " + itemname + " chamberableFrom"))
				return false;
			g_Game.ConfigGetTextArray("CfgWeapons " + itemname + " chamberableFrom", chamberable);
			if (chamberable.Count() == 0 || itemname == "Groza" || itemname == "Trumpet")
				return false;
		}
		if (g_Game.IsKindOf(itemname, "dz_lightai"))
			return false;
		return true;
	}

}