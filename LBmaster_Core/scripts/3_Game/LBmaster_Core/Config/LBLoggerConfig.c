/*
class LBLoggerConfig : LBConfigLoader<LBLoggerConfig_> {
	override void InitVars() {
		InitVarsInternal("Common", "Logger.json", LBConfigType.CONFIG, true, "core.change.logger"); // (easy)
	}
}
*/
// This file contains the setting for what messages should be logged and where they should be written to
class LBLoggerConfig { /*: LBConfigBase*/

	const static int CURRENT_VERSION = 2;
	int version = CURRENT_VERSION; // Version for internal Version tracking (Current Version: 1)
	int logLevel = 4; // Defines how much information will be logged (0 to 7) (none to everything). (`0 = None`, `1 = Fatal`, `2 = Error`, `3 = Admin`, `4 = Info` (default), `5 = Debug`, `6 = Verbose`, `7 = Spam`)
	bool logFullTimestamp = false; // 0 to only log the time of the Day in the log. 1 to log the full timestamp including the date in the log
	string logFolder = "$profile:"; // Folder where the LBmaster log will be saved to (default is the profiles folder to allow easy log rotation for the server managers)
	bool logToScriptlog = false; // Toggle if the log should be written to the scriptlog too. This does not disable the logging to the LBmaster Log
	bool sendCriticalErrorLogs = true; // When this option is enabled, specific error logs are sent to the License server for analysis. Logs are only kept for 30 Days and are only used to improve our products and make them more reliable and bugfree.

	bool UpdateVersion() {
		if (version == CURRENT_VERSION)
			return false;
		if (version < 1) {
			logToScriptlog = false;
		}
		if (version < 2) {
			sendCriticalErrorLogs = true;
		}
		version = CURRENT_VERSION;
		return true;
	}

}