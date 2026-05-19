class LBCurrencyConfigEntry {

	string itemclassname; // The Classname (case sensitive) of the Item
	int value; // Value **one** Item is worth (not a whole stack). For example there is a 1 Dollar bill and you can stack them up to 500, then the value is 1 and not 500
	bool depositableOnly = false; // Set if the currency can only be deposited, for example to deposit something like Gold Bars at the ATM, but don't give them back when withdrawing money

	static LBCurrencyConfigEntry Init(string classname, int val, bool depositableOnly_ = false) {
		LBCurrencyConfigEntry entry = new LBCurrencyConfigEntry();
		entry.itemclassname = classname;
		entry.value = val;
		if (val == -1) {
			int len = 0;
			for (int i = classname.Length(); i > 0; --i) {
				int char = classname.Substring(i - 1, 1).Hash();
				if (char < 48 || char > 57)
					break;
				++len;
			}
			if (len > 0) {
				entry.value = classname.Substring(classname.Length() - len, len).ToInt();
			} else {
				entry.value = 0;
			}
		}
		entry.depositableOnly = depositableOnly_;
		return entry;
	}

	void Setup(string classname) {
		itemclassname = classname;
	}

	string ToListString(int column) {
		if (column == 0)
			return itemclassname;
		return "" + value;
	}

	void WriteToCtx(ParamsWriteContext ctx) {
		ctx.Write(itemclassname);
		ctx.Write(value);
		ctx.Write(depositableOnly);
	}

	bool ReadFromCtx(ParamsReadContext ctx) {
		if (!ctx.Read(itemclassname))
			return false;
		if (!ctx.Read(value))
			return false;
		if (!ctx.Read(depositableOnly))
			return false;
		return true;
	}

}
