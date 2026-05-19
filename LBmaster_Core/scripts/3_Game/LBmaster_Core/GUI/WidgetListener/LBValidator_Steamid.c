class LBValidator_Steamid : LBValidator {

	override bool IsValid(string value) {
		return value.Length() == 17 && value.IndexOf("765611") == 0;
	}

}