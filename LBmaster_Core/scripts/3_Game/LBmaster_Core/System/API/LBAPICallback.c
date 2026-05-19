class LBAPICallback : RestCallback {

	LBAPIEndpointHandler handler;
	int requestId;

	void LBAPICallback(LBAPIEndpointHandler handler_, int requestId_) {
		this.handler = handler_;
		this.requestId = requestId_;
	}

	override void OnError(int errorCode) {
		handler.RequestNext();
	}

	override void OnTimeout() {
		handler.RequestNext();
	}

	override void OnSuccess(string data, int dataSize) {
		if (data == "" || data[0] != "{") {
			handler.RequestNext();
			return;
		}
		int start = TickCount(0);
		bool ok = handler.OnDataReceived(data, requestId);
		int time = TickCount(start);
		if (!ok)
			handler.RequestNext(true);
		LBLogger.Verbose("Answered Request for " + handler.name + " in " + (time / 10) + "us", "Core");
	}

	override void OnFileCreated(string fileName, int dataSize) {
		handler.RequestNext();
	}

}