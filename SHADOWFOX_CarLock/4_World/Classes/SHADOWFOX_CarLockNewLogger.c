#ifndef SHADOWFOX_CARLOCK_DISABLE
#define SHADOWFOX_CARLOCK_DISABLE

class SHADOWFOX_CarLockNewLogger
{
    private string m_LoggerPrefixName;

    void SHADOWFOX_CarLockNewLogger(string prefixname = "")
    {
        m_LoggerPrefixName = prefixname;
    }

    void Log(string txt)
    {
        if (GetGame().IsServer())
        {
            string date = SHADOWFOX_CarLockHelpers.GetCurrentDateTimeString();
            string filename = "Log_" + SHADOWFOX_CarLockHelpers.GetCurrentDateTimeString().Substring(0, 10) + ".log";
            string path = SHADOWFOX_CL_CONFIG_ROOT_SERVER + "Logs/" + filename;

            if (!FileExist(SHADOWFOX_CL_CONFIG_ROOT_SERVER + "Logs/"))
            {
                MakeDirectory(SHADOWFOX_CL_CONFIG_ROOT_SERVER + "Logs/");
            }

            FileHandle file = OpenFile(path, FileMode.APPEND);
            if (file != 0)
            {
                FPrintln(file, SHADOWFOX_CarLockHelpers.GetCurrentDateTimeString() + " | " + m_LoggerPrefixName + " | " + txt);
                CloseFile(file);
            }
        }
    }

    void LogInfo(string txt)
    {
        Log("INFO | " + txt);
    }

    void LogError(string txt)
    {
        Log("ERROR | " + txt);
    }

    static ref SHADOWFOX_CarLockNewLogger g_SF_Logger;
    static SHADOWFOX_CarLockNewLogger Get()
    {
        if (!g_SF_Logger) g_SF_Logger = new SHADOWFOX_CarLockNewLogger("SHADOWFOX_CarLock");
        return g_SF_Logger;
    }
};

#endif
