class LBFileList {

	static ref LBFileList g_LBFileList = null;

	static LBFileList Get(ScriptCaller finishedCallback) {
		if (!g_LBFileList) {
			g_LBFileList = new LBFileList(finishedCallback);
		} else if (finishedCallback) {
			finishedCallback.Invoke();
		}
		return g_LBFileList;
	}

	private ref LBFileTree fileTree = new LBFileTree("", "");
	private ref TStringSet failedPBOs = new TStringSet();
	private ref ScriptCaller callback;

	void LBFileList(ScriptCaller finishedCallback) {
		callback = finishedCallback;
		thread InitRoot();
	}

	LBFileTree GetRoot() {
		return fileTree;
	}

	array<ref LBFileTree> GetRootFiles() {
		return fileTree.children;
	}

	private void InitRoot() {
		Sleep(100);
		LoadPBOs("Addons");
		Sleep(1);
		LoadPBOs("dta");
		Sleep(1);
		string mods = "";
		GetCLIParam("mod", mods);
		TStringArray modList = new TStringArray();
		mods.Split(";", modList);
		foreach (string mod : modList) {
			LoadPBOs(mod + "/Addons");
			Sleep(1);
		}
		TStringSet prefixes = FindPotentialPrefixes();
		foreach (string prefix : prefixes) {
			LoadByPrefix(prefix);
			Sleep(1);
		}
		if (callback)
			callback.Invoke();
	}

	bool LoadByPrefix(string prefix) {
		if (prefix.LastIndexOf("/") != prefix.Length() - 1)
			prefix = prefix + "/";
		if (IsPrefixIgnored(prefix))
			return false;
		LBLogger.Verbose("Checking potential prefix: " + prefix, "Core");
		string filename;
		FileAttr fileAttr;
		FindFileHandle find = FindFile(prefix + "*", filename, fileAttr, FindFileFlags.ALL);
		if (find == 0)
			return false;
		bool found = false;
		LBLogger.Verbose("Successfully finding files in " + prefix + ". First: " + filename, "Core");
		if (filename != "") {
			fileTree.InsertFilePath(prefix + filename);
			found = true;
		}
		while (FindNextFile(find, filename, fileAttr)) {
			found = true;
			if (filename != "")
				fileTree.InsertFilePath(prefix + filename);
		}
		CloseFindFile(find);
		return found;
	}

	bool IsPrefixIgnored(string prefix) {
		if (prefix.LastIndexOf("/") == prefix.Length() - 1)
			prefix = prefix.Substring(0, prefix.Length() - 1);
		prefix.ToLower();
		if (FileExist(prefix + "/*.*") || FileExist(prefix + "/\t") || FileExist(prefix + "/\t\t")) {
			return true;
		}
		if (prefix == "gunner_sib" || prefix.IndexOf("helicoptermodsib") == 0 || prefix.IndexOf("helicoptersib") == 0 || prefix == "helicoptermodscripts")
			return true;
		return false;
	}

	private TStringSet FindPotentialPrefixes() {
		TStringSet all = new TStringSet();
		int childCount = g_Game.ConfigGetChildrenCount("CfgMods");
		for (int child = 0; child < childCount; ++child) {
			string name;
			g_Game.ConfigGetChildName("CfgMods", child, name);
			AddPotentialPrefixes(all, name);
		}
		foreach (string pboName : failedPBOs) {
			if (pboName.Length() > 4) {
				string nameOnly = pboName.Substring(0, pboName.Length() - 4);
				all.Insert(nameOnly);
				string nameOnly2 = nameOnly + "";
				nameOnly2.Replace("_", "/");
				all.Insert(nameOnly2);
			}
		}
		childCount = g_Game.ConfigGetChildrenCount("CfgPatches");
		for (child = 0; child < childCount; ++child) {
			string name2;
			g_Game.ConfigGetChildName("CfgPatches", child, name2);
			all.Insert(name2);
			string name3 = name2 + "";
			name3.Replace("_", "/");
			all.Insert(name3);
		}
		return all;
	}

	private void AddPotentialPrefixes(out TStringSet all, string cppName) {
		AddPotentialPrefixCpp(all, "CfgMods " + cppName + " dir");
		AddPotentialPrefixCpp(all, "CfgMods " + cppName + " inputs");
		AddPotentialPrefixCpps(all, "CfgMods " + cppName + " defs widgetStyles");
		AddPotentialPrefixCpps(all, "CfgMods " + cppName + " defs imageSets");
		AddPotentialPrefixCpps(all, "CfgMods " + cppName + " defs gameScriptModule");
		AddPotentialPrefixCpps(all, "CfgMods " + cppName + " defs worldScriptModule");
		AddPotentialPrefixCpps(all, "CfgMods " + cppName + " defs missionScriptModule");
	}

	private void AddPotentialPrefixCpps(out TStringSet all, string arrayPath) {
		if (!g_Game.ConfigIsExisting(arrayPath))
			return;
		TStringArray arr = new TStringArray();
		g_Game.ConfigGetTextArray(arrayPath, arr);
		foreach (string entry : arr) {
			AddPotentialPrefix(all, entry);
		}
	}

	private void AddPotentialPrefixCpp(out TStringSet all, string cpppath) {
		if (!g_Game.ConfigIsExisting(cpppath))
			return;
		AddPotentialPrefix(all, g_Game.ConfigGetTextOut(cpppath));
	}

	private void AddPotentialPrefix(out TStringSet all, string path) {
		all.Insert(path);
		TStringArray parts = LBFileTree.SplitFile(path);
		string full = "";
		foreach (string part : parts) {
			full = full + part + "/";
			all.Insert(full);
		}
	}

	private void LoadPBOs(string folder) {
		string filename;
		FileAttr fileAttr;
		FindFileHandle find;
		FindFileHandle findPBOs = FindFile(folder + "/*.pbo", filename, fileAttr, FindFileFlags.ALL);
		if (findPBOs == 0)
			return;
		Sleep(1);
		LoadPBO(folder + "/" + filename, filename);
		while (FindNextFile(findPBOs, filename, fileAttr)) {
			LoadPBO(folder + "/" + filename, filename);
		}
		CloseFindFile(findPBOs);
	}

	private void LoadPBO(string path, string filename) {
		TIntArray bytes = new TIntArray();
		if (!ReadHeaderList(path, bytes)) {
			failedPBOs.Insert(filename);
			return;
		}
		Sleep(1);
		LBLogger.Verbose("Loading PBO: " + path + " HeaderLength: " + bytes.Count(), "Core");
		map<string, string> properties = new map<string, string>();
		int index = ReadProperties(bytes, properties);
		LBLogger.Verbose("PBO has " + properties.Count() + " properties: ", "Core");
		foreach (string key, string value : properties)
			LBLogger.Verbose(key + " -> " + value, "Core");
		TStringArray filenames = new TStringArray();
		index = ReadFilenames(index, bytes, filenames);
		LBLogger.Verbose("PBO has " + filenames.Count() + " filenames: " + filenames, "Core");
		string prefix = GetPrefix(properties, filename);
		if (IsPrefixIgnored(prefix))
			return;
		foreach (string add : filenames) {
			fileTree.InsertFilePath(prefix + "/" + add);
		}
	}

	private string GetPrefix(map<string, string> properties, string filename) {
		foreach (string key, string value : properties) {
			if (LBStringTools.ToLowerString(key) == "prefix")
				return value;
		}
		int end = filename.Length() - 4;
		return filename.Substring(0, end);
	}

	private int ReadProperties(TIntArray bytes, out map<string, string> properties) {
		int i = 21;
		for (; i < bytes.Count() - 20;) {
			string key = ReadString(i, bytes);
			if (key == "")
				return i;
			string value = ReadString(i, bytes);
			properties.Insert(key, value);
		}
		return i;
	}

	private string ReadString(inout int index, TIntArray bytes) {
		string value = "";
		while (index < bytes.Count() && bytes.Get(index) != 0) {
			value += bytes.Get(index).AsciiToString();
			index++;
		}
		index++;
		return value;
	}

	private int ReadFilenames(int index, TIntArray bytes, out TStringArray filenames) {
		while (index < bytes.Count()) {
			string filename = ReadString(index, bytes);
			index += 20;
			if (filename == "")
				break;
			filenames.Insert(filename);
			if (index % 100 == 0)
				Sleep(1);
		}
		return index;
	}

	static TStringSet LoadDataFolderJsonFiles(string folder, int fileLength = -1) {
		return LoadAnyFolderJsonFiles("LBmaster/Data/" + folder, fileLength);
	}

	static set<string> LoadAnyFolderJsonFiles(string folder, int fileLength = -1) {
		TStringSet foundFiles = new TStringSet();
		string filename;
		FileAttr fileAttr;
		FindFileHandle handle = FindFile("$profile:" + folder + "/*.json", filename, fileAttr, FindFileFlags.ALL);
		if (handle == 0) {
#ifdef PLATFORM_LINUX
			LBLogger.Error("Could not search folder: $profile:" + folder + " for json files! Could not open search handle, make sure the permissions are setup properly!", "Core");
#endif
			return foundFiles;
		}
		if (filename.Length() > fileLength && fileLength != -1) {
			filename = filename.Substring(0, fileLength);
			foundFiles.Insert(filename);
		} else if (filename.Length() >= 5) {
			filename = filename.Substring(0, filename.Length() - 5);
			foundFiles.Insert(filename);
		} else if (filename != "") {
			LBLogger.Error("Found File " + filename + " in $profile:" + folder + " which was not expexted there!", "Core");
		}
		while (FindNextFile(handle, filename, fileAttr)) {
			if (filename.Length() > fileLength && fileLength != -1) {
				filename = filename.Substring(0, fileLength);
				foundFiles.Insert(filename);
			} else if (filename.Length() >= 5) {
				filename = filename.Substring(0, filename.Length() - 5);
				foundFiles.Insert(filename);
			} else if (filename != "") {
				LBLogger.Error("Found File " + filename + " in $profile:" + folder + " which was not expexted there!", "Core");
			}
		}
		CloseFindFile(handle);
		LBLogger.Debug("Found " + foundFiles.Count() + " json Files in $profile:" + folder, "Core");
		return foundFiles;
	}

	private bool ReadHeaderList(string pbo, out TIntArray output) {
		FileHandle file = OpenFile(pbo, FileMode.READ);
		string tempFilename = "$profile:temp.pbo";
		if (file == 0) {
			if (CopyFile(pbo, tempFilename)) {
				file = OpenFile(tempFilename, FileMode.READ);
			}
			if (file == 0) {
				LBLogger.Error("Failed to open file", "Core");
				return false;
			}
		}
		const int INT_COUNT = 1000;
		int bytes[INT_COUNT];
		int nulls = 0;
		while (true) {
			int read = ReadFile(file, bytes, INT_COUNT * 4);
			if (read <= 0)
				break;
			for (int i2 = 0; i2 < (read + 3) / 4; ++i2) {
				for (int a = 0; a < 4; ++a) {
					int value = (bytes[i2] >> (a * 8)) & 0xff;
					output.Insert(value);
					if (value == 0)
						nulls++;
					else
						nulls = 0;
					if (nulls == 22) {
						DeleteFile(tempFilename);
						CloseFile(file);
						return true;
					}
				}
			}
		}
		DeleteFile(tempFilename);
		CloseFile(file);
		return true;
	}
}