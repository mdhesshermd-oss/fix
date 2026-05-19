class LBAPIEndpointHandler {

	string name;
	RestApi api;
	RestContext contextPoll, contextPush;
	int requestId = 0, failedRequestDelay = 0;
	bool restricted = false;

	void LBAPIEndpointHandler(string name_, string urlPoll, string urlPush, bool restricted_, string apiKey, int delay) {
		name = name_;
		restricted = restricted_;
		failedRequestDelay = delay;
		api = GetRestApi();
		if (!api)
			api = CreateRestApi();
		contextPoll = api.GetRestContext(urlPoll);
		if (apiKey != "")
			LBLogger.Debug("Setting API Key", "Core");
		if (apiKey != "")
			contextPoll.SetHeader(apiKey);
		contextPush = api.GetRestContext(urlPush);
		if (apiKey != "")
			contextPush.SetHeader(apiKey);
	}

	void StartRequest() {
		int id = ++requestId;
		LBLogger.Spam("Requesting " + id, "Core");
		contextPoll.POST(new LBAPICallback(this, id), "id=" + id + "&port=" + LBAPIManager.port, "");
	}

	bool SendResponse(LBAPIResponse resp, int id) {
		string json = resp.ToJson();
		LBLogger.Debug("Sending Response " + resp + " for " + id + " Data: " + json, "Core");
		contextPush.POST(new LBAPICallbackEmpty(this), "id=" + id + "&port=" + LBAPIManager.port, json);
		return true;
	}

	void RequestNext(bool immediate = false) {
		if (!immediate && failedRequestDelay > 0)
			g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(StartRequest, failedRequestDelay * 1000);
		else
			StartRequest();
	}

	bool OnDataReceived(string data, int id) {
		if (data == "")
			return false;
		LBLogger.Debug("Received data: " + data, "Core");
		LBAPIPollResponse resp;
		string error;
		if (!LBAPIParser.m_Serial.ReadFromString(resp, data, error)) {
			return SendResponse(LBAPIResponse.OnError(501, "Could not read poll request"), id);
		}
		LBLogger.Debug("Received Request for Route: " + resp.route + " for ID " + id, "Core");
		string dataRaw = "";
		int dataIndex = data.IndexOf("data");
		if (dataIndex != -1) {
			int dataStart = data.IndexOfFrom(dataIndex, ":");
			if (dataStart != -1) {
				int dataEnd = data.LastIndexOf("}");
				if (dataEnd != -1) {
					dataRaw = data.Substring(dataStart + 1, dataEnd - dataStart - 1);
				}
			}
		}
		return SendResponse(LBAPIManager.OnAPIRequest(name, resp.route, dataRaw, restricted), id);
	}

}