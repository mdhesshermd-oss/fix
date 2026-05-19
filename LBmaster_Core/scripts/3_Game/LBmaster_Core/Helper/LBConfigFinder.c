class LBConfigFinder {

	static TStringArray FindAllImageSetPaths() {
		TStringArray paths = new TStringArray();
		string filename;
		FileAttr attr;
		FindFileHandle handle = FindFile("gui/imagesets/*", filename, attr, FindFileFlags.ALL);
		if (handle != 0) {
			if (LBStringTools.EndsWith(filename, ".imageset"))
				paths.Insert("gui/imagesets/" + filename);
			while (FindNextFile(handle, filename, attr)) {
				if (LBStringTools.EndsWith(filename, ".imageset"))
					paths.Insert("gui/imagesets/" + filename);
			}
			CloseFindFile(handle);
		}
		int count = g_Game.ConfigGetChildrenCount("CfgMods");
		for (int i = 0; i < count; ++i) {
			string name;
			g_Game.ConfigGetChildName("CfgMods", i, name);
			if (!g_Game.ConfigIsExisting("CfgMods " + name + " defs imageSets files"))
				continue;
			TStringArray modImageSets = new TStringArray();
			g_Game.ConfigGetTextArray("CfgMods " + name + " defs imageSets files", modImageSets);
			paths.InsertAll(modImageSets);
		}
		LBLogger.Debug("Found " + paths.Count() + " Image set paths", "Core");
		return paths;
	}

	static TStringArray LoadAllImageSetsAsList() {
		map<string, ref TStringArray> sets = LoadAllImageSets();
		TStringArray output = new TStringArray();
		foreach (string set_, TStringArray images : sets) {
			foreach (string image : images) {
				output.Insert("set:" + set_ + " image:" + image);
			}
		}
		return output;
	}

	static map<string, ref TStringArray> LoadAllImageSets() {
		TStringArray paths = FindAllImageSetPaths();
		map<string, ref TStringArray> output = new map<string, ref TStringArray>();
		foreach (string path : paths) {
			bool nextIsImageSet = false, nextIsImage = false;
			FileHandle handle = OpenFile(path, FileMode.READ);
			if (handle == 0) {
				LBLogger.Debug("Could not open Imageset " + path + " to read", "Core");
				continue;
			}
			string line;
			TStringArray currentImageSet;
			while (FGets(handle, line) > 0) {
				if (line.IndexOf("ImageSetClass") != -1) {
					nextIsImageSet = true;
					continue;
				} else if (line.IndexOf("ImageSetDefClass") != -1) {
					nextIsImage = true;
					continue;
				}
				if (line.Trim().IndexOf("Name") == 0) {
					int start = line.IndexOf("\"");
					int end = line.LastIndexOf("\"");
					if (start == -1 || end == -1)
						continue;
					string name = LBStringTools.SubstringFromTo(line, start + 1, end);
					if (nextIsImageSet) {
						nextIsImageSet = false;
						if (!output.Find(name, currentImageSet)) {
							currentImageSet = new TStringArray();
							output.Insert(name, currentImageSet);
						}
					} else if (nextIsImage) {
						nextIsImage = false;
						if (!currentImageSet) {
							LBLogger.Debug("Could not add Image " + name + " to imageset as it's before any imageset!", "Core");
							continue;
						}
						currentImageSet.Insert(name);
					}
				}
			}
		}
		return output;
	}

}