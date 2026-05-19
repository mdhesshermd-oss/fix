string lb_last_JsonLoaderError = "";

class LBJsonLoader<Class T1> {

	static ref JsonSerializer m_Serializer = new JsonSerializer;
	static ref array<Class> persistentInstances = new array<Class>();

	static bool JsonLoadFile(string filename, out T1 data, bool closeOnError = false, bool writeErrorToLog = true, bool keepBackups = false) {
		bool close = false, writeError = false;
		if (closeOnError && !keepBackups)
			close = true;
		if (writeErrorToLog && !keepBackups)
			writeError = true;
		if (JsonLoadFileInternal(filename, filename, data, close, writeError))
			return true;
		if (JsonLoadFileInternal(filename + ".backup", filename, data, close, writeError))
			return true;
		if (JsonLoadFileInternal(filename + ".backup2", filename, data, closeOnError, writeErrorToLog))
			return true;
		return false;
	}

	private static bool JsonLoadFileInternal(string filename, string originalFilename, out T1 data, bool closeOnError = false, bool writeErrorToLog = true) {
		string file_content;
		int start = TickCount(0);
		if (!ReadContent(filename, closeOnError, writeErrorToLog, file_content))
			return false;
		lb_last_JsonLoaderError = "";
		LBLogger.Verbose("Reading file content of " + filename + " took " + (TickCount(start) / 10000) + "ms", "Core");
		start = TickCount(0);
		if (!m_Serializer.ReadFromString(data, file_content, lb_last_JsonLoaderError)) {
			if (closeOnError) {
				LBLogger.Fatal("Could not read json file: " + originalFilename + " and closing server for invalid configuration ! Error: " + lb_last_JsonLoaderError, "Core");
				int z = 7 % (1 - 1);
			} else if (writeErrorToLog) {
				LBLogger.Error("Could not read json file: " + originalFilename + "! Error: " + lb_last_JsonLoaderError, "Core");
			}
			return false;
		} else if (filename != originalFilename) {
			LBLogger.Info("Restored Backup file " + filename, "Core");
			WriteFileContent(originalFilename, file_content);
		}
		LBLogger.Verbose("Parsing Json file " + filename + " took " + (TickCount(start) / 10000) + "ms", "Core");
		return true;
	}

	static void JsonSaveFile(string filename, T1 data, bool keepBackups = false) {
		WriteContent(filename, data, keepBackups);
		if (g_Game && keepBackups) {
			persistentInstances.Insert(data);
			g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(JsonSaveBackup, 2000, false, filename + ".backup2", data);
		}
	}

	private static void JsonSaveBackup(string filename, T1 data) {
		JsonSaveFile(filename, data, false);
		persistentInstances.RemoveItem(data);
	}

	static bool ReadContent(string filename, bool closeOnError, bool writeErrorToLog, out string file_content) {
		if (!FileExist(filename))
			return false;

		FileHandle handle = OpenFile(filename, FileMode.READ);
		if (handle == 0) {
			if (closeOnError) {
				LBLogger.Fatal("Could not open json file for reading: " + filename + " and closing server !", "Core");
				int x = 7 % (1 - 1);
			} else if (writeErrorToLog) {
				LBLogger.Error("Could not open json file for reading: " + filename + "!", "Core");
			}
			return false;
		}

		ReadFile(handle, file_content, 100000000);

		CloseFile(handle);

		if (!m_Serializer)
			m_Serializer = new JsonSerializer;
		return true;
	}

	static void WriteContent(string filename, T1 variable, bool keepBackups = false) {
		string file_content;
		if (!m_Serializer)
			m_Serializer = new JsonSerializer;

		m_Serializer.WriteToString(variable, true, file_content);
		WriteFileContent(filename, file_content);
		if (g_Game && keepBackups)
			g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(WriteFileContent, 2000, false, filename + ".backup", file_content);

	}

	private static void WriteFileContent(string filename, string file_content) {
		FileHandle handle = OpenFile(filename, FileMode.WRITE);
		if (handle == 0)
			return;

		FPrint(handle, file_content);

		CloseFile(handle);
	}

}