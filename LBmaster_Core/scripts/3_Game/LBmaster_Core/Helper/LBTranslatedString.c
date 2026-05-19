class LBTranslatedString {

	string value;
	ref TStringArray translation;

	void LBTranslatedString(string value_, TStringArray translation_ = null) {
		this.value = value_;
		this.translation = translation_;
	}

	string Get() {
		string message = LBStringTools.MultiTranslateString(value);
		if (translation) {
			for (int i = 0; i < translation.Count(); i += 2) {
				if (message.Contains(translation.Get(i))) {
					message.Replace(translation.Get(i), translation.Get(i + 1));
				}
			}
		}
		if (message.IndexOf("\\n") != -1)
			message.Replace("\\n", "\n");
		return message;
	}

	LBTranslatedString Combine(LBTranslatedString other) {
		if (!other)
			return this;
		string otherValue = "NULL";
		if (other)
			otherValue = other.value;
		string message = value + other.value;
		TStringArray translations = new TStringArray();
		if (translation)
			translations.InsertArray(translation);
		if (other.translation)
			translations.InsertArray(other.translation);
		return LBTranslatedString(message, translations);
	}

}