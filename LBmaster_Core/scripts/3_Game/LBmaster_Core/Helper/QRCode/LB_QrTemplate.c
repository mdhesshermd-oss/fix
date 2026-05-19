class LB_QrTemplate {

	int version;
	int size;
	ref array<int> template = new array<int>();
	ref array<int> mask = new array<int>();
	ref array<int> outputBitIndexes = new array<int>();
	ref array<int> isFunction = new array<int>();

	void LB_QrTemplate(int ver) {
		if (ver < LB_QrCode.MIN_VERSION || ver > LB_QrCode.MAX_VERSION) {
			Error("QR Code Version out of range");
			return;
		}
		this.version = ver;
		this.size = version * 4 + 17;
		LB_ReedSolomonGenerator.ResizeArray(template, (size * size + 31) / 32);
		LB_ReedSolomonGenerator.ResizeArray(isFunction, template.Count());

		drawFunctionPatterns();
		mask = generateMask(); // Reads fields, returns array
		outputBitIndexes = generateZigzagScan(); // Reads fields, returns array
		isFunction = null;
	}

	private void drawFunctionPatterns() {
		// Draw horizontal and vertical timing patterns
		for (int i = 0; i < size; i++) {
			darkenFunctionModule(6, i, ~i & 1);
			darkenFunctionModule(i, 6, ~i & 1);
		}

		// Draw 3 finder patterns (all corners except bottom right; overwrites some timing modules)
		drawFinderPattern(3, 3);
		drawFinderPattern(size - 4, 3);
		drawFinderPattern(3, size - 4);

		// Draw numerous alignment patterns
		array<int> alignPatPos = getAlignmentPatternPositions();
		int numAlign = alignPatPos.Count();
		for (i = 0; i < numAlign; i++) {
			for (int j = 0; j < numAlign; j++) {
				// Don't draw on the three finder corners
				if (!(i == 0 && j == 0 || i == 0 && j == numAlign - 1 || i == numAlign - 1 && j == 0))
					drawAlignmentPattern(alignPatPos[i], alignPatPos[j]);
			}
		}

		// Draw configuration data
		drawDummyFormatBits();
		drawVersion();
	}

	private void drawDummyFormatBits() {
		// Draw first copy
		for (int i = 0; i <= 5; i++)
			darkenFunctionModule(8, i, 0);
		darkenFunctionModule(8, 7, 0);
		darkenFunctionModule(8, 8, 0);
		darkenFunctionModule(7, 8, 0);
		for (i = 9; i < 15; i++)
			darkenFunctionModule(14 - i, 8, 0);

		// Draw second copy
		for (i = 0; i < 8; i++)
			darkenFunctionModule(size - 1 - i, 8, 0);
		for (i = 8; i < 15; i++)
			darkenFunctionModule(8, size - 15 + i, 0);
		darkenFunctionModule(8, size - 8, 1); // Always dark
	}

	private void drawVersion() {
		if (version < 7)
			return;

		// Calculate error correction code and pack bits
		int rem = version; // version is uint6, in the range [7, 40]
		for (int i = 0; i < 12; i++)
			rem = (rem << 1) ^ (LBConverter.ShiftRightLogic(rem, 11) * 0x1F25);
		int bits = version << 12 | rem; // uint18
		if (!(LBConverter.ShiftRightLogic(bits, 18) == 0))
			Error("Error in drawVersion");

		// Draw two copies
		for (i = 0; i < 18; i++) {
			int bit = LBMath.GetBit(bits, i);
			int a = size - 11 + i % 3;
			int b = i / 3;
			darkenFunctionModule(a, b, bit);
			darkenFunctionModule(b, a, bit);
		}
	}

	private void drawFinderPattern(int x, int y) {
		for (int dy = -4; dy <= 4; dy++) {
			for (int dx = -4; dx <= 4; dx++) {
				int dist = LBMath.Max(Math.AbsInt(dx), Math.AbsInt(dy)); // Chebyshev/infinity norm
				int xx = x + dx, yy = y + dy;
				if (0 <= xx && xx < size && 0 <= yy && yy < size)
					darkenFunctionModule(xx, yy, LBT<int>.T(dist != 2 && dist != 4, 1, 0));
			}
		}
	}

	private void drawAlignmentPattern(int x, int y) {
		for (int dy = -2; dy <= 2; dy++) {
			for (int dx = -2; dx <= 2; dx++)
				darkenFunctionModule(x + dx, y + dy, Math.AbsInt(LBMath.Max(Math.AbsInt(dx), Math.AbsInt(dy)) - 1));
		}
	}

	private array<int> generateMask() {
		array<int> result = new array<int>();
		LB_ReedSolomonGenerator.ResizeArray(result, template.Count());
		for (int y = 0, i = 0; y < size; y++) {
			for (int x = 0; x < size; x++) {
				bool invert = false;
				if ((x + y) % 2 == 0)
					invert = true;
				int bit = LBT<int>.T(invert, 1, 0) & ~getModule(isFunction, x, y);
				int index = LBConverter.ShiftRightLogic(i, 5);
				result[index] = result[index] | (bit << i);
				++i;
			}
		}
		return result;
	}

	private array<int> generateZigzagScan() {
		array<int> result = new array<int>();
		LB_ReedSolomonGenerator.ResizeArray(result, getNumRawDataModules(version) / 8 * 8);
		int i = 0; // Bit index into the data
		for (int right = size - 1; right >= 1; right -= 2) { // Index of right column in each column pair
			if (right == 6)
				right = 5;
			for (int vert = 0; vert < size; vert++) { // Vertical counter
				for (int j = 0; j < 2; j++) {
					int x = right - j; // Actual x coordinate
					bool upward = false;
					if (((right + 1) & 2) == 0)
						upward = true;
					int y = LBT<int>.T(upward, size - 1 - vert, vert); // Actual y coordinate
					if (getModule(isFunction, x, y) == 0 && i < result.Count()) {
						result[i] = y * size + x;
						i++;
					}
				}
			}
		}
		if (i != result.Count())
			Error("Result not correct!");
		return result;
	}

	private int getModule(array<int> grid, int x, int y) {
		if (!(0 <= x && x < size))
			Error("Error in getModule");
		if (!(0 <= y && y < size))
			Error("Error in getModule");
		int i = y * size + x;
		return LBMath.GetBit(grid[LBConverter.ShiftRightLogic(i, 5)], i);
	}

	private void darkenFunctionModule(int x, int y, int enable) {
		if (!(0 <= x && x < size))
			Error("Error in darkenFunctionModule");
		if (!(0 <= y && y < size))
			Error("Error in darkenFunctionModule");
		if (!(enable == 0 || enable == 1))
			Error("Error in darkenFunctionModule");
		int i = y * size + x;
		int index = LBConverter.ShiftRightLogic(i, 5);
		template[index] = template[index] | (enable << i);
		isFunction[index] = isFunction[index] | (1 << i);
	}

	private array<int> getAlignmentPatternPositions() {
		if (version == 1)
			return new array<int>;
		else {
			int numAlign = version / 7 + 2;
			int step = (version * 8 + numAlign * 3 + 5) / (numAlign * 4 - 4) * 2;
			array<int> result = new array<int>();
			LB_ReedSolomonGenerator.ResizeArray(result, numAlign);
			result[0] = 6;
			for (int i = result.Count() - 1, pos = size - 7; i >= 1; i--) {
				result[i] = pos;
				pos -= step;
			}
			return result;
		}
	}

	static int getNumRawDataModules(int ver) {
		if (ver < LB_QrCode.MIN_VERSION || ver > LB_QrCode.MAX_VERSION) {
			Error("Version number out of range");
			return 0;
		}
		int result = (16 * ver + 128) * ver + 64;
		if (ver >= 2) {
			int numAlign = ver / 7 + 2;
			result -= (25 * numAlign - 10) * numAlign - 55;
			if (ver >= 7)
				result -= 36;
		}
		return result;
	}

}