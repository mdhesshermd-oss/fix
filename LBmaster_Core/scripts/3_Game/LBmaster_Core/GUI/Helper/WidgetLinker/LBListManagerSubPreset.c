class LBListManagerSubPreset : LBListManagerString {

	override string GetEntry(int index, int column) {
		if (!arr || index < 0 || index >= arr.Count())
			return "";
		string val = arr.Get(index);
		int splitIndex = val.LastIndexOf("*");
		if (splitIndex == -1) {
			if (column == 0)
				return val;
			return "x1.0";
		} else {
			if (column == 0)
				return val.Substring(0, splitIndex);
			return "x" + LBStringTools.End(val, splitIndex + 1).ToFloat();
		}
		return "";
	}

}