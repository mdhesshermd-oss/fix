modded class OptionsMenuGame {

	static int lb_optionsStart = 574634;
	ref array<ref OptionSelectorMultistate> selectors_lb = new array<ref OptionSelectorMultistate>();

	void OptionsMenuGame(Widget parent, Widget details_root, GameOptions options, OptionsMenu menu) {
		if (LBTooltipConfig.Get) {
			ScriptInvoker inv = new ScriptInvoker();
			inv.Insert(OnTooltipShowChanged);
			RegisterOptionLB("Show Tooltips", "Toggle if Tooltips while hovering over some widgets should be displayed or not", LBTooltipConfig.Get.enabled, inv, "hud_settings_content");
		}
	}

	void RegisterOptionLB(string name, string tooltip, int index, ScriptInvoker onChange, string parent) {
		RegisterOptionLBMultiple(name, tooltip, index, onChange, parent, { "#options_controls_disabled", "#options_controls_enabled" });
	}
	void RegisterOptionLBMultiple(string name, string tooltip, int index, ScriptInvoker onChange, string parent, TStringArray states) {
		Widget w = LBLayoutManager.Get().CreateLayout("DayZConfigOption", m_Root.FindAnyWidget(parent));
		Widget option = w.FindAnyWidget("setting_option");
		option.SetUserID(lb_optionsStart);
		OptionSelectorMultistate selector = new OptionSelectorMultistate(option, index, this, false, states);
		name.ToUpper();
		TextWidget.Cast(w.FindAnyWidget("setting_label")).SetText(name);
		selectors_lb.Insert(selector);
		selector.m_OptionChanged = onChange;
		m_TextMap.Insert(lb_optionsStart, new Param2<string, string>(name, tooltip));
		++lb_optionsStart;
	}

	void OnTooltipShowChanged(int index) {
		if (index == 0)
			LBTooltipConfig.Get.enabled = false;
		else
			LBTooltipConfig.Get.enabled = true;
		LBTooltipConfig.Loader.Save();
	}

}