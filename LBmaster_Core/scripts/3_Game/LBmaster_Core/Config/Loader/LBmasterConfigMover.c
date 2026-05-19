class LBmasterConfigMover {

	static void MoveFolder(string from, string to) {
		from = TrimSlashes(from);
		to = TrimSlashes(to);
		if (!FileExist(from))
			return;
		string filename = "";
		FileAttr attr;
		FindFileHandle handle = FindFile(from + "/*", filename, attr, FindFileFlags.ALL);
		if (filename != "") {
			if (attr == FileAttr.DIRECTORY) {
				MoveFolder(from + "/" + filename, to + "/" + filename);
			} else {
				MoveFileToFolder(from + "/" + filename, to);
			}
			while (FindNextFile(handle, filename, attr)) {
				if (attr == FileAttr.DIRECTORY) {
					MoveFolder(from + "/" + filename, to + "/" + filename);
				} else {
					MoveFileToFolder(from + "/" + filename, to);
				}
			}
			CloseFindFile(handle);
		}
	}

	static void MoveFileToFolder(string from, string toFolder) {
		toFolder = TrimSlashes(toFolder);
		string fromFolder = GetParentFolder(from);
		string fromFile = from.Substring(fromFolder.Length() + 1, from.Length() - 1 - fromFolder.Length());
		string to = toFolder + "/" + fromFile;
		MoveFileTo(from, to);
	}

	static void MoveFileTo(string from, string toFile, bool deleteFolder = true) {
		string toFolder = GetParentFolder(toFile);
		if (!FileExist(toFile) && FileExist(from)) {
			CreateFolders(toFolder);
			if (CopyFile(from, toFile)) {
				LBLogger.Debug("Moved File: " + from + " To: " + toFile, "Core");
				DeleteFile(from);
			}
		}
		if (deleteFolder)
			DeleteEmptyFolder(GetParentFolder(from));
	}

	static void DeleteFileAndBackups(string name) {
		LBDeleteFile(name);
		LBDeleteFile(name + ".backup");
		LBDeleteFile(name + ".backup2");
	}

	static void LBDeleteFile(string name) {
		if (FileExist(name))
			DeleteFile(name);
	}

	static void MoveFileAndBackupsFree(string from, string toFolder) {
		int i = 1;

		toFolder = TrimSlashes(toFolder);
		string fromFolder = GetParentFolder(from);
		string fromFile = from.Substring(fromFolder.Length() + 1, from.Length() - 1 - fromFolder.Length());
		string to = toFolder + "/" + fromFile;

		while (JsonFileExist(to)) {
			to = toFolder + "/" + fromFile + "(" + i + ")";
			++i;
		}
		LBLogger.Verbose("Moving " + from + " and backups to " + to, "Core");
		MoveFileTo(from, to, false);
		MoveFileTo(from + ".backup", to + ".backup", false);
		MoveFileTo(from + ".backup2", to + ".backup2", false);
	}

	static bool JsonFileExist(string filename) {
		return FileExist(filename) || FileExist(filename + ".backup") || FileExist(filename + ".backup2");
	}

	static void MoveFileAndBackups(string from, string toFolder) {
		MoveFileToFolder(from, toFolder);
		MoveFileToFolder(from + ".backup", toFolder);
		MoveFileToFolder(from + ".backup2", toFolder);
	}

	static void CreateFolders(string folder) {
		folder = TrimSlashes(folder);
		while (folder != "") {
			CreateParentFolders(folder);
			if (!FileExist(folder)) {
				LBLogger.Debug("Create Folder: " + folder, "Core");
				MakeDirectory(folder);
			}
			folder = GetParentFolder(folder);
		}

	}

	static void CreateParentFolders(string file) {
		string parent = GetParentFolder(file);
		CreateFolders(parent);
	}

	static string GetParentFolder(string path) {
		int length = path.Length();
		int index = length - 1;
		while (index >= 0) {
			string part = path.Substring(index, length - index);
			if (part.Contains("/") || part.Contains("\\"))
				break;
			index--;
		}
		if (index >= 0)
			return path.Substring(0, index);
		return "";
	}

	static void DeleteEmptyFolder(string path) {
		string filename = "";
		FileAttr attr;
		FindFileHandle handle = FindFile(path + "/*", filename, attr, FindFileFlags.ALL);
		CloseFindFile(handle);
		if (filename == "") {
			DeleteFile(path);
		}

		string parent = GetParentFolder(path);
		if (parent != "")
			DeleteEmptyFolder(parent);

	}

	static string TrimSlashes(string path) {
		if (path.Length() == 0)
			return path;
		if (path.Substring(path.Length() - 1, 1) == "/" || path.Substring(path.Length() - 1, 1) == "\\") {
			return path.Substring(0, path.Length() - 1);
		}
		return path;
	}

}