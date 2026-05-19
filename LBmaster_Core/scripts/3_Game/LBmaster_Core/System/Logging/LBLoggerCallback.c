class LBLoggerCallback : RestCallback {

	override void OnError(int errorCode) {}
	override void OnSuccess(string data, int dataSize) {}
	override void OnTimeout() {}
	override void OnFileCreated(string fileName, int dataSize) {}

}