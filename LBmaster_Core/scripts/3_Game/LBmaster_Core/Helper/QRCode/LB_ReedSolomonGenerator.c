class LB_ReedSolomonGenerator {
	ref array<int> coefficients = new array<int>;

	void LB_ReedSolomonGenerator(int degree) {
		ResizeArray(coefficients, degree);

		coefficients[degree - 1] = 1;

		int root = 1;
		for (int i = 0; i < degree; i++) {
			for (int j = 0; j < coefficients.Count(); j++) {
				coefficients[j] = multiply(coefficients[j] & 0xFF, root) & 0xff;
				if (j + 1 < coefficients.Count()) {
					coefficients[j] = coefficients[j] ^ coefficients[j + 1];
				}
			}
			root = multiply(root, 0x02);
		}
	}

	void getRemainder(array<int> data, int dataOff, int dataLen, out array<int> result) {
		ResizeArray(result, coefficients.Count());

		for (int i = dataOff, dataEnd = dataOff + dataLen; i < dataEnd; i++) {
			int factor = (data[i] ^ result[0]) & 0xFF;

			for (int j = 0; j < result.Count() - 1; j++) {
				int newVal = result.Get(j + 1) ^ multiply(factor, coefficients.Get(j) & 0xFF);
				result.Set(j, newVal & 0xff);
			}
			newVal = multiply(factor, coefficients.Get(result.Count() - 1) & 0xFF);
			result.Set(result.Count() - 1, newVal & 0xff);
		}
	}

	int multiply(int x, int y) {
		int z = 0;
		for (int i = 7; i >= 0; i--) {
			z = (z << 1) ^ (LBConverter.ShiftRightLogic(z, 7) * 0x11D);
			z ^= (LBConverter.ShiftRightLogic(y, i) & 1) * x;
		}
		return z;
	}

	static void ResizeArray(array<int> arr, int size) {
		arr.Resize(size);
		for (int z = 0; z < size; ++z)
			arr[z] = 0;
	}

	static void ResizeArray(array<bool> arr, int size) {
		arr.Resize(size);
		for (int z = 0; z < size; ++z)
			arr[z] = 0;
	}

	static void ResizeArray(array<ref array<bool>> arr, int size) {
		arr.Resize(size);
		for (int z = 0; z < size; ++z)
			arr[z] = null;
	}
}