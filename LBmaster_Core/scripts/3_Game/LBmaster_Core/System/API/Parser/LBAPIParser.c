class LBAPIParser<Class T1> : LBAPIParserBase {

	typename TN_T1 = LBStringTools.FixTypename(T1);

	override LBAPIResponse OnRequest(string content) {
		string error;
		T1 var;
		if (!m_Serial.ReadFromString(var, content, error)) {
			Class.CastTo(var, TN_T1.Spawn());
			var.ToExpected();
			string expectedContent;
			m_Serial.WriteToString(var, false, expectedContent);
			return LBAPIResponse.OnError(501, "Failed to parse Json.\nExpected Format: " + expectedContent + "\nError: " + error);
		}
		LBAPIResponse resp = new LBAPIResponse(400, "Not executed");
		caller.Invoke(resp, var);
		return resp;
	}

}
