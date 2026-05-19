class LBT<Class T1> {

	static T1 T(bool first, T1 f, T1 s) {
		if (first)
			return f;
		return s;
	}

}