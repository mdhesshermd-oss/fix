class LBTestLogger {

	static void AssertFatal(string message) {
		if (LBLogger.lastFatal != message) {
			LBTestManager.OnError(0);
			LBTestManager.PrintLine("FAIL " + LBLogger.GetCallingMethodFromStack(0) + " Expected \"" + message + "\" got \"" + LBLogger.lastFatal + "\"");
		} else {
			LBTestManager.OnPassed(0);
			LBTestManager.PrintLine("PASS " + LBLogger.GetCallingMethodFromStack(0) + " Expected \"" + message + "\" got \"" + message + "\"");
		}
		LBLogger.lastFatal = "";
	}

	static void AssertError(string message) {
		if (LBLogger.lastError != message) {
			LBTestManager.OnError(0);
			LBTestManager.PrintLine("FAIL " + LBLogger.GetCallingMethodFromStack(0) + " Expected \"" + message + "\" got \"" + LBLogger.lastError + "\"");
		} else {
			LBTestManager.OnPassed(0);
			LBTestManager.PrintLine("PASS " + LBLogger.GetCallingMethodFromStack(0) + " Expected \"" + message + "\" got \"" + message + "\"");
		}
		LBLogger.lastError = "";
	}

}