class LBTestMargin<Class T> {

	static void Assert(T value, T expected, T error, int stackOffset = 0, string extraMessage = "") {
		T diff = value - expected;
		if (diff > error || diff < -error) {
			LBTestManager.OnError(stackOffset);
			LBTestManager.PrintLine("FAIL " + LBLogger.GetCallingMethodFromStack(stackOffset) + " Expected \"" + expected + "\" got \"" + value + "\"" + extraMessage);
		} else {
			LBTestManager.OnPassed(stackOffset);
			LBTestManager.PrintLine("PASS " + LBLogger.GetCallingMethodFromStack(stackOffset) + " Expected \"" + expected + "\" got \"" + value + "\"" + extraMessage);
		}
	}

}