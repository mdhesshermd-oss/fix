class LBListManagerString : LBListManagerBase {

	TStringArray arr;

	void SetArr(TStringArray arr_) {
		this.arr = arr_;
	}

	override string GetType() {
		return "string";
	}

	void LBListManagerString(int copyIndexStart = 0, int copyIndexEnd = 0) {
		copyStringIndexStart = copyIndexStart;
		copyStringIndexEnd = copyIndexEnd;
	}

	override void Invalidate() {
		arr = null;
	}

	override bool IsValid() {
		return arr != null;
	}

	override void RemoveEntry(int index) {
		if (arr)
			arr.RemoveOrdered(index);
	}

	override void AddEntry(string value, bool doSetup) {
		if (arr)
			arr.Insert(value);
	}

	override int CountEntries() {
		if (!arr)
			return 0;
		return arr.Count();
	}

	override string GetEntry(int index, int column) {
		if (!arr || index < 0 || index >= arr.Count())
			return "";
		return arr.Get(index);
	}

	override Class GetValue(int index) {
		if (index < 0 || index >= arr.Count())
			return null;
		return null;
	}

	override int DuplicateEntry(int index) {
		if (!arr || index < 0 || index >= arr.Count())
			return index;
		string entry = arr.Get(index);
		arr.InsertAt(entry, index + 1);
		return index + 1;
	}

	override string CopyCurrentValue(int index) {
		if (index < 0 || index >= arr.Count())
			return "";
		return arr.Get(index);
	}

	override bool PasteCopiedValue(int after) {
		if (copiedValue == "" || copiedValueType != GetType())
			return false;
		if (after + 1 > arr.Count())
			arr.InsertAt(copiedValue, arr.Count());
		else
			arr.InsertAt(copiedValue, after + 1);
		return true;
	}

	override bool SwapEntries(int index1, int index2) {
		if (!arr || index1 < 0 || index2 < 0 || index1 >= arr.Count() || index2 >= arr.Count())
			return false;
		string temp = arr.Get(index1);
		arr.Set(index1, arr.Get(index2));
		arr.Set(index2, temp);
		return true;
	}

}