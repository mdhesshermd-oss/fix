class LBAPIParserRaw : LBAPIParserBase {

	override LBAPIResponse OnRequest(string content) {
		LBAPIResponse resp = new LBAPIResponse(400, "Not executed");
		caller.Invoke(resp, content);
		return resp;
	}

}