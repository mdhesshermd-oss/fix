[DayZGame.RegisterLBAdminPage("LBLoggingSettingsMenu", "LBmaster_Core/logo/logo.paa", "Logger Settings", "core.change.logger")]
class LBLoggingSettingsMenu : LBAdmin_Menu_Page {

	CheckBoxWidget chckbx_full_time_client, chckbx_scriptlog_client, chckbx_full_time_server, chckbx_scriptlog_server, chckbx_send_critical_server;
	XComboBoxWidget combo_loglevel_client, combo_loglevel_server;
	EditBoxWidget edit_folder_client, edit_folder_server;
	Widget panel_client, panel_server;
	ButtonWidget btn_save_client, btn_save_server;

	ref LBLoggerConfig clientCfg, serverCfg;

	override void OnShow() {
		linked.LoadLinkedVars();
		clientCfg = LBLogger.LoadConfig();
		panel_server.Show(false);
		if (LBAdmins.Get().HasPermission("core.change.logger_server")) {
			g_Game.RPCSingleParam(null, LBmaster_Core_RPCs.REQUEST_LOGGER_CONFIG, new Param1<bool>(true), true);
		}
	}

	override void OnRPC(Object target, int rpc_type, ParamsReadContext ctx) {
		if (rpc_type == LBmaster_Core_RPCs.REQUEST_LOGGER_CONFIG) {
			ctx.Read(serverCfg);
			panel_server.Show(true);
			linked.LoadLinkedVars();
		}
	}

	override array<string> GetEditedConfigs() {}

	override void RegisterAllLinkedVars() {
		array<ref Param2<string, string>> logLevels = new array<ref Param2<string, string>>();
		logLevels.Insert(new Param2<string, string>("None", "0"));
		logLevels.Insert(new Param2<string, string>("Fatal", "1"));
		logLevels.Insert(new Param2<string, string>("Error", "2"));
		logLevels.Insert(new Param2<string, string>("Admin", "3"));
		logLevels.Insert(new Param2<string, string>("Info", "4"));
		logLevels.Insert(new Param2<string, string>("Debug", "5"));
		logLevels.Insert(new Param2<string, string>("Verbose", "6"));
		logLevels.Insert(new Param2<string, string>("Spam", "7"));
		linked.RegisterLinkedVar("clientCfg.logLevel", combo_loglevel_client).SetComboValues(logLevels);
		linked.RegisterLinkedVar("clientCfg.logFolder", edit_folder_client);
		linked.RegisterLinkedVar("clientCfg.logFullTimestamp", chckbx_full_time_client);
		linked.RegisterLinkedVar("clientCfg.logToScriptlog", chckbx_scriptlog_client);

		linked.RegisterLinkedVar("serverCfg.logLevel", combo_loglevel_server).SetComboValues(logLevels);
		linked.RegisterLinkedVar("serverCfg.logFolder", edit_folder_server);
		linked.RegisterLinkedVar("serverCfg.logFullTimestamp", chckbx_full_time_server);
		linked.RegisterLinkedVar("serverCfg.logToScriptlog", chckbx_scriptlog_server);
		linked.RegisterLinkedVar("serverCfg.sendCriticalErrorLogs", chckbx_send_critical_server);
	}

	override bool OnClick(Widget w, int x, int y, int button) {
		if (super.OnClick(w, x, y, button))
			return true;
		if (w == btn_save_client) {
			string error;
			if (JsonFileLoader<LBLoggerConfig>.SaveFile(LBLogger.LOG_CONFIG_PATH, clientCfg, error)) {
				LBLogger.ReloadConfig();
				NotificationSystem.AddNotificationExtended(4.0, "#lb_co_config_saved", "The config was saved sucessfully !", GetLBMessageInfoIcon());
			}
		} else if (w == btn_save_server) {
			ScriptRPC rpc = new ScriptRPC();
			rpc.Write(serverCfg);
			rpc.Send(null, LBmaster_Core_RPCs.CHANGE_LOGGER_CONFIG, true);
			LBLoadingIcon.Show();
		}
		return false;
	}

}