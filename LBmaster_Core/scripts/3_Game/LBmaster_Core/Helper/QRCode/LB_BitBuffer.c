class LB_BitBuffer {
	ref array<int> data = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	int bitLength = 0;

	void appendBits(int val, int len) {
		if (len < 0 || len > 31 || LBConverter.ShiftRightLogic(val, len) != 0)
			Error("Value out of range");
		if (len > int.MAX - bitLength)
			Error("Maximum length reached");

		if (bitLength + len + 1 > data.Count() << 5)
			data.Resize(data.Count() * 2);
		if (!(bitLength + len <= data.Count() << 5))
			Error("Unexpected Error in appendBits");

		int remain = 32 - (bitLength & 0x1F);
		if (remain < len) {
			int index = LBConverter.ShiftRightLogic(bitLength, 5);
			data[index] = data[index] | LBConverter.ShiftRightLogic(val, len - remain);
			bitLength += remain;
			if (!((bitLength & 0x1F) == 0))
				Error("Unexpected Error in appendBits");
			len -= remain;
			val &= (1 << len) - 1;
			remain = 32;
		}
		index = LBConverter.ShiftRightLogic(bitLength, 5);
		data[index] = data[index] | (val << (remain - len));
		bitLength += len;
	}

	void appendBits(array<int> vals, int len) {
		if (len == 0)
			return;
		if (len < 0 || len > vals.Count() * 32)
			Error("Value out of range");
		int wholeWords = len / 32;
		int tailBits = len % 32;
		if (tailBits > 0 && vals[wholeWords] << tailBits != 0)
			Error("Last word must have low bits clear");
		if (len > int.MAX - bitLength)
			Error("Maximum length reached");

		while (bitLength + len > data.Count() * 32)
			data.Resize(data.Count() * 2);

		int shift = bitLength % 32;
		if (shift == 0) {
			int copylen = (len + 31) / 32;
			int copydst = bitLength / 32;
			for (int i = 0; i < copylen; ++i) {
				data.Set(i + copydst, vals.Get(i));
			}
			bitLength += len;
		} else {
			for (i = 0; i < wholeWords; i++) {
				int word = vals[i];
				int index = LBConverter.ShiftRightLogic(bitLength, 5);
				data[index] = data[index] | LBConverter.ShiftRightLogic(word, shift);
				bitLength += 32;
				index = LBConverter.ShiftRightLogic(bitLength, 5);
				data[index] = word << (32 - shift);
			}
			if (tailBits > 0)
				appendBits(LBConverter.ShiftRightLogic(vals[wholeWords], (32 - tailBits)), tailBits);
		}
	}

	array<int> getBytes() {
		if (bitLength % 8 != 0)
			Error("IllegalStateException");
		array<int> result = new array<int>();
		LB_ReedSolomonGenerator.ResizeArray(result, bitLength / 8);
		for (int i = 0; i < result.Count(); ++i) {
			result[i] = LBConverter.ShiftRightLogic(data[LBConverter.ShiftRightLogic(i, 2)], ~i << 3) & 0xff;
		}
		return result;
	}

	int getBitLength() {
		return bitLength;
	}
}
