class LBInputUtils {

	static string GetInputKeybindOut(string inputname) {
		string keyBind = "";
		GetInputKeybind(inputname, keyBind);
		return keyBind;
	}

	static bool GetInputKeybind(string inputname, out string keybind) {
		UAInput input = GetUApi().GetInputByName(inputname);
		if (!input) {
			keybind = "UNKNOWN";
			return false;
		}
		if (SetElementTitle(input, EInputDeviceType.MOUSE_AND_KEYBOARD, keybind)) {
			return true;
		}
		keybind = "NOT SET";
		return false;
	}

	private static bool SetElementTitle(UAInput pInput, int iDeviceFlags, out string output) {
		int countbind = 0;

		for (int a = 0; a < pInput.AlternativeCount(); a++) {
			pInput.SelectAlternative(a);
			if (pInput.IsCombo()) {
				if (pInput.BindingCount() > 0) {
					if (pInput.Binding(0) != 0 && pInput.CheckBindDevice(0, iDeviceFlags)) {
						if (countbind > 0)
							output += ", ";

						output += GetUApi().GetButtonName(pInput.Binding(0));
						countbind++;

						for (int i = 1; i < pInput.BindingCount(); i++) {
							if (pInput.Binding(i) != 0) {
								output += " + " + GetUApi().GetButtonName(pInput.Binding(i));
								countbind++;
							}
						}

					}
				}
			} else {
				if (pInput.BindingCount() > 0) {
					if (pInput.Binding(0) != 0 && pInput.CheckBindDevice(0, iDeviceFlags)) {
						if (countbind > 0)
							output += ", ";

						output += GetUApi().GetButtonName(pInput.Binding(0));
						countbind++;
					}
				}
			}

		}

		return (countbind > 0);
	}
}