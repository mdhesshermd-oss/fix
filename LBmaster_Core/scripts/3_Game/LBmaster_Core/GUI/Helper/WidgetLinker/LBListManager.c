class LBListManager<Class T1> : LBListManagerBase {

	typename TN_T1 = LBStringTools.FixTypename(T1);
	array<ref T1> arr;

	void SetArr(array<ref T1> arr_) {
		this.arr = arr_;
	}

	void LBListManager(int copyIndexStart = 0, int copyIndexEnd = 0) {
		copyStringIndexStart = copyIndexStart;
		copyStringIndexEnd = copyIndexEnd;
	}

	override string GetType() {
		return TN_T1.ToString();
	}

	override void Invalidate() {
		arr = null;
	}

	override bool IsValid() {
		return arr != null;
	}

	override void RemoveEntry(int index) {
		arr.RemoveOrdered(index);
	}

	override void AddEntry(string value, bool doSetup) {
		T1 instance;
		Class.CastTo(instance, TN_T1.Spawn());
		if (doSetup)
			g_Game.GameScript.CallFunctionParams(instance, "Setup", null, new Param1<string>(value));
		arr.Insert(instance);
	}

	override int CountEntries() {
		return arr.Count();
	}

	override string GetEntry(int index, int column) {
		if (!arr || index < 0 || index >= arr.Count())
			return "";
		return arr.Get(index).ToListString(column);
	}

	override Class GetValue(int index) {
		if (index < 0 || index >= arr.Count())
			return null;
		return arr.Get(index);
	}

	override int DuplicateEntry(int index) {
		if (index < 0 || index >= arr.Count())
			return index;
		T1 entry = arr.Get(index);
		string json, error;
		LBAPIParser.m_Serial.WriteToString(entry, false, json);
		T1 copy;
		LBAPIParser.m_Serial.ReadFromString(copy, json, error);
		arr.InsertAt(copy, index + 1);
		return index + 1;
	}

	override string CopyCurrentValue(int index) {
		if (index < 0 || index >= arr.Count())
			return "";
		T1 entry = arr.Get(index);
		string json;
		LBAPIParser.m_Serial.WriteToString(entry, false, json);
		return json;
	}

	override bool PasteCopiedValue(int after) {
		if (copiedValue == "" || copiedValueType != GetType())
			return false;
		string error;
		T1 copy;
		if (!LBAPIParser.m_Serial.ReadFromString(copy, copiedValue, error))
			return false;
		if (after + 1 > arr.Count())
			arr.InsertAt(copy, arr.Count());
		else
			arr.InsertAt(copy, after + 1);
		return true;
	}

	override bool SwapEntries(int index1, int index2) {
		if (index1 < 0 || index2 < 0 || index1 >= arr.Count() || index2 >= arr.Count())
			return false;
		T1 temp = arr.Get(index1);
		arr.Set(index1, arr.Get(index2));
		arr.Set(index2, temp);
		return true;
	}

}