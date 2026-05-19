// Class to Test arrays for equality. Will first check the size of both arrays. If they don't match, throw an error and then check each element for equality with ==
class LBTestArray<Class T> {

	static void Assert(array<T> value, array<T> expected, int stackOffset = 0, string extraMessage = "") {
		if (value.Count() != expected.Count()) {
			LBTestManager.OnError(stackOffset);
			LBTestManager.PrintLine("FAIL " + LBLogger.GetCallingMethodFromStack(stackOffset) + " Array Size wrong ! Expected \"" + expected.Count() + "\" got \"" + value.Count() + "\"" + extraMessage);
			return;
		}
		string errorStr = "";
		for (int i = 0; i < value.Count(); ++i) {
			if (value[i] != expected[i]) {
				errorStr = errorStr + " (i: " + i + " E: " + expected[i] + " V: " + value[i] + ")";
			}
		}
		if (errorStr != "") {
			LBTestManager.OnError(stackOffset);
			LBTestManager.PrintLine("FAIL " + LBLogger.GetCallingMethodFromStack(stackOffset) + " Arrays missmatch at" + errorStr + extraMessage);
		} else {
			LBTestManager.OnPassed(stackOffset);
			LBTestManager.PrintLine("PASS " + LBLogger.GetCallingMethodFromStack(stackOffset) + " Expected \"" + expected + "\" got \"" + value + "\" Match" + extraMessage);
		}
	}

}