class LBAPICallbackEmpty : RestCallback {

	LBAPIEndpointHandler handler;

	void LBAPICallbackEmpty(LBAPIEndpointHandler handler_) {
		handler = handler_;
	}

	override void OnError(int errorCode) {
		handler.RequestNext(true);
	}
	override void OnTimeout() {
		handler.RequestNext(true);
	}
	override void OnSuccess(string data, int dataSize) {
		handler.RequestNext(true);
	}
	override void OnFileCreated(string fileName, int dataSize) {
		handler.RequestNext(true);
	}

}