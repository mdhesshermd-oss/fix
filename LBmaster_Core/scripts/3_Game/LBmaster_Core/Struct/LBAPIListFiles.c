class LBAPIListFiles {

	string path;
	string pattern;

	void ToExpected() {
		path = "{PATH}";
	}

	string GetPattern() {
		if (pattern.IndexOf("..") != -1)
			return "";
		return pattern.Trim();
	}

	string GetSanitized() {
		if (path.IndexOf("..") != -1 || path.IndexOf("\\" + "\\") == 0)
			return "";
		return path.Trim();
	}

}