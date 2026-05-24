#ifndef SHADOWFOX_CARLOCK_DISABLE
#define SHADOWFOX_CARLOCK_DISABLE

class SHADOWFOX_CarLockStandaloneConfig
{
    int LockPickChance = 50;
    ref array<string> Admins;
    ref array<string> LockPickTools;
    bool EnableAutoLock = true;
    int AutoLockDistance = 100;
    bool EnablePasswordSharing = true;
    bool EnablePlayerDataTracking = true;

    void SHADOWFOX_CarLockStandaloneConfig()
    {
        Admins = new array<string>();
        LockPickTools = new array<string>();
        // Defaults
        LockPickTools.Insert("Lockpick");
        LockPickTools.Insert("Screwdriver");
    }

    static ref SHADOWFOX_CarLockStandaloneConfig Load()
    {
        SHADOWFOX_CarLockStandaloneConfig config = new SHADOWFOX_CarLockStandaloneConfig();
        string path = SHADOWFOX_CL_CONFIG_ROOT_SERVER + "Settings.json";
        if (FileExist(path))
        {
            JsonFileLoader<SHADOWFOX_CarLockStandaloneConfig>.JsonLoadFile(path, config);
        }
        else
        {
            config.Save();
        }
        return config;
    }

    void Save()
    {
        if (!FileExist(SHADOWFOX_CL_CONFIG_ROOT_SERVER))
        {
            MakeDirectory(SHADOWFOX_CL_CONFIG_ROOT_SERVER);
        }
        JsonFileLoader<SHADOWFOX_CarLockStandaloneConfig>.JsonSaveFile(SHADOWFOX_CL_CONFIG_ROOT_SERVER + "Settings.json", this);
    }
};

static ref SHADOWFOX_CarLockStandaloneConfig g_SF_Config;
static SHADOWFOX_CarLockStandaloneConfig GetSHADOWFOX_CarLockStandaloneConfig()
{
    if (!g_SF_Config) g_SF_Config = SHADOWFOX_CarLockStandaloneConfig.Load();
    return g_SF_Config;
}

#endif
