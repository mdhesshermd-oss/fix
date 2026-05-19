class LB_QrSegment {

	int numChars;
	ref array<int> data;
	int bitLength;

	static LB_QrSegment makeBytes(array<int> inputdata) {
		if (inputdata.Count() * 8 > int.MAX)
			Error("Data too long");
		array<int> bits = new array<int>();
		LB_ReedSolomonGenerator.ResizeArray(bits, (inputdata.Count() + 3) / 4);
		for (int i = 0; i < inputdata.Count(); i++) {
			int index = LBConverter.ShiftRightLogic(i, 2);
			bits[index] = bits[index] | ((inputdata[i] & 0xFF) << (~i << 3));
		}
		return new LB_QrSegment(inputdata.Count(), bits, inputdata.Count() * 8);
	}

	void LB_QrSegment(int numCh, array<int> inputdata, int bitLen) {
		this.data = inputdata;
		if (numCh < 0 || bitLen < 0 || bitLen > data.Count() * 32) {
			Error("Invalid value");
			return;
		}
		numChars = numCh;
		bitLength = bitLen;
	}

	static int getTotalBits(notnull LB_QrSegment seg, int version) {
		int result = 0;
		int ccbits = numCharCountBits(version);
		if (seg.numChars >= (1 << ccbits))
			return -1; // The segment's length doesn't fit the field's bit width
		result += 4 + ccbits + seg.bitLength;
		if (result > int.MAX)
			return -1; // The sum will overflow an int type
		return (int)result;
	}

	static int numCharCountBits(int ver) {
		if (!(LB_QrCode.MIN_VERSION <= ver && ver <= LB_QrCode.MAX_VERSION))
			Error("Unexpected error in numCharCountBits");
		if ((ver + 7) / 17 == 0)
			return 8;
		return 16;
	}

}