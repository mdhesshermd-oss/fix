class LBListManagerMapStringBool : LBListManagerBase {

	map<string, bool> arr;

	void SetArr(map<string, bool> arr_) {
		this.arr = arr_;
	}

	override string GetType() {
		return "string";
	}

	override void Invalidate() {
		arr = null;
	}

	override bool IsValid() {
		return arr != null;
	}

	override void RemoveEntry(int index) {
		arr.RemoveElement(index);
	}

	override void AddEntry(string value, bool doSetup) {
		arr.Insert(value, false);
	}

	override int CountEntries() {
		return arr.Count();
	}

	override string GetEntry(int index, int column) {
		if (index < 0 || index >= arr.Count())
			return "";
		return arr.GetKey(index);
	}

	override Class GetValue(int index) {
		return null;
	}

	override int DuplicateEntry(int index) {
		return index;
	}

	override bool SwapEntries(int index1, int index2) {
		return false;
	}

}