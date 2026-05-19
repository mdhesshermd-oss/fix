[DayZGame.RegisterLBAdminPage("CurrenciesAdminMenu", "LBmaster_Core/logo/logo.paa", "#lb_admin_currencies", "core.change.currencies")]
class CurrenciesAdminMenu : LBAdmin_Menu_Page {

	EditBoxWidget input_prefix, input_suffix, input_itemname, input_value, input_separator;
	CheckBoxWidget chckbx_depositable_only;
	ButtonWidget btn_add, btn_delete;
	TextListboxWidget list_currencies;
	ItemPreviewWidget item_preview;
	EntityAI lastPreview = null;

	override void OnShow() {
		linked.LoadLinkedVars();
	}

	override array<string> GetEditedConfigs() {
		return {"LBCurrencyConfig"};
	}

	override void OnHide() {
		if (lastPreview) {
			g_Game.ObjectDelete(lastPreview);
		}
	}

	override void RegisterAllLinkedVars() {
		linked.RegisterLinkedVar("GetCurrencyConfig().currencyValues[list_currencies].itemclassname", input_itemname, true).SetChangeTrigger("UpdateCurrencies()");
		linked.RegisterLinkedVar("GetCurrencyConfig().currencyValues[list_currencies].value", input_value);
		linked.RegisterLinkedVar("GetCurrencyConfig().currencyValues[list_currencies].depositableOnly", chckbx_depositable_only);
		linked.RegisterLinkedVar("GetCurrencyConfig().currencyPrefix", input_prefix);
		linked.RegisterLinkedVar("GetCurrencyConfig().currencySuffix", input_suffix);
		linked.RegisterLinkedVar("GetCurrencyConfig().thousandsSeparator", input_separator);
		linked.RegisterLinkedList("GetCurrencyConfig().currencyValues", list_currencies, btn_add, btn_delete, null).EnableItemSelector().SetListManager(new LBListManager<LBCurrencyConfigEntry>(), true).SetChangeTrigger("UpdateCurrencies()");
	}

	void UpdateCurrencies() {
		UpdateSelectedCurrency();
		for (int i = 0; i < list_currencies.GetNumItems(); i++) {
			LBCurrencyConfigEntry entry = LBCurrencyConfig.Get.GetEntryClass(i);
			string path = "CfgVehicles " + entry.itemclassname;
			TStringArray fullPath = new TStringArray();
			g_Game.ConfigGetFullPath(path, fullPath);
			if (fullPath.Count() <= 0) {
				list_currencies.SetItemColor(i, 0, ARGB(255, 255, 0, 0));
			} else {
				list_currencies.SetItemColor(i, 0, ARGB(255, 0, 255, 0));
			}
		}
	}

	LBCurrencyConfig_ GetCurrencyConfig() {
		return LBCurrencyConfig.Get;
	}

	LBCurrencyConfigEntry GetSelectedCurrency() {

		int row = list_currencies.GetSelectedRow();
		if (row < 0 || row >= list_currencies.GetNumItems())
			return null;
		return LBCurrencyConfig.Get.GetEntryClass(row);
	}

	void UpdateSelectedCurrency() {
		LBCurrencyConfigEntry entry = GetSelectedCurrency();
		if (!entry)
			return;
		if (lastPreview) {
			item_preview.SetItem(null);
			g_Game.ObjectDelete(lastPreview);
		}

		Object obj = g_Game.CreateObjectEx(entry.itemclassname, vector.Zero, ECE_LOCAL);
		lastPreview = EntityAI.Cast(obj);
		if (!lastPreview && obj) {
			g_Game.ObjectDelete(obj);
		} else {
			item_preview.SetItem(lastPreview);
		}
	}
}