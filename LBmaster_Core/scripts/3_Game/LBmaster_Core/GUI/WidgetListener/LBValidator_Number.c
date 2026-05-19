class LBValidator_Number : LBValidator {

	reference int min;
	reference int max;

	override bool IsValid(string value) {
		return true;
	}

	override bool GetColor(bool valid, out int color) {
		return false;
	}

	LBValidator_Number SetMinMax(int min_, int max_) {
		this.min = min_;
		this.max = max_;
		return this;
	}

	override string GetSanitizedValue(string value) {
		if (value == "")
			return "";
		if (value == "-")
			return "-";
		int iVal = value.ToInt();
		if (value == "0-" || value == "-0") {
			if (min < 0)
				return "-1";
			return "0";
		}
		if (iVal == 0 && value.Length() > 8) {
			if (value.IndexOf("-") == 0)
				iVal = min;
			else
				iVal = max;
		}
		if (iVal < min)
			iVal = min;
		if (iVal > max)
			iVal = max;
		return iVal.ToString();
	}

}