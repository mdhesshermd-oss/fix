class LBListManagerBase {

	static string copiedValue;
	static string copiedValueType;

	int copyStringIndexStart = 0;
	int copyStringIndexEnd = 0;

	void RemoveEntry(int index);
	void AddEntry(string value, bool doSetup);
	int CountEntries();
	string GetEntry(int index, int column);
	Class GetValue(int index);
	bool IsValid();
	void Invalidate();
	int DuplicateEntry(int index);
	bool SwapEntries(int index1, int index2);
	bool IsSearched(int index, string search, array<Widget> extraWidgets);
	string CopyCurrentValue(int index);
	bool PasteCopiedValue(int after);
	string GetCopyString(int index) {
		copiedValue = CopyCurrentValue(index);
		copiedValueType = GetType();
		if (copyStringIndexStart == copyStringIndexEnd)
			return GetEntry(index, copyStringIndexStart);
		if (copyStringIndexStart > copyStringIndexEnd)
			return "";
		string output = GetEntry(index, copyStringIndexStart);
		for (int i = copyStringIndexStart + 1; i <= copyStringIndexEnd; ++i) {
			output = output + ", " + GetEntry(index, i);
		}
		return output;
	}
	string GetType();

}