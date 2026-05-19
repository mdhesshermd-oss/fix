// Main Test Framework class
// Example Test
// It's recommended to make the tested methods private to reduce the change of them being called by accident
class ClassToTest {

	static bool IsPositive(int number) {
		return number >= 0; // == 0 would be a special case and the number would not be positive. The expected behavior is 0 is positive, so this could throw an error if not implemented like expected.
	}

	[LBTestManager.StartTest(ScriptCaller.Create(IsPositiveTest))]
	private static void IsPositiveTest() { // Make sure the Test Method is a static Method ! If your Tested Method is not static, you need to create a new Instance here first and then call the tests afterwards
		LBTest<bool>.Assert(IsPositive(1), true);
		LBTest<bool>.Assert(IsPositive(0), true);
		LBTest<bool>.Assert(IsPositive(-1), false);
		LBTest<bool>.Assert(IsPositive(-18546), false);
		LBTest<bool>.Assert(IsPositive(int.MAX), true);
		LBTest<bool>.Assert(IsPositive(int.MIN), false);
	}

	string GetSign(int number) {
		if (number == 0)
			return "";
		if (number < 0)
			return "-";
		return "+";
	}

	[LBTestManager.StartTest(ScriptCaller.Create(IsNegativeTest))]
	private static void IsNegativeTest() {
		ClassToTest instance = new ClassToTest();
		LBTest<string>.Assert(instance.GetSign(1), "+");
		LBTest<string>.Assert(instance.GetSign(-1), "-");
		LBTest<string>.Assert(instance.GetSign(0), "");
		LBTest<string>.Assert(instance.GetSign(-57432), "-");
		LBTest<string>.Assert(instance.GetSign(87656), "+");
		LBTest<string>.Assert(instance.GetSign(int.MAX), "+");
		LBTest<string>.Assert(instance.GetSign(int.MIN), "-");
	}

}