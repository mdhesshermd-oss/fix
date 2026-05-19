class LBValidator_Float : LBValidator {

	reference float min;
	reference float max;

	override bool IsValid(string value) {
		return true;
	}

	override bool GetColor(bool valid, out int color) {
		return false;
	}

	override string GetSanitizedValue(string value) {
		float fVal = value.ToFloat();
		string endDot = "";
		if (value.LastIndexOf(".") == value.Length() - 1 || value.LastIndexOf(",") == value.Length() - 1)
			endDot = ".";
		if (value == ".")
			return "0.";
		if (value == "")
			return "";
		if (value == "-")
			return "-";
		if (value == "0-" || value == "-0") {
			if (min < 0)
				return "-" + endDot;
			return "0" + endDot;
		}
		if (fVal < min)
			fVal = min;
		if (fVal > max)
			fVal = max;
		return fVal.ToString() + endDot;
	}

}