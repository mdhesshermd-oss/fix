class LBStringTools {

	// TEST START
	[LBTestManager.StartTest(ScriptCaller.Create(CompareTest))]
	private static void CompareTest() {
		LBTest<int>.Assert(Comp("", "", true), 0);
		LBTest<int>.Assert(Comp(" ", "", true), 1);
		LBTest<int>.Assert(Comp("", " ", true), -1);
		LBTest<int>.Assert(Comp("hello", "Hello", true), 32);
		LBTest<int>.Assert(Comp("hello", "Hello", false), 0);
		LBTest<int>.Assert(Comp("hello World", "Hello", false), 6);
		LBTest<int>.Assert(Comp("hello World", "Hello", true), 32);
		LBTest<int>.Assert(Comp("Peter", "Peter Meyer", true), -6);
		LBTest<int>.Assert(Comp("Peter", "peter Meyer ali", false), -10);
		LBTest<int>.Assert(Comp("Herr", "Arnold", false), 7);
		LBTest<int>.Assert(Comp("Affe", "Hugo", true), -7);
		LBTest<int>.Assert(Comp("abc", "abcd", true), -1);
		LBTest<int>.Assert(Comp("abcd", "abc", true), 1);
		LBTest<int>.Assert(Comp("abcdef", "abcd", true), 2);
		LBTest<int>.Assert(Comp("abcdef", "aa", true), 1);
		LBTest<int>.Assert(Comp("ABC", "aa", true), -32);
		LBTest<int>.Assert(Comp("ABC", "ABCDE", true), -2);
	}

	[LBTestManager.StartTest(ScriptCaller.Create(ToLowerStringTest))]
	private static void ToLowerStringTest() {
		LBTest<string>.Assert(ToLowerString("Hallo"), "hallo");
		LBTest<string>.Assert(ToLowerString("Hallo "), "hallo ");
		LBTest<string>.Assert(ToLowerString(" "), " ");
		LBTest<string>.Assert(ToLowerString(""), "");
		LBTest<string>.Assert(ToLowerString("PETER"), "peter");
		LBTest<string>.Assert(ToLowerString("already lower"), "already lower");
	}

	[LBTestManager.StartTest(ScriptCaller.Create(ToLowerStringFromToTest))]
	private static void ToLowerStringFromToTest() {
		LBTest<string>.Assert(ToLowerStringFromTo("CAPSLOCK", 0, 8), "capslock");
		LBTest<string>.Assert(ToLowerStringFromTo("CAPSLOCK", 1, 7), "CapslocK");
		LBTest<string>.Assert(ToLowerStringFromTo("CAPSLOCK", 4, 8), "CAPSlock");
	}

	[LBTestManager.StartTest(ScriptCaller.Create(FindStringEndTest))]
	private static void FindStringEndTest() {
		LBTest<int>.Assert(FindStringEnd("", 0), 0);
		LBTest<int>.Assert(FindStringEnd(" ", 0), 0);
		LBTest<int>.Assert(FindStringEnd("ab ", 0), 2);
		LBTest<int>.Assert(FindStringEnd("ab", 0), 2);
		LBTest<int>.Assert(FindStringEnd("ab dfe", 2), 2);
		LBTest<int>.Assert(FindStringEnd("ab#dfe", 2), 2);
		LBTest<int>.Assert(FindStringEnd("abd#dfe", 2), 3);
		LBTest<int>.Assert(FindStringEnd("ab dfe", 3), 6);
		LBTest<int>.Assert(FindStringEnd("ab\ndfe", 3), 6);
		LBTest<int>.Assert(FindStringEnd("ab#dfe", 3), 6);
		LBTest<int>.Assert(FindStringEnd("ab\ndfe", 0), 2);
		LBTest<int>.Assert(FindStringEnd("ab\tdfe", 0), 2);
	}

	[LBTestManager.StartTest(ScriptCaller.Create(MultiTranslateStringTest))]
	private static void MultiTranslateStringTest() {
		LBTest<string>.Assert(MultiTranslateString(""), "");
		LBTest<string>.Assert(MultiTranslateString("hello world"), "hello world");
		LBTest<string>.Assert(MultiTranslateString("hello# world"), "hello world");
		LBTest<string>.Assert(MultiTranslateString("hello#lb_file_select world"), "hello" + Widget.TranslateString("#lb_file_select") + " world");
		LBTest<string>.Assert(MultiTranslateString("#lb_file_select world"), Widget.TranslateString("#lb_file_select") + " world");
		LBTest<string>.Assert(MultiTranslateString("#lb_file_select"), Widget.TranslateString("#lb_file_select"));
		LBTest<string>.Assert(MultiTranslateString("#lb_file_select#lb_file_select"), Widget.TranslateString("#lb_file_select") + Widget.TranslateString("#lb_file_select"));
		LBTest<string>.Assert(MultiTranslateString("hello #lb_file_select #lb_file_select"), "hello " + Widget.TranslateString("#lb_file_select") + " " + Widget.TranslateString("#lb_file_select"));
		LBTest<string>.Assert(MultiTranslateString("hello #lb_file_selec #lb_file_select"), "hello " + Widget.TranslateString("#lb_file_selec") + " " + Widget.TranslateString("#lb_file_select"));
	}

	[LBTestManager.StartTest(ScriptCaller.Create(EndsWithTest))]
	private static void EndsWithTest() {
		LBTest<bool>.Assert(EndsWith("", ""), true);
		LBTest<bool>.Assert(EndsWith("abc", ""), true);
		LBTest<bool>.Assert(EndsWith("abc", "c"), true);
		LBTest<bool>.Assert(EndsWith("abc", "b"), false);
		LBTest<bool>.Assert(EndsWith("herbert", "ert"), true);
		LBTest<bool>.Assert(EndsWith("herbert", "eRt"), false);
		LBTest<bool>.Assert(EndsWith("herbert", ""), true);
		LBTest<bool>.Assert(EndsWith("herbert", "fgbhdsbfhsd"), false);
		LBTest<bool>.Assert(EndsWith("herbert", "eerbert"), false);
		LBTest<bool>.Assert(EndsWith("<", "<?"), false);
		LBTest<bool>.Assert(EndsWith("<?", "<?"), true);
	}

	[LBTestManager.StartTest(ScriptCaller.Create(IsNumericTest))]
	private static void IsNumericTest() {
		LBTest<bool>.Assert(IsNumeric(""), false);
		LBTest<bool>.Assert(IsNumeric("a"), false);
		LBTest<bool>.Assert(IsNumeric("0"), true);
		LBTest<bool>.Assert(IsNumeric("9"), true);
		LBTest<bool>.Assert(IsNumeric("9 "), false);
		LBTest<bool>.Assert(IsNumeric(" 0"), false);
		LBTest<bool>.Assert(IsNumeric("a5"), false);
	}

	[LBTestManager.StartTest(ScriptCaller.Create(IsFloatCharTest))]
	private static void IsFloatCharTest() {
		LBTest<bool>.Assert(IsFloatChar(""), false);
		LBTest<bool>.Assert(IsFloatChar("e"), true);
		LBTest<bool>.Assert(IsFloatChar("."), true);
		LBTest<bool>.Assert(IsFloatChar("E"), true);
		LBTest<bool>.Assert(IsFloatChar("9 "), false);
		LBTest<bool>.Assert(IsFloatChar(" 0"), false);
		LBTest<bool>.Assert(IsFloatChar("a5"), false);
	}

	[LBTestManager.StartTest(ScriptCaller.Create(JoinTest))]
	private static void JoinTest() {
		LBTest<string>.Assert(Join(new TStringArray(), ", "), "");
		LBTest<string>.Assert(Join({"Test"}, ", "), "Test");
		LBTest<string>.Assert(Join({"Test"}, ""), "Test");
		LBTest<string>.Assert(Join({""}, ""), "");
		LBTest<string>.Assert(Join({"TEst", "Peter"}, ""), "TEstPeter");
		LBTest<string>.Assert(Join({"TEst", "Peter"}, ", "), "TEst, Peter");
		LBTest<string>.Assert(Join({"1", "2", "3"}, "."), "1.2.3");
	}

	[LBTestManager.StartTest(ScriptCaller.Create(SplitJoinTest))]
	private static void SplitJoinTest() {
		LBTest<string>.Assert(Join(Split("Test Test Test", " "), " "), "Test Test Test");
		LBTest<string>.Assert(Join(Split("Test Test Test  ", " "), " "), "Test Test Test  ");
		LBTest<string>.Assert(Join(Split("Test Test Test  ", "Test"), "Test"), "Test Test Test  ");
		LBTest<string>.Assert(Join(Split("absdhudfhabsdasdsadh", "a"), "a"), "absdhudfhabsdasdsadh");
		LBTest<string>.Assert(Join(Split("absdhudfhabsdasdsadh", "sd"), "sd"), "absdhudfhabsdasdsadh");
	}

	[LBTestManager.StartTest(ScriptCaller.Create(EndTest))]
	private static void EndTest() {
		LBTest<string>.Assert(End("Test Test Test", -1), "");
		LBTest<string>.Assert(End("Test Test Test", 0), "Test Test Test");
		LBTest<string>.Assert(End("Test Test Test", 2), "st Test Test");
		LBTest<string>.Assert(End("Test Test Test", 200), "");
		LBTest<string>.Assert(End("Test Test Test", 14), "");
		LBTest<string>.Assert(End("Test Test Test", 12), "st");
	}
	// TEST END

	static typename FixTypename(typename other) {
		string tnStr = other.ToString();
		typename tn = tnStr.ToType();
		return tn;
	}

	static TStringArray Split(string str, string delimiter) {
		TStringArray output = new TStringArray();
		int txt_len	= 0;
		int del_len = delimiter.Length();
		int sep_pos	= -del_len;
		int nxt_sep_pos = 0;
		string text = "";

		bool line_end = false;
		while (line_end == false) {
			sep_pos = sep_pos + txt_len + del_len;
			nxt_sep_pos = str.IndexOfFrom(sep_pos, delimiter);
			if (nxt_sep_pos == -1) {
				nxt_sep_pos = str.Length();
				line_end = true;
			}

			txt_len = nxt_sep_pos - sep_pos;
			text = str.Substring(sep_pos, txt_len);
			output.Insert(text);
		}
		return output;
	}

	static string Join(TStringArray arr, string delimiter) {
		int count = arr.Count();
		if (count == 0)
			return "";
		if (count == 1)
			return arr[0];
		string str = arr[0];
		for (int i = 1; i < count; ++i)
			str += delimiter + arr[i];
		return str;
	}

	static bool IsNumeric(string char) {
		int hash = char.Hash();
		return hash > 47 && hash < 58;
	}

	static bool IsFloatChar(string char) {
		return char == "." || char == "e" || char == "E";
	}

	static bool IsTextCharacter(string char) {
		int hash = char.Hash();
		return char == "_" || (hash > 47 && hash < 58) || (hash > 96 && hash < 123) || (hash > 64 && hash < 91); // Num, Lower, Upper
	}

	static string MultiTranslateString(string input) {
		int index = input.IndexOf("#");
		string output = "";
		int lastEnd = 0;
		while (index >= 0) {
			output += input.Substring(lastEnd, index - lastEnd);
			int end = FindStringEnd(input, index + 1);
			output += Widget.TranslateString(input.Substring(index, end - index));
			index = input.IndexOfFrom(end, "#");
			lastEnd = end;
		}
		output += input.Substring(lastEnd, input.Length() - lastEnd);
		return output;
	}

	private static int FindStringEnd(string input, int start) {
		TStringArray endCharacters = {" ", "\n", "\t", "#", "!", ".", "?"};
		while (input.Length() > start && endCharacters.Find(input[start]) == -1)
			++start;
		return start;
	}

	static string RandomString(int length) {
		string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-+";
		int len = characters.Length();
		string output = "";
		for (int i = 0; i < length; ++i) {
			string char = characters.Get(Math.RandomInt(0, len));
			output = output + char;
		}
		return output;
	}

	static string RandomCharString(int length) {
		string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		int len = characters.Length();
		string output = "";
		for (int i = 0; i < length; ++i) {
			string char = characters.Get(Math.RandomInt(0, len));
			output = output + char;
		}
		return output;
	}

	static bool SafeReplace(inout string str, string search, string replaced) {
		if (str.IndexOf(search) == -1)
			return false;
		return str.Replace(search, replaced);
	}

	static bool EndsWith(string str, string end) {
		int lenDiff = str.Length() - end.Length();
		return end == "" || (lenDiff >= 0 && str.LastIndexOf(end) == lenDiff);
	}

	static bool EqualsIgnoreCase(string str1, string str2) {
		str1.ToLower();
		str2.ToLower();
		return str1 == str2;
	}

	static bool ContainsIgnoreCase(string str, string search) {
		if (search == "")
			return true;
		str.ToLower();
		search.ToLower();
		int index = str.IndexOf(search);
		return index != -1;
	}

	static string ToLowerString(string other) {
		string copy = other + "";
		copy.ToLower();
		return copy;
	}

	static string ToLowerStringFromTo(string other, int startIndex, int endIndex) {
		int len = other.Length();
		if (endIndex > len)
			endIndex = len;
		string start = other.Substring(0, startIndex);
		string end = other.Substring(endIndex, len - endIndex);
		string mid = other.Substring(startIndex, endIndex - startIndex);
		mid.ToLower();
		return start + mid + end;
	}

	static int Comp(string a, string b, bool caseSensitive) {
		int min = a.Length();
		if (b.Length() < min)
			min = b.Length();
		if (caseSensitive) {
			for (int i = 0; i < min; ++i) {
				int ah = a.Get(i).Hash();
				int bh = b.Get(i).Hash();
				if (ah != bh)
					return ah - bh;
			}
		} else {
			for (i = 0; i < min; ++i) {
				string al = ToLowerString(a.Get(i));
				string bl = ToLowerString(b.Get(i));
				ah = al.Hash();
				bh = bl.Hash();
				if (ah != bh)
					return ah - bh;
			}
		}
		return a.Length() - b.Length();
	}

	static string PrettyPrintEnumString(string str) {
		if (str.Length() == 0)
			return "";
		if (str.Contains("_"))
			str.Replace("_", " ");
		str.ToLower();
		int n = 0;
		while (true) {
			string tmp = str[n];
			tmp.ToUpper();
			str[n] = tmp;
			n = str.IndexOfFrom(n + 1, " ");
			if (n == -1)
				break;
			++n;
		}
		return str;
	}

	static string End(string str, int startIndex) {
		int length = str.Length();
		if (startIndex >= length || startIndex < 0)
			return "";
		return str.Substring(startIndex, length - startIndex);
	}

	static string SubstringFromTo(string str, int start, int end) {
		return str.Substring(start, end - start);
	}

}