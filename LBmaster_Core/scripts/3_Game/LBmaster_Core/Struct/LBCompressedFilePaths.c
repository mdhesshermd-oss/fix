class LBCompressedFilePaths {

	string name;
	ref array<ref LBCompressedFilePaths> childPaths = new array<ref LBCompressedFilePaths>();
	ref map<string, LBCompressedFilePaths> childCache = new map<string, LBCompressedFilePaths>();
	int myIndex = -1;

	[LBTestManager.StartTest(ScriptCaller.Create(TestCompressionGenerator))]
	static void TestCompressionGenerator() {
		LBCompressedFilePaths root1 = new LBCompressedFilePaths();
		LBCompressedFilePaths child1 = root1.AddPath("lb/test/abc.paa");
		LBCompressedFilePaths child2 = root1.AddPath("lb/test/cde.paa");
		LBCompressedFilePaths child3 = root1.AddPath("lb/abc/abc.paa");
		LBCompressedFilePaths child4 = root1.AddPath("lb/abc/abc.paa");
		LBCompressedFilePaths child5 = root1.AddPath("t/abc");
		LBTest<int>.Assert(root1.childPaths.Count(), 2);
		LBTest<int>.Assert(root1.childCache.Count(), 2);
		LBTest<int>.Assert(root1.myIndex, -1);
		root1.CalculateIndex();
		LBTest<int>.Assert(root1.myIndex, 0);
		LBTest<int>.Assert(root1.childPaths.Get(0).myIndex, 1);
		LBTest<int>.Assert(root1.childPaths.Get(1).myIndex, 7);
		LBTest<int>.Assert(child1.myIndex, 3);
		LBTest<int>.Assert(child2.myIndex, 4);
		LBTest<int>.Assert(child3.myIndex, 6);
		LBTest<int>.Assert(child4.myIndex, 6);
		LBTest<int>.Assert(child5.myIndex, 8);
		LBTest<string>.Assert(root1.GetPath(6), "lb\\abc\\abc.paa");
		LBTest<string>.Assert(root1.GetPath(5), "lb\\abc");
		LBTest<string>.Assert(root1.GetPath(0), "");
		LBTest<bool>.Assert(child3 == child4, true);
		ScriptReadWriteContext ctx = new ScriptReadWriteContext();
		root1.WriteToCtx(ctx.GetWriteContext());
		LBCompressedFilePaths root2 = new LBCompressedFilePaths();
		LBTest<bool>.Assert(root2.ReadFromCtx(ctx.GetReadContext()), true);
		LBTest<string>.Assert(root1.GetPath(3), root2.GetPath(3));
		TStringArray list1 = root1.GetIndexList();
		TStringArray list2 = root2.GetIndexList();
		LBTest<int>.Assert(list1.Count(), 9);
		LBTest<int>.Assert(list2.Count(), 9);
		LBTest<string>.Assert(list2.Get(3), "lb\\test\\abc.paa");
		LBTest<string>.Assert(list2.Get(0), "");
		LBTest<string>.Assert(list2.Get(1), "lb");
		LBTest<string>.Assert(list2.Get(7), "t");
		LBTestArray<string>.Assert(list1, list2);
		LBTest<int>.Assert(list1.Count(), root1.GetMaxIndex() + 1);
		LBTest<int>.Assert(list2.Count(), root2.GetMaxIndex() + 1);
	}

	TStringArray GetIndexList() {
		TStringArray arr = new TStringArray();
		arr.Reserve(GetMaxIndex() + 1);
		WriteToIndexList(arr, "", "\\");
		return arr;
	}

	private void WriteToIndexList(TStringArray list, string prevPath, string delimiter) {
		if (myIndex == 0) {
			list.Insert(name);
		} else if (prevPath != "") {
			prevPath = prevPath + delimiter + name;
			list.Insert(prevPath);
		} else {
			prevPath = name;
			list.Insert(prevPath);
		}
		foreach (LBCompressedFilePaths child : childPaths) {
			child.WriteToIndexList(list, prevPath, delimiter);
		}
	}

	void WriteToCtx(ParamsWriteContext ctx) {
		ctx.Write(name);
		int count = childPaths.Count();
		ctx.Write(count);
		for (int i = 0; i < count; ++i) {
			childPaths.Get(i).WriteToCtx(ctx);
		}
	}

	bool ReadFromCtx(ParamsReadContext ctx) {
		if (!ReadFromCtxInt(ctx))
			return false;
		CalculateIndex();
		return true;
	}

	private bool ReadFromCtxInt(ParamsReadContext ctx) {
		if (!ctx.Read(name))
			return false;
		int count;
		if (!ctx.Read(count))
			return false;
		for (int i = 0; i < count; ++i) {
			LBCompressedFilePaths child = new LBCompressedFilePaths();
			if (!child.ReadFromCtxInt(ctx))
				return false;
			childPaths.Insert(child);
			childCache.Insert(child.name, child);
		}
		return true;
	}

	LBCompressedFilePaths Get(string search) {
		LBCompressedFilePaths found;
		if (childCache.Find(search, found))
			return found;
		found = new LBCompressedFilePaths();
		found.name = search;
		childPaths.Insert(found);
		childCache.Insert(search, found);
		return found;
	}

	LBCompressedFilePaths AddPath(string path) {
		if (path.Contains("\\"))
			path.Replace("\\", "/");
		path.ToLower();
		return Add(path, "/");
	}

	LBCompressedFilePaths Add(string path, string delimiter) {
		int start = 0;
		LBCompressedFilePaths current = this;
		while (true) {
			int end = path.IndexOfFrom(start, delimiter);
			if (end == -1) {
				string subStr = path.Substring(start, path.Length() - start);
				return current.Get(subStr);
			}
			subStr = path.Substring(start, end - start);
			current = current.Get(subStr);
			start = end + 1;
		}
		return current;
	}

	string GetPath(int index) {
		return Get(index, "\\");
	}

	string Get(int index, string delimiter) {
		if (index == myIndex)
			return name;
		for (int i = childPaths.Count() - 1; i >= 0; --i) {
			if (childPaths.Get(i).myIndex > index)
				continue;
			if (myIndex == 0)
				return childPaths.Get(i).GetPath(index);
			return name + delimiter + childPaths.Get(i).GetPath(index);
		}
		return "";
	}

	int GetMaxIndex() {
		if (childPaths.Count() == 0)
			return myIndex;
		return childPaths.Get(childPaths.Count() - 1).GetMaxIndex();
	}

	void CalculateIndex() {
		int current = 0;
		CalcMyIndex(current);
	}

	private void CalcMyIndex(inout int current) {
		myIndex = current;
		++current;
		foreach (LBCompressedFilePaths child : childPaths) {
			child.CalcMyIndex(current);
		}
	}

}