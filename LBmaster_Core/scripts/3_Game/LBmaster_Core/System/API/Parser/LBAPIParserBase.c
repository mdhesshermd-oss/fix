class LBAPIParserBase {

	static ref JsonSerializer m_Serial = new JsonSerializer();

	ref ScriptCaller caller;
	bool restricted = false;

	void LBAPIParserBase(ScriptCaller caller_, bool restricted_ = false) {
		this.caller = caller_;
		this.restricted = restricted_;
	}

	LBAPIResponse OnRequest(string content);

	bool IsValid() {
		return caller && caller.IsValid();
	}

}