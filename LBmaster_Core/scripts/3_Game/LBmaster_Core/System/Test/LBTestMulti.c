class LBTestMulti<Class T> {

	static ref TStringArray results = new TStringArray();
	static bool passed = true;

	static void Append(T value, T expected, int stackOffset = 0, string extraMessage = "") {
		if (value != expected) {
			passed = false;
		}
		results.Insert(" Expected \"" + expected + "\" got \"" + value + "\"" + extraMessage);
	}

	static void Assert(T value, T expected, int stackOffset = 0, string extraMessage = "") {
		Append(value, expected, stackOffset + 1, extraMessage);
		string expectedStr = "";
		for (int i = 0; i < results.Count(); ++i) {
			expectedStr = expectedStr + results[i];
		}
		if (!passed) {
			LBTestManager.OnError(stackOffset);
			LBTestManager.PrintLine("FAIL " + LBLogger.GetCallingMethodFromStack(stackOffset) + expectedStr);
		} else {
			LBTestManager.OnPassed(stackOffset);
			LBTestManager.PrintLine("PASS " + LBLogger.GetCallingMethodFromStack(stackOffset) + expectedStr);
		}
		passed = true;
		results.Clear();
	}

}