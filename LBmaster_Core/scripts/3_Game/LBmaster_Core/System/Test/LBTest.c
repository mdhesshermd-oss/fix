// Basic Class to Test any values for equality with == and inform the test manager and print any error messages.
class LBTest<Class T> {

	static void Assert(T value, T expected, int stackOffset = 0, string extraMessage = "") {
		if (value != expected) {
			LBTestManager.OnError(stackOffset);
			LBTestManager.PrintLine("FAIL " + LBLogger.GetCallingMethodFromStack(stackOffset) + " Expected \"" + expected + "\" got \"" + value + "\"" + extraMessage);
		} else {
			LBTestManager.OnPassed(stackOffset);
			LBTestManager.PrintLine("PASS " + LBLogger.GetCallingMethodFromStack(stackOffset) + " Expected \"" + expected + "\" got \"" + value + "\"" + extraMessage);
		}
	}

	static void AssertNotNull(T value, int stackOffset = 0, string extraMessage = "") {
		if (!value) {
			LBTestManager.OnError(stackOffset);
			LBTestManager.PrintLine("FAIL " + LBLogger.GetCallingMethodFromStack(stackOffset) + " Expected not NULL got \"" + value + "\"" + extraMessage);
		} else {
			LBTestManager.OnPassed(stackOffset);
			LBTestManager.PrintLine("PASS " + LBLogger.GetCallingMethodFromStack(stackOffset) + " Expected not NULL got \"" + value + "\"" + extraMessage);
		}
	}

}
