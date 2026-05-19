int lb_logger_init = LBLogger.Init();
class LBLogger {

	private static const int LOG_FATAL = 1;
	private static const int LOG_ERROR = 2;
	private static const int LOG_ADMIN = 3;
	private static const int LOG_INFO = 4;
	private static const int LOG_DEBUG = 5;
	private static const int LOG_VERBOSE = 6;
	private static const int LOG_SPAM = 7;

	static const string LOG_CONFIG_PATH = "$profile:LBmaster/Config/Common/Logger.json";

	private static FileHandle fileHandle;
	private static bool logFullTimestamp;
	private static int currentLogLevel;

	private static bool doLog;
	private static bool sendCriticalErrorLogs;
	private static bool logToScriptlog;

	private static ref map<string, ref Param2<string, string>> loggingPrefixInfo = new map<string, ref Param2<string, string>>();
	static ref TStringArray errorLogs = new TStringArray();
	static string lastError, lastFatal;
	static string savedToFile, saveToFolder;

	static int Init() {
		SetupLogger();
		return 1;
	}

	static LBLoggerConfig LoadConfig() {
		LBmasterConfigMover.CreateParentFolders(LOG_CONFIG_PATH);
		LBLoggerConfig cfg;
		string error = "";
		if (!FileExist(LOG_CONFIG_PATH)) {
			cfg = new LBLoggerConfig();
			if (!JsonFileLoader<LBLoggerConfig>.SaveFile(LOG_CONFIG_PATH, cfg, error))
				Print("Could not Save Logger.json! Error: " + error);
		} else {
			if (!JsonFileLoader<LBLoggerConfig>.LoadFile(LOG_CONFIG_PATH, cfg, error))
				Print("Could not Load Logger.json! Error: " + error);
		}
		if (cfg.UpdateVersion()) {
			if (!JsonFileLoader<LBLoggerConfig>.SaveFile(LOG_CONFIG_PATH, cfg, error))
				Print("Could not Save Logger.json! Error: " + error);
		}

		return cfg;
	}

	private static void SetupLogger() {
		LBLoggerConfig cfg = LoadConfig();
		currentLogLevel = cfg.logLevel;
		logFullTimestamp = cfg.logFullTimestamp;
		logToScriptlog = cfg.logToScriptlog;
		sendCriticalErrorLogs = cfg.sendCriticalErrorLogs;
		saveToFolder = cfg.logFolder;

		string timestamp = LBDate.Init(false).ToFormattedString();
		string fileTimestamp = timestamp + "";
		fileTimestamp.Replace(".", "_");
		fileTimestamp.Replace(":", "_");
		fileTimestamp.Replace(" ", "_");
		savedToFile = cfg.logFolder + "LBmaster_" + fileTimestamp + ".log";
		LBmasterConfigMover.CreateParentFolders(savedToFile);
		fileHandle = OpenFile(savedToFile, FileMode.WRITE);
		if (fileHandle == 0) {
			Print("Could not Start LBLogger at: " + savedToFile);
			Error("Could not Start LBLogger at: " + savedToFile);
		} else {
			Print("Started LBLogger at: " + savedToFile);
			FPrintln(fileHandle, "Logger Started at: " + timestamp);
			doLog = true; // Only Set DoLog to 1 after the File Handle was created successfully
		}
	}

	static void ReloadConfig() {
		LBLoggerConfig cfg = LoadConfig();
		if (cfg.logLevel != currentLogLevel) {
			Admin("Log Level was changed from " + GetLoglevelStringPretty(currentLogLevel) + " to " + GetLoglevelStringPretty(cfg.logLevel), "Core");
		}
		if (cfg.logFullTimestamp != logFullTimestamp) {
			Admin("Log Full Timestamp was changed from " + logFullTimestamp + " to " + cfg.logFullTimestamp, "Core");
		}
		if (cfg.logToScriptlog != logToScriptlog) {
			Admin("Log to Scriptlog was changed from " + logToScriptlog + " to " + cfg.logToScriptlog, "Core");
		}
		if (cfg.sendCriticalErrorLogs != sendCriticalErrorLogs) {
			Admin("Send critical errors was changed from " + sendCriticalErrorLogs + " to " + cfg.sendCriticalErrorLogs, "Core");
		}
		if (cfg.logFolder != saveToFolder) {
			Admin("Log Folder was changed from " + saveToFolder + " to " + cfg.logFolder, "Core");
			CloseFile(fileHandle);
			SetupLogger();
		} else {
			currentLogLevel = cfg.logLevel;
			logFullTimestamp = cfg.logFullTimestamp;
			logToScriptlog = cfg.logToScriptlog;
			sendCriticalErrorLogs = cfg.sendCriticalErrorLogs;
		}
	}

	private static string GetTimestampString() {
		int m_sec, m_min, m_hour;
		GetHourMinuteSecond(m_hour, m_min, m_sec);
		string hourStr = "" + m_hour;
		if (m_hour < 10)
			hourStr = "0" + m_hour;
		string minStr = "" + m_min;
		if (m_min < 10)
			minStr = "0" + m_min;
		string secStr = "" + m_sec;
		if (m_sec < 10)
			secStr = "0" + m_sec;
		if (logFullTimestamp) {
			int m_year, m_month, m_day;
			GetYearMonthDay(m_year, m_month, m_day);
			string monthStr = "" + m_month;
			if (m_month < 10)
				monthStr = "0" + m_month;
			string dayStr = "" + m_day;
			if (m_day < 10)
				dayStr = "0" + m_day;
			return "" + m_day + "." + m_month + "." + m_year + " " + hourStr + ":" + minStr + ":" + secStr;
		} else {
			return hourStr + ":" + minStr + ":" + secStr;
		}
	}

	static string GetCallingMethodFromStack(int index = 0) {
		string stack;
		DumpStackString(stack);
		TStringArray lines = new TStringArray();
		stack.Split("\n", lines);
		if (lines.Count() > index + 2)
			return lines.Get(index + 2).Trim();
		return "Unknown Source";
	}

	static string GetCallingMethodNameFromStack(int index = 0) {
		string line = GetCallingMethodFromStack(index + 1);
		index = line.IndexOf("(");
		if (index == INDEX_NOT_FOUND)
			return line;
		return line.Substring(0, index);
	}

	static bool Spam() {
		return IsLogged(LOG_VERBOSE);
	}

	static bool Verbose() {
		return IsLogged(LOG_VERBOSE);
	}

	static bool Debug() {
		return IsLogged(LOG_DEBUG);
	}

	static bool Info() {
		return IsLogged(LOG_INFO);
	}

	static bool Admin() {
		return IsLogged(LOG_ADMIN);
	}

	static bool Error() {
		return IsLogged(LOG_ERROR);
	}

	static bool Fatal() {
		return IsLogged(LOG_FATAL);
	}

	static void Spam(string message, string mod) {
		Log(message, LOG_SPAM, mod, " [SPAM ] [");
	}

	static void Verbose(string message, string mod) {
		Log(message, LOG_VERBOSE, mod, " [VERB ] [");
	}

	static void Debug(string message, string mod) {
		Log(message, LOG_DEBUG, mod, " [DEBUG] [");
	}

	static void Info(string message, string mod) {
		Log(message, LOG_INFO, mod, " [INFO ] [");
	}

	static void Admin(string message, string mod) {
		Log(message, LOG_ADMIN, mod, " [ADMIN] [");
	}

	static void Error(string message, string mod, bool remoteLog = false) {
		lastError = message;
		string fullMessage = Log(message, LOG_ERROR, mod, " [ERROR] [");
#ifdef SERVER
		if (errorLogs && fullMessage != "")
			errorLogs.Insert(fullMessage);
		if (remoteLog) {
			RemoteLogMessage(message, mod);
		}
#endif
	}

	static void Fatal(string message, string mod, bool remoteLog = false) {
		string fullMessage = Log(message, LOG_FATAL, mod, " [FATAL] [");
		lastFatal = message;
#ifdef SERVER
		if (errorLogs && fullMessage != "")
			errorLogs.Insert(fullMessage);
		if (remoteLog) {
			RemoteLogMessage(message, mod);
		}
#endif
	}

	private static bool IsLogged(int logLevel) {
		return logLevel <= currentLogLevel;
	}

	static void RemoteLogMessage(string message, string mod) {
#ifdef SERVER
		if (!sendCriticalErrorLogs)
			return;
		Param2<string, string> info;
		if (!loggingPrefixInfo.Find(mod, info)) {
			string stack = "";
			DumpStackString(stack);
			TStringArray lines = new TStringArray();
			stack.Split("\n", lines);
			if (lines.Count() < 3)
				return;
			string line = lines.Get(2);
			int index1 = line.IndexOf(" ");
			int index2 = line.IndexOfFrom(index1, "/");
			int index3 = line.IndexOfFrom(index1, "\\");
			if (index2 == -1 || index3 != -1 && index3 < index2)
				index2 = index3;
			if (index1 == -1 || index2 == -1)
				return;
			string pboPrefix = line.Substring(index1, index2 - index1).Trim();
			string version = g_Game.ConfigGetTextOut("LBVersions " + pboPrefix + " version");
			info = new Param2<string, string>(pboPrefix, version);
			loggingPrefixInfo.Insert(mod, info);
		}
		RestApi api = GetRestApi();
		if (api == null)
			api = CreateRestApi();
		string port = "2302";
		GetCLIParam("port", port);
		Print("Info: " + info.param1 + " and " + info.param2);
		RestContext ctx = api.GetRestContext("https://" + GetElevatorGame().GetLicenseServerConnection() + "/remoteLog?p=" + port + "&v=" + info.param2 + "&n=" + info.param1);
		ctx.POST(new LBLoggerCallback(), "", message);
#endif
	}

	static string FormatPlayerIdentity(PlayerIdentity identity) {
		if (identity == null)
			return "NULL";
		return identity.GetName() + " (" + identity.GetPlainId() + ")";
	}

	static string FormatPlayerIdentity(Man player) {
		if (player == null)
			return "NULL";
		return FormatPlayerIdentity(player.GetIdentity());
	}

	private static string Log(string message, int logLevel, string mod, string logLevelStr) {
		if (logLevel > currentLogLevel)
			return "";
		if (!doLog) {
			Print("[" + mod + "] " + message);
			return "";
		}
		string finalMessage = GetTimestampString() + logLevelStr + mod + "] " + message;
		FPrintln(fileHandle, finalMessage);
		if (logToScriptlog) {
			Print("" + finalMessage);
		}
		return finalMessage;
	}

	static string GetLoglevelStringPretty(int level) {
		string logLevelStr;
		switch (level) {
			case LOG_FATAL:
				logLevelStr = "Fatal";
				break;
			case LOG_ERROR:
				logLevelStr = "Error";
				break;
			case LOG_ADMIN:
				logLevelStr = "Admin";
				break;
			case LOG_INFO:
				logLevelStr = "Info";
				break;
			case LOG_DEBUG:
				logLevelStr = "Debug";
				break;
			case LOG_VERBOSE:
				logLevelStr = "Verbose";
				break;
			case LOG_SPAM:
				logLevelStr = "Spam";
				break;
			default:
				logLevelStr = "Unknown";
				break;
		}
		return logLevelStr;
	}

}