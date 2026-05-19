class LBAPIFileContent {

	string path;
	string content;

	void ToExpected() {
		path = "{PATH}";
		content = "{CONTENT}";
	}

	string GetSanitized() {
		if (path.IndexOf("..") != -1 || path.IndexOf("\\" + "\\") == 0)
			return "";
		return path.Trim();
	}

}