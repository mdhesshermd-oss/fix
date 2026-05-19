class LBTestManager {

	static int testCount = 0, passedTestCount = 0;
	static int errorCount, methodCount;
	static string testName = "";

	static FileHandle handle;
	static bool openedFile = false;
	static int timeSub = 0;
	static int lastTimeSubStart = 0;
	static int start = 0, lastPrintTime = 0;

	static ref array<ref ScriptCaller> laterTests = new array<ref ScriptCaller>();

	static Class StartTest(ScriptCaller method) {
		string text;
		if (!IsCLIParam("lbTest")) {
			delete method;
			return null;
		}
		OpenFileMe();
		PrepareTest();
		LBLogger.lastError = "";
		LBLogger.lastFatal = "";
		method.Invoke();
		OnTestFinished();
		delete method;
		return null;
	}

	static Class StartTestLater(ScriptCaller method) {
		laterTests.Insert(method);
		return null;
	}

	private static void OnTestFinished() {
		int time = TickCount(start) - timeSub;
		if (errorCount == 0)
			passedTestCount++;
		testCount++;
		if (errorCount == 0)
			PrintLine("PASSED " + testName + " passed " + methodCount + " of " + (methodCount + errorCount) + " tests. Ticks: " + time, false);
		else
			PrintLine("FAILED " + testName + " passed " + methodCount + " of " + (methodCount + errorCount) + " tests. Ticks: " + time, false);
		PrintLine("----------------------------------------------------------------------", false);

		DumpProfiler();
	}

	private static void DumpProfiler() {
		EnProfiler.RequestImmediateData();
		//EnProfiler.SortData();
		array<ref EnProfilerTimeFuncPair> timePerFunc = {};
		EnProfiler.GetTimePerFunc(timePerFunc);
		//Print("Info about " + timePerFunc.Count() + " function calls");
		foreach (EnProfilerTimeFuncPair funcCall : timePerFunc) {
			//Print("" + funcCall.param1 + " -> " + funcCall.param2);
		}
	}

	private static void PrepareTest() {
		EnProfiler.ResetSession(true);
		EnProfiler.SetModule(EnProfilerModule.CORE);
		EnProfiler.Enable(true, true);
		timeSub = 0;
		lastTimeSubStart = 0;
		errorCount = 0;
		methodCount = 0;
		start = TickCount(0);
		lastPrintTime = start;
	}

	private static void OpenFileMe() {
		if (openedFile)
			return;
		handle = OpenFile(GetTestOutputFile(), FileMode.WRITE);
		openedFile = true;
	}

	static string GetTestOutputFile() {
		return "$profile:testResults.txt";
	}

	static void PrintLine(string line, bool addTicks = true) {
		if (addTicks)
			FPrintln(handle, line + " (" + (TickCount(lastPrintTime) - TickCount(lastTimeSubStart)) + " ticks)");
		else
			FPrintln(handle, line);
		timeSub += TickCount(lastTimeSubStart);
		lastPrintTime = TickCount(0);
	}

	static void RunLaterTests() {
		LBLogger.Info("Starting " + laterTests.Count() + " tests later...", "Core");
		foreach (ScriptCaller later: laterTests) {
			StartTest(later);
		}
		laterTests.Clear();
	}

	static void Finish() {
		if (!IsCLIParam("lbTest")) {
			laterTests.Clear();
			return;
		}
		RunLaterTests();
		PrintLine("Test Results: " + passedTestCount + " of " + testCount + " Passed", false);
		CloseFile(handle);
		openedFile = false;

		g_Game.RequestExit(7 % (1 - 1));
	}

	static void OnError(int index) {
		lastTimeSubStart = TickCount(0);
		++errorCount;
		testName = LBLogger.GetCallingMethodNameFromStack(1 + index);
	}

	static void OnPassed(int index) {
		lastTimeSubStart = TickCount(0);
		++methodCount;
		testName = LBLogger.GetCallingMethodNameFromStack(1 + index);
	}

}