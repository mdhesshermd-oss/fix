class LBMath {

	[LBTestManager.StartTest(ScriptCaller.Create(TestMax))]
	static void TestMax() {
		LBTest<int>.Assert(Max(10, 50), 50);
		LBTest<int>.Assert(Max(50, 10), 50);
		LBTest<int>.Assert(Max(0, 0), 0);
		LBTest<int>.Assert(Max(-1, 0), 0);
		LBTest<int>.Assert(Max(-10000000, 1000), 1000);
		LBTest<int>.Assert(Max(int.MIN, int.MAX), int.MAX);
		LBTest<int>.Assert(Max(int.MAX, int.MIN), int.MAX);
	}

	[LBTestManager.StartTest(ScriptCaller.Create(TestMin))]
	static void TestMin() {
		LBTest<int>.Assert(Min(10, 50), 10);
		LBTest<int>.Assert(Min(50, 10), 10);
		LBTest<int>.Assert(Min(0, 0), 0);
		LBTest<int>.Assert(Min(-1, 0), -1);
		LBTest<int>.Assert(Min(-10000000, 1000), -10000000);
		LBTest<int>.Assert(Min(int.MIN, int.MAX), int.MIN);
		LBTest<int>.Assert(Min(int.MAX, int.MIN), int.MIN);
	}

	[LBTestManager.StartTest(ScriptCaller.Create(TestIntDivCeiled))]
	static void TestIntDivCeiled() {
		LBTest<int>.Assert(IntDivCeiled(10, 50), 1);
		LBTest<int>.Assert(IntDivCeiled(50, 10), 5);
		LBTest<int>.Assert(IntDivCeiled(51, 10), 6);
		LBTest<int>.Assert(IntDivCeiled(0, 10), 0);
		LBTest<int>.Assert(IntDivCeiled(1, 10), 1);
		LBTest<int>.Assert(IntDivCeiled(1, 1), 1);
		LBTest<int>.Assert(IntDivCeiled(2, 1), 2);
		LBTest<int>.Assert(IntDivCeiled(3, 2), 2);
	}

	static int IntDivCeiled(int value, int div) {
		int result = value / div;
		if (result * div < value)
			return result + 1;
		return result;
	}

	static int Max(int a, int b) {
		if (a > b)
			return a;
		return b;
	}

	static int Min(int a, int b) {
		if (a < b)
			return a;
		return b;
	}

	static int GetBit(int val, int at) {
		return LBConverter.ShiftRightLogic(val, at) & 1;
	}

}