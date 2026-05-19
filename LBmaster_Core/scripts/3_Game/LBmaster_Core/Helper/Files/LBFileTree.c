class LBFileTree {

	string name, fullPath, ending = "";
	bool folder = false;
	ref array<ref LBFileTree> children = new array<ref LBFileTree>();
	ref map<string, LBFileTree> childrenMap = new map<string, LBFileTree>();
	bool sorted = false;

	static TStringArray SplitFile(string filename) {
		TStringArray parts = new TStringArray();
		int index = 0;
		while (index <= filename.Length()) {
			int from = index;
			index = filename.IndexOfFrom(from, "/");
			int index2 = filename.IndexOfFrom(from, "\\");
			if (index == -1 || (index2 != -1 && index2 < index)) {
				index = index2;
			}
			int end = index;
			if (index == -1) {
				end = filename.Length();
			}
			string part = filename.Substring(from, end - from);
			parts.Insert(part);
			index = end + 1;
		}
		return parts;
	}

	bool IsFolder() {
		return folder;
	}

	int GetChildIndex(string search) {
		for (int i = 0; i < children.Count(); ++i) {
			if (children.Get(i).name == search)
				return i;
		}
		return -1;
	}

	void LBFileTree(string filename, string full) {
		this.fullPath = full;
		this.name = filename;
		int index = filename.LastIndexOf(".");
		if (index != -1) {
			this.ending = filename.Substring(index, filename.Length() - index);
		}
	}

	void SortChildren() {
		if (sorted)
			return;
		while (!sorted) {
			sorted = true;
			int count = children.Count();
			for (int i = 1; i < count; ++i) {
				LBFileTree prev = children.Get(i - 1);
				LBFileTree child = children.Get(i);
				if (LBStringTools.Comp(prev.name, child.name, false) > 0) {
					children.Set(i, prev);
					children.Set(i - 1, child);
					sorted = false;
				}
			}
		}
	}

	void InsertFilePath(string full) {
		TStringArray parts = SplitFile(full);
		LBFileTree tree = this;
		string pre = "";
		foreach (string part : parts) {
			tree = tree.AddChild(part, pre + part);
			pre = pre + part + "/";
		}
	}

	LBFileTree FindFileInFolders(string path) {
		string lower = LBStringTools.ToLowerString(path);
		TStringArray parts = new TStringArray();
		lower.Split("/", parts);
		LBFileTree parent = this;
		foreach (string part : parts) {
			if (!parent)
				break;
			parent = parent.FindFile(part);
		}
		return parent;
	}

	LBFileTree FindFile(string filename) {
		string lower = LBStringTools.ToLowerString(filename);
		return childrenMap.Get(lower);
	}

	private LBFileTree AddChild(string filename, string full) {
		LBFileTree file;
		this.folder = true;
		string lower = LBStringTools.ToLowerString(filename);
		if (childrenMap.Find(lower, file)) {
			return file;
		}
		sorted = false;
		file = new LBFileTree(filename, full);
		childrenMap.Insert(lower, file);
		children.Insert(file);
		return file;
	}

}