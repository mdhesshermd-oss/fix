class LBStringArrayTools {

	// TEST START
	[LBTestManager.StartTest(ScriptCaller.Create(FindBestTest))]
	private static void FindBestTest() {
		LBTest<string>.Assert(FindBestMatch({"abbc", "abc", "ac", "bdf", "fgh"}, "a", false), "abbc");
		LBTest<string>.Assert(FindBestMatch(new TStringArray(), "a", false), "");
		LBTest<string>.Assert(FindBestMatch({"abbc", "abc", "ac", "bdf", "fgh"}, "b", false), "bdf");
		LBTest<string>.Assert(FindBestMatch({"abbc", "abc", "ac", "bdf", "fgh"}, "ab", false), "abbc");
		LBTest<string>.Assert(FindBestMatch({"abc", "abcdef", "ac", "bdf", "fgh"}, "abc", false), "abc");
		LBTest<string>.Assert(FindBestMatch({"abc", "abcdef", "ac", "bdf", "fgh"}, "abcd", false), "abcdef");
		LBTest<string>.Assert(FindBestMatch({"abc", "abcdef", "ac", "bdf", "fgh"}, "f", false), "fgh");
		LBTest<string>.Assert(FindBestMatch({"abc", "abcdef", "ac", "acc", "bdf", "fgh"}, "", false), "abc");
		LBTest<string>.Assert(FindBestMatch({"abc", "abcdef", "ac", "ba", "bc", "bdfff", "bdf", "fgh"}, "z", false), "fgh");
		LBTest<string>.Assert(FindBestMatch({"abc", "abcdef", "ac", "ba", "bc", "bdfff", "fgh"}, "z", false), "fgh");
		LBTest<string>.Assert(FindBestMatch({"abc", "abcdef", "ac", "bdf", "fgh"}, "A", true), "abc");
		LBTest<string>.Assert(FindBestMatch({"ABC", "abcdef", "ac", "bdf", "fgh"}, "A", true), "ABC");
		LBTest<string>.Assert(FindBestMatch({"ABCDE", "ABC", "abcdef", "ac", "bdf", "fgh"}, "ABC", true), "ABCDE");
		LBTest<string>.Assert(FindBestMatch({"ABC", "abcdef", "ac", "bdf", "fgh"}, "@BCDE", true), "ABC");
		LBTest<string>.Assert(FindBestMatch({"ABC", "abcdef", "ac", "bdf", "fgh"}, "a", true), "abcdef");
		LBTest<string>.Assert(FindBestMatch({"ABC", "abcdef", "ac", "bdf", "fgh"}, "AA", true), "ABC");
	}

	[LBTestManager.StartTest(ScriptCaller.Create(FindBestMatchStartingTest))]
	private static void FindBestMatchStartingTest() {
		LBTest<string>.Assert(FindBestMatchStarting({"abbc", "abc", "ac", "bdf", "fgh"}, "a", false), "abbc");
		LBTest<string>.Assert(FindBestMatchStarting(new TStringArray(), "a", false), "");
		LBTest<string>.Assert(FindBestMatchStarting({"abbc", "abc", "ac", "bdf", "fgh"}, "b", false), "bdf");
		LBTest<string>.Assert(FindBestMatchStarting({"abbc", "abc", "ac", "bdf", "fgh"}, "ab", false), "abbc");
		LBTest<string>.Assert(FindBestMatchStarting({"abc", "abcdef", "ac", "bdf", "fgh"}, "abc", false), "abc");
		LBTest<string>.Assert(FindBestMatchStarting({"abc", "abcdef", "ac", "bdf", "fgh"}, "abcd", false), "abcdef");
		LBTest<string>.Assert(FindBestMatchStarting({"abc", "abcdef", "ac", "bdf", "fgh"}, "f", false), "fgh");
		LBTest<string>.Assert(FindBestMatchStarting({"abc", "abcdef", "ac", "acc", "bdf", "fgh"}, "", false), "abc");
		LBTest<string>.Assert(FindBestMatchStarting({"abc", "abcdef", "ac", "ba", "bc", "bdfff", "bdf", "fgh"}, "z", false), "");
		LBTest<string>.Assert(FindBestMatchStarting({"abc", "abcdef", "ac", "ba", "bc", "bdfff", "fgh"}, "z", false), "");
		LBTest<string>.Assert(FindBestMatchStarting({"abc", "abcdef", "ac", "bdf", "fgh"}, "A", true), "");
		LBTest<string>.Assert(FindBestMatchStarting({"ABC", "abcdef", "ac", "bdf", "fgh"}, "A", true), "ABC");
		LBTest<string>.Assert(FindBestMatchStarting({"ABCDE", "ABC", "abcdef", "ac", "bdf", "fgh"}, "ABC", true), "ABCDE");
		LBTest<string>.Assert(FindBestMatchStarting({"ABC", "abcdef", "ac", "bdf", "fgh"}, "@BCDE", true), "");
		LBTest<string>.Assert(FindBestMatchStarting({"ABC", "abcdef", "ac", "bdf", "fgh"}, "a", true), "abcdef");
		LBTest<string>.Assert(FindBestMatchStarting({"ABC", "abcdef", "ac", "bdf", "fgh"}, "ac", true), "ac");
		LBTest<string>.Assert(FindBestMatchStarting({"ABC", "abcdef", "ac", "bdf", "fgh"}, "AA", true), "");
	}

	[LBTestManager.StartTest(ScriptCaller.Create(QuickSortTest))]
	private static void QuickSortTest() {
		TStringArray arr = { "a", "c", "b", "d", "ab", "cd" };
		LBTestArray<string>.Assert(QuickSort(arr, true), {"a", "ab", "b", "c", "cd", "d"});
		LBTestArray<string>.Assert(QuickSort(arr, false), {"a", "ab", "b", "c", "cd", "d"});
		arr = {"A", "b", "hallo", "test", "Proc", "", " ", "empty"};
		LBTestArray<string>.Assert(QuickSort(arr, true), {"", " ", "A", "Proc", "b", "empty", "hallo", "test"});
		LBTestArray<string>.Assert(QuickSort(arr, false), {"", " ", "A", "b", "empty", "hallo", "Proc", "test"});
		arr = {"abc", "abbc", "ac", "bdf", "fgh"};
		LBTestArray<string>.Assert(QuickSort(arr, false), {"abbc", "abc", "ac", "bdf", "fgh"});
		arr = {"abc", "abcdef", "ac", "bdf", "fgh"};
		LBTestArray<string>.Assert(QuickSort(arr, false), {"abc", "abcdef", "ac", "bdf", "fgh"});
		arr = {"ABC", "abcdef", "ac", "bdf", "fgh"};
		LBTestArray<string>.Assert(QuickSort(arr, false), {"ABC", "abcdef", "ac", "bdf", "fgh"});
	}

	[LBTestManager.StartTest(ScriptCaller.Create(IsInLowerListTest))]
	private static void IsInLowerListTest() {
		TStringArray lowerList = {"abd", "fhjds", "hey", "", "peter"};
		LBTest<bool>.Assert(IsInLowerList(lowerList, "hey"), true);
		LBTest<bool>.Assert(IsInLowerList(lowerList, "heyy"), false);
		LBTest<bool>.Assert(IsInLowerList(lowerList, "HEY"), true);
		LBTest<bool>.Assert(IsInLowerList(lowerList, " "), false);
		LBTest<bool>.Assert(IsInLowerList(lowerList, ""), true);
		LBTest<bool>.Assert(IsInLowerList(lowerList, "Peter "), false);
		LBTest<bool>.Assert(IsInLowerList(lowerList, "Peter"), true);
		LBTest<bool>.Assert(IsInLowerList(lowerList, "peter"), true);
	}
	// TEST END

	static bool IsInLowerList(TStringArray arr, string classname) {
		string lower = classname + "";
		lower.ToLower();
		return arr.Find(lower) != -1;
	}

	static TStringArray QuickSort(TStringArray arr, bool caseSensitive) {
		TStringArray copy = new TStringArray();
		copy.InsertArray(arr);
		QuickSort(copy, caseSensitive, 0, copy.Count() - 1);
		return copy;
	}

	private static void QuickSort(TStringArray arr, bool caseSensitive, int begin, int end) {
		if (begin < end) {
			int partitionIndex = Partition(arr, begin, end, caseSensitive);

			QuickSort(arr, caseSensitive, begin, partitionIndex - 1);
			QuickSort(arr, caseSensitive, partitionIndex + 1, end);
		}
	}

	private static int Partition(TStringArray arr, int begin, int end, bool caseSensitive) {
		string pivot = arr[end];
		int i = (begin - 1);

		for (int j = begin; j < end; j++) {
			if (LBStringTools.Comp(arr[j], pivot, caseSensitive) < 0) {
				++i;

				string swapTemp = arr[i];
				arr[i] = arr[j];
				arr[j] = swapTemp;
			}
		}

		swapTemp = arr[i + 1];
		arr[i + 1] = arr[end];
		arr[end] = swapTemp;

		return i + 1;
	}

	static string FindBestMatch(TStringArray arr, string text, bool caseSensitive) {
		if (arr.Count() == 0)
			return "";
		int min = 0, max = arr.Count() - 1;
		while (min < max) {
			int mid = min + ((max - min) / 2);
			string midElement = arr.Get(mid);
			int comp = LBStringTools.Comp(midElement, text, caseSensitive);
			if (comp < 0) {
				min = mid + 1;
			} else {
				max = mid;
			}
		}
		return arr.Get(min);
	}

	static string FindBestMatchStarting(TStringArray arr, string text, bool caseSensitive) {
		string best = FindBestMatch(arr, text, caseSensitive);
		string bestOriginal = best;
		if (!caseSensitive) {
			best = LBStringTools.ToLowerString(best);
			text = LBStringTools.ToLowerString(text);
		}
		if (best.IndexOf(text) == 0)
			return bestOriginal;
		return "";

	}

}