class LB_QrCode {

	static const int MIN_VERSION = 1;
	static const int MAX_VERSION = 40;
	static const ref TIntArray ECC_CODEWORDS_PER_BLOCK = {-1, 7, 10, 15, 20, 26, 18, 20, 24, 30, 18, 20, 24, 26, 30, 22, 24, 28, 30, 28, 28, 28, 28, 30, 30, 26, 28, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30}; // Low

	static const ref TIntArray NUM_ERROR_CORRECTION_BLOCKS = {-1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 4, 4, 4, 4, 4, 6, 6, 6, 6, 7, 8, 8, 9, 9, 10, 12, 12, 12, 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 24, 25}; // Low

	int version;
	int size;
	ref array<int> modules;

	static LB_QrCode encodeText(string text) {
		array<int> textBytes = new array<int>();
		int textLen = text.Length();
		LB_ReedSolomonGenerator.ResizeArray(textBytes, textLen);
		for (int i = 0; i < textLen; ++i) {
			textBytes[i] = text.Substring(i, 1).Hash();
		}
		LB_QrSegment seg = LB_QrSegment.makeBytes(textBytes);

		// Find the minimal version number to use
		int ver, dataUsedBits;
		for (ver = MIN_VERSION; true; ver++) {
			int dataCapacityBits = getNumDataCodewords(ver) * 8; // Number of data bits available
			dataUsedBits = LB_QrSegment.getTotalBits(seg, ver);
			if (dataUsedBits != -1 && dataUsedBits <= dataCapacityBits)
				break; // This version number is found to be suitable
			if (ver >= MAX_VERSION) { // All versions in the range could not fit the given data
				string msg = "Segment too long";
				if (dataUsedBits != -1)
					msg = string.Format("Data length = %d bits, Max capacity = %d bits", dataUsedBits, dataCapacityBits);
				Error(msg);
			}
		}

		// Concatenate all segments to create the data bit string
		LB_BitBuffer bb = new LB_BitBuffer();
		bb.appendBits(0x4, 4);
		bb.appendBits(seg.numChars, LB_QrSegment.numCharCountBits(ver));
		bb.appendBits(seg.data, seg.bitLength);
		if (!(bb.bitLength == dataUsedBits))
			Error("Unpexpected Error in encodeText");

		// Add terminator and pad up to a byte if applicable
		dataCapacityBits = getNumDataCodewords(ver) * 8;
		if (!(bb.bitLength <= dataCapacityBits))
			Error("Unpexpected Error in encodeText");
		bb.appendBits(0, LBMath.Min(4, dataCapacityBits - bb.bitLength));
		bb.appendBits(0, (8 - bb.bitLength % 8) % 8);
		if (!(bb.bitLength % 8 == 0))
			Error("Unpexpected Error in encodeText");

		// Pad with alternating bytes until data capacity is reached
		for (int padByte = 0xEC; bb.bitLength < dataCapacityBits; padByte ^= 0xEC ^ 0x11)
			bb.appendBits(padByte, 8);
		// Create the QR Code object
		return new LB_QrCode(ver, bb.getBytes());
	}

	void LB_QrCode(int ver, array<int> dataCodewords) {
		// Check arguments and initialize fields
		if (ver < MIN_VERSION || ver > MAX_VERSION)
			Error("Version value out of range");
		version = ver;
		size = ver * 4 + 17;

		LB_QrTemplate tpl = new LB_QrTemplate(ver);
		modules = new array<int>();
		modules.InsertAll(tpl.template);

		// Compute ECC, draw modules, do masking
		array<int> allCodewords = addEccAndInterleave(dataCodewords);
		drawCodewords(tpl.outputBitIndexes, allCodewords);
		handleConstructorMasking(tpl.mask);
	}

	bool getModule(int x, int y) {
		if (0 <= x && x < size && 0 <= y && y < size) {
			int i = y * size + x;
			return LBMath.GetBit(modules[LBConverter.ShiftRightLogic(i, 5)], i) != 0;
		} else
			return false;
	}

	void drawFormatBits() {
		// Calculate error correction code and pack bits
		int data = 1 << 3; // errCorrLvl is uint2, mask is uint3
		int rem = data;
		for (int i = 0; i < 10; i++)
			rem = (rem << 1) ^ (LBConverter.ShiftRightLogic(rem, 9) * 0x537);
		int bits = (data << 10 | rem) ^ 0x5412; // uint15

		// Draw first copy
		for (i = 0; i <= 5; i++)
			setModule(8, i, LBMath.GetBit(bits, i));
		setModule(8, 7, LBMath.GetBit(bits, 6));
		setModule(8, 8, LBMath.GetBit(bits, 7));
		setModule(7, 8, LBMath.GetBit(bits, 8));
		for (i = 9; i < 15; i++)
			setModule(14 - i, 8, LBMath.GetBit(bits, i));

		// Draw second copy
		for (i = 0; i < 8; i++)
			setModule(size - 1 - i, 8, LBMath.GetBit(bits, i));
		for (i = 8; i < 15; i++)
			setModule(8, size - 15 + i, LBMath.GetBit(bits, i));
		setModule(8, size - 8, 1); // Always dark
	}

	void setModule(int x, int y, int dark) {
		if (!(0 <= x && x < size))
			Error("Unexpected Error in setModule");
		if (!(0 <= y && y < size))
			Error("Unexpected Error in setModule");
		if (!(dark == 0 || dark == 1))
			Error("Unexpected Error in setModule");
		int i = y * size + x;
		int index = LBConverter.ShiftRightLogic(i, 5);
		modules[index] = modules[index] & (~(1 << i));
		modules[index] = modules[index] | (dark << i);
	}

	array<int> addEccAndInterleave(notnull array<int> data) {
		if (data.Count() != getNumDataCodewords(version))
			Error("Unpexpected error in addEccAndInterleave");

		// Calculate parameter numbers
		int numBlocks = NUM_ERROR_CORRECTION_BLOCKS[version];
		int blockEccLen = ECC_CODEWORDS_PER_BLOCK[version];
		int rawCodewords = LB_QrTemplate.getNumRawDataModules(version) / 8;
		int numShortBlocks = numBlocks - rawCodewords % numBlocks;
		int shortBlockDataLen = rawCodewords / numBlocks - blockEccLen;

		// Split data into blocks, calculate ECC, and interleave
		// (not concatenate) the bytes into a single sequence
		array<int> result = new array<int>();
		LB_ReedSolomonGenerator.ResizeArray(result, rawCodewords);
		LB_ReedSolomonGenerator rs = new LB_ReedSolomonGenerator(blockEccLen);
		array<int> ecc = new array<int>();
		LB_ReedSolomonGenerator.ResizeArray(ecc, blockEccLen); // Temporary storage per iteration
		for (int i = 0, k = 0; i < numBlocks; i++) {
			int datLen = shortBlockDataLen + LBT<int>.T(i < numShortBlocks, 0, 1);
			rs.getRemainder(data, k, datLen, ecc);
			int l = i;
			for (int j = 0; j < datLen; j++) { // Copy data
				if (j == shortBlockDataLen)
					l -= numShortBlocks;
				result[l] = data[k];
				l += numBlocks;
				k++;
			}
			l = data.Count() + i;
			for (j = 0; j < blockEccLen; j++) { // Copy ECC
				result[l] = ecc[j];
				l += numBlocks;
			}
		}
		return result;
	}

	void drawCodewords(notnull array<int> dataOutputBitIndexes, notnull array<int> allCodewords) {
		if (allCodewords.Count() * 8 != dataOutputBitIndexes.Count())
			Error("Unexpected Error in drawCodewords. " + (allCodewords.Count() * 8) + " != " + dataOutputBitIndexes.Count());
		for (int i = 0; i < dataOutputBitIndexes.Count(); i++) {
			int j = dataOutputBitIndexes[i];
			int bit = LBMath.GetBit(allCodewords[LBConverter.ShiftRightLogic(i, 3)], ~i & 7);
			int index = LBConverter.ShiftRightLogic(j, 5);
			modules[index] = modules[index] | (bit << j);
		}
	}

	void applyMask(array<int> msk) {
		if (msk.Count() != modules.Count())
			Error("Unexpected error in applyMask");
		for (int i = 0; i < msk.Count(); i++)
			modules[i] = modules[i] ^ msk[i];
	}

	void handleConstructorMasking(array<int> mask) {
		applyMask(mask); // Apply the final choice of mask
		drawFormatBits(); // Overwrite old format bits
	}

	static int getNumDataCodewords(int ver) {
		return LB_QrTemplate.getNumRawDataModules(ver) / 8 - ECC_CODEWORDS_PER_BLOCK[ver] * NUM_ERROR_CORRECTION_BLOCKS[ver];
	}
}
