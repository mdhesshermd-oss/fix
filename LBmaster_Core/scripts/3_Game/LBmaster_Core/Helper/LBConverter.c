class LBConverter {

	// Test START
	[LBTestManager.StartTest(ScriptCaller.Create(ARGBToComponentsTest))]
	private static void ARGBToComponentsTest() {
		TestColor(0, 0, 0, 0, 0);
		TestColor(ARGB(255, 255, 255, 255), 255, 255, 255, 255);
		TestColor(ARGB(255, 123, 255, 255), 255, 123, 255, 255);
		TestColor(ARGB(0, 123, 255, 255), 0, 123, 255, 255);
		TestColor(ARGB(0, 123, 255, 34), 0, 123, 255, 34);
		TestColor(ARGB(0, 123, 5, 34), 0, 123, 5, 34);
		TestColor(5465456, 0, 83, 101, 112);
	}

	[LBTestManager.StartTest(ScriptCaller.Create(MixColorsTest))]
	private static void MixColorsTest() {
		LBTest<int>.Assert(MixColors(ARGB(255, 255, 255, 255), ARGB(255, 255, 255, 255), 0.55), ARGB(255, 255, 255, 255));
		LBTest<int>.Assert(MixColors(ARGB(255, 255, 255, 255), ARGB(255, 255, 255, 255), 1), ARGB(255, 255, 255, 255));
		LBTest<int>.Assert(MixColors(ARGB(101, 150, 0, 230), ARGB(101, 150, 0, 230), 0.45), ARGB(101, 150, 0, 230));
		LBTest<int>.Assert(MixColors(ARGB(101, 150, 0, 230), ARGB(101, 150, 0, 230), 0), ARGB(101, 150, 0, 230));
		LBTest<int>.Assert(MixColors(ARGB(101, 150, 0, 230), ARGB(255, 255, 255, 255), 1), ARGB(101, 150, 0, 230));
		LBTest<int>.Assert(MixColors(ARGB(30, 50, 150, 60), ARGB(180, 93, 240, 60), 0), ARGB(180, 93, 240, 60));
		LBTest<int>.Assert(MixColors(ARGB(30, 50, 150, 60), ARGB(180, 93, 240, 60), 0.2), ARGB(150, 84, 222, 60));
		LBTest<int>.Assert(MixColors(ARGB(30, 50, 40, 80), ARGB(180, 93, 76, 60), 0.66), ARGB(81, 64, 52, 73));
	}

	[LBTestManager.StartTest(ScriptCaller.Create(GetHighestBitTest))]
	private static void GetHighestBitTest() {
		LBTest<int>.Assert(GetHighestBit(0), 0);
		LBTest<int>.Assert(GetHighestBit(1), 1);
		LBTest<int>.Assert(GetHighestBit(2), 2);
		LBTest<int>.Assert(GetHighestBit(3), 2);
		LBTest<int>.Assert(GetHighestBit(15), 4);
		LBTest<int>.Assert(GetHighestBit(16), 5);
		LBTest<int>.Assert(GetHighestBit(1902), 11);
		LBTest<int>.Assert(GetHighestBit(-1), 32);
		LBTest<int>.Assert(GetHighestBit(-15155555), 32);
		LBTest<int>.Assert(GetHighestBit(65535), 16);
		LBTest<int>.Assert(GetHighestBit(65536), 17);
	}

	[LBTestManager.StartTest(ScriptCaller.Create(GetBitMaskTest))]
	private static void GetBitMaskTest() {
		LBTest<int>.Assert(GetBitMask(0), 0);
		LBTest<int>.Assert(GetBitMask(1), 1);
		LBTest<int>.Assert(GetBitMask(2), 3);
		LBTest<int>.Assert(GetBitMask(3), 7);
		LBTest<int>.Assert(GetBitMask(15), 32767);
		LBTest<int>.Assert(GetBitMask(31), 2147483647);
		LBTest<int>.Assert(GetBitMask(32), -1);
		LBTest<int>.Assert(GetBitMask(50), 0);
	}

	[LBTestManager.StartTest(ScriptCaller.Create(IntToHex1Test))]
	private static void IntToHex1Test() {
		LBTest<string>.Assert(IntToHex1(0), "0");
		LBTest<string>.Assert(IntToHex1(-1), "");
		LBTest<string>.Assert(IntToHex1(-156466), "");
		LBTest<string>.Assert(IntToHex1(5), "5");
		LBTest<string>.Assert(IntToHex1(10), "A");
		LBTest<string>.Assert(IntToHex1(15), "F");
		LBTest<string>.Assert(IntToHex1(16), "");
	}

	[LBTestManager.StartTest(ScriptCaller.Create(IntToHex2Test))]
	private static void IntToHex2Test() {
		LBTest<string>.Assert(IntToHex2(-1), "0");
		LBTest<string>.Assert(IntToHex2(0), "00");
		LBTest<string>.Assert(IntToHex2(5), "05");
		LBTest<string>.Assert(IntToHex2(16), "10");
		LBTest<string>.Assert(IntToHex2(64), "40");
		LBTest<string>.Assert(IntToHex2(240), "F0");
		LBTest<string>.Assert(IntToHex2(255), "FF");
		LBTest<string>.Assert(IntToHex2(256), "0");

	}

	[LBTestManager.StartTest(ScriptCaller.Create(ParseHexTest))]
	private static void ParseHexTest() {
		LBTest<int>.Assert(ParseHex(""), 0);
		LBTest<int>.Assert(ParseHex("1"), 1);
		LBTest<int>.Assert(ParseHex("#"), 0);
		LBTest<int>.Assert(ParseHex("#abc"), 0xabc);
		LBTest<int>.Assert(ParseHex("#F"), 0xf);
		LBTest<int>.Assert(ParseHex("ff556677"), 0xff556677);
		LBTest<int>.Assert(ParseHex("FF55ab77"), 0xFF55ab77);
	}

	[LBTestManager.StartTest(ScriptCaller.Create(ParseHex1Test))]
	private static void ParseHex1Test() {
		LBTest<int>.Assert(ParseHex1("2"), 2);
		LBTest<int>.Assert(ParseHex1("1"), 1);
		LBTest<int>.Assert(ParseHex1("a"), 10);
		LBTest<int>.Assert(ParseHex1("F"), 15);
		LBTest<int>.Assert(ParseHex1("b"), 11);
	}

	[LBTestManager.StartTest(ScriptCaller.Create(ShiftRightLogicTest))]
	private static void ShiftRightLogicTest() {
		LBTest<int>.Assert(ShiftRightLogic(-1, 1), 0x7fffffff);
		LBTest<int>.Assert(ShiftRightLogic(0, 0), 0);
		LBTest<int>.Assert(ShiftRightLogic(20, 0), 20);
		LBTest<int>.Assert(ShiftRightLogic(-50, 2), 0x3ffffff3);
		LBTest<int>.Assert(ShiftRightLogic(0x80000000, 1), 0x40000000);
		LBTest<int>.Assert(ShiftRightLogic(0x80000000, 4), 0x08000000);
		LBTest<int>.Assert(ShiftRightLogic(0x80000000, 5), 0x04000000);
		LBTest<int>.Assert(ShiftRightLogic(0x80000000, 16), 0x00008000);
		LBTest<int>.Assert(ShiftRightLogic(0x80000000, 64), 0x80000000);
		LBTest<int>.Assert(ShiftRightLogic(0x80000000, -64), 0x80000000);
		LBTest<int>.Assert(ShiftRightLogic(0x80000000, -63), 0x40000000);
		LBTest<int>.Assert(ShiftRightLogic(0x80000000, -33), 0x1);
		LBTest<int>.Assert(ShiftRightLogic(0x80000000, -1), 0x1);
	}

	static string TypenameToString(typename tn) {
		return tn.ToString();
	}

	private static void TestColor(int color, int ae, int re, int ge, int be) { // Helper method
		int a, r, g, b;
		ARGBToComponents(color, a, r, g, b);
		LBTestMulti<int>.Append(a, ae, 1, " Alpha");
		LBTestMulti<int>.Append(r, re, 1, " Red");
		LBTestMulti<int>.Append(g, ge, 1, " Green");
		LBTestMulti<int>.Assert(b, be, 1, " Blue");
	}
	// Test END

	static void ARGBToComponents(int argb, out int a, out int r, out int g, out int b) {
		a = (argb >> 24) & 0xff;
		r = (argb >> 16) & 0xff;
		g = (argb >> 8) & 0xff;
		b = (argb) & 0xff;
	}

	static int MixColors(int colorA, int colorB, float partA) {
		float partB = 1.0 - partA;
		int a = ((colorA >> 24) & 0xFF) * partA + ((colorB >> 24) & 0xFF) * partB;
		int r = ((colorA >> 16) & 0xFF) * partA + ((colorB >> 16) & 0xFF) * partB;
		int g = ((colorA >> 8) & 0xFF) * partA + ((colorB >> 8) & 0xFF) * partB;
		int b = (colorA & 0xFF) * partA + (colorB & 0xFF) * partB;
		return ARGB(a, r, g, b);
	}

	static string IntToHex2(int val) {
		return IntToHex1(val / 16) + IntToHex1(val % 16);
	}

	static int ParseHex(string str) {
		if (str.Length() == 0)
			return 0;
		int val = 0;
		if (str[0] == "#")
			str = str.Substring(1, str.Length() - 1);
		int factor = 1;
		for (int i = 7; i >= 0; --i) {
			if (str.Length() <= i)
				continue;

			val += ParseHex1(str[i]) * factor;
			factor = factor << 4;
		}
		return val;
	}

	static int ParseHex1(string oneLenString) {
		int hash = oneLenString.Hash();
		if (hash < 58)
			hash -= 48;
		else if (hash < 71)
			hash -= 55;
		else if (hash < 103)
			hash -= 87;
		return hash;
	}

	static string IntToHex1(int val) {
		if (val >= 0 && val < 10)
			return "" + val;
		switch (val) {
			case 10:
				return "A";
			case 11:
				return "B";
			case 12:
				return "C";
			case 13:
				return "D";
			case 14:
				return "E";
			case 15:
				return "F";
		}
		return "";
	}

	static int ShiftRightLogic(int val, int amount) {
		amount &= 31;
		if (amount == 0)
			return val;
		return ((val >> 1) & 0x7fffffff) >> (amount - 1);
	}

	static int GetBitMask(int bits) {
		switch (bits) {
			case 0:
				return 0x00000000;
			case 1:
				return 0x00000001;
			case 2:
				return 0x00000003;
			case 3:
				return 0x00000007;
			case 4:
				return 0x0000000f;
			case 5:
				return 0x0000001f;
			case 6:
				return 0x0000003f;
			case 7:
				return 0x0000007f;
			case 8:
				return 0x000000ff;
			case 9:
				return 0x000001ff;
			case 10:
				return 0x000003ff;
			case 11:
				return 0x000007ff;
			case 12:
				return 0x00000fff;
			case 13:
				return 0x00001fff;
			case 14:
				return 0x00003fff;
			case 15:
				return 0x00007fff;
			case 16:
				return 0x0000ffff;
			case 17:
				return 0x0001ffff;
			case 18:
				return 0x0003ffff;
			case 19:
				return 0x0007ffff;
			case 20:
				return 0x000fffff;
			case 21:
				return 0x001fffff;
			case 22:
				return 0x003fffff;
			case 23:
				return 0x007fffff;
			case 24:
				return 0x00ffffff;
			case 25:
				return 0x01ffffff;
			case 26:
				return 0x03ffffff;
			case 27:
				return 0x07ffffff;
			case 28:
				return 0x0fffffff;
			case 29:
				return 0x1fffffff;
			case 30:
				return 0x3fffffff;
			case 31:
				return 0x7fffffff;
			case 32:
				return 0xffffffff;
		}
		return 0;
	}

	static int GetHighestBit(int number) {
		int index = 0;
		while (number != 0) {
			++index;
			number >>= 1;
			number &= 0x7fffffff;
		}
		return index;
	}

}
