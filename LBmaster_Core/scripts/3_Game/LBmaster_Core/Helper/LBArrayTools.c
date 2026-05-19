class LBArrayTools<Class T1> {

	static array<T1> ToArray(set<T1> set_) {
		array<T1> arr = new array<T1>();
		foreach (T1 entry : set_)
			arr.Insert(entry);
		return arr;
	}

	static set<T1> ToSet(array<T1> arr_) {
		set<T1> set_ = new set<T1>();
		foreach (T1 entry : arr_)
			set_.Insert(entry);
		return set_;
	}

	static void InsertAll(array<T1> to, set<T1> from) {
		foreach (string entry : from)
			to.Insert(entry);
	}

	static void InsertAll(set<T1> to, array<T1> from) {
		foreach (string entry : from)
			to.Insert(entry);
	}

	static void RemoveDuplicate(array<T1> arr) {
		array<T1> inserted = new array<T1>();
		for (int i = 0; i < arr.Count(); i++) {
			if (inserted.Find(arr[i]) == -1) {
				inserted.Insert(arr[i]);
			} else {
				arr.Remove(i);
				i--;
			}
		}
	}

}
