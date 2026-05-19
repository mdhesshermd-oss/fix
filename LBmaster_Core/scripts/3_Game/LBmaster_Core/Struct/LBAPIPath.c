class LBAPIPath {

	string path;

	void ToExpected() {
		path = "{PATH}";
	}

	string GetSanitized() {
		if (path.IndexOf(":") != -1 || path.IndexOf("..") != -1 || path.IndexOf("\\" + "\\") == 0)
			return "";
		return path.Trim();
	}

	string GetSanitizedProfile() {
		if (path.IndexOf("..") != -1 || path.IndexOf("\\" + "\\") == 0)
			return "";
		return path.Trim();
	}

}