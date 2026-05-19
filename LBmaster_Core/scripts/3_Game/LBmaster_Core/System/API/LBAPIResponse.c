class LBAPIResponse {

	private int code;
	private string message;
	[NonSerialized()]
	private string data;

	void LBAPIResponse(int code_, string message_) {
		this.code = code_;
		this.message = message_;
	}

	LBAPIResponse SetData(string data_) {
		this.data = data_;
		return this;
	}

	void SetOK() {
		Set(200, "OK");
	}

	void SetNotFound() {
		Set(404, "Not Found");
	}

	void Set(int code_, string message_) {
		this.code = code_;
		this.message = message_;
	}

	string ToJson() {
		string content;
		LBAPIResponse resp = this;
		LBAPIParserBase.m_Serial.WriteToString(resp, false, content);
		if (data != "") {
			content = content.Substring(0, content.LastIndexOf("}"));
			content = content + ",\"data\":" + data + "}";
		}
		return content;
	}

	static LBAPIResponse AccessDenied() {
		return new LBAPIResponse(403, "Forbidden");
	}

	static LBAPIResponse OnError(int code_, string message_) {
		return new LBAPIResponse(code_, message_);
	}

	static LBAPIResponse NotFound(string path_) {
		return new LBAPIResponse(404, path_);
	}

	int GetCode() {
		return code;
	}

	string GetMessage() {
		return message;
	}

}