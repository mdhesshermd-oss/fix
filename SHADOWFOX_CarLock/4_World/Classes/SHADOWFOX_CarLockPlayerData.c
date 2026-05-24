#ifndef SHADOWFOX_CARLOCK_DISABLE
#define SHADOWFOX_CARLOCK_DISABLE

class SHADOWFOX_VehicleRecordData
{
    int VehiclePassword;
    string FirstLockTime;
    vector LastKnownPosition;
    string LastUpdateTime;
    string VehicleType;

    void SHADOWFOX_VehicleRecordData()
    {
        VehiclePassword = -1;
        FirstLockTime = "";
        LastKnownPosition = "0 0 0";
        LastUpdateTime = "";
        VehicleType = "";
    }
}

class SHADOWFOX_CarLockPlayerData
{
    string SteamID;
    string PlayerName;
    ref map<string, ref SHADOWFOX_VehicleRecordData> Vehicles;

    void SHADOWFOX_CarLockPlayerData()
    {
        SteamID = "";
        PlayerName = "";
        Vehicles = new map<string, ref SHADOWFOX_VehicleRecordData>();
    }

    static string GetFilePath(string steamID)
    {
        return SHADOWFOX_CL_CONFIG_DIR_SERVER + steamID + ".json";
    }

    static ref SHADOWFOX_CarLockPlayerData Load(string steamID)
    {
        SHADOWFOX_CarLockPlayerData data = new SHADOWFOX_CarLockPlayerData();
        data.SteamID = steamID;
        string path = GetFilePath(steamID);
        if (FileExist(path))
        {
            JsonFileLoader<SHADOWFOX_CarLockPlayerData>.JsonLoadFile(path, data);
        }
        return data;
    }

    void Save()
    {
        string path = GetFilePath(SteamID);
        if (!FileExist(SHADOWFOX_CL_CONFIG_DIR_SERVER))
        {
            MakeDirectory(SHADOWFOX_CL_CONFIG_DIR_SERVER);
        }
        JsonFileLoader<SHADOWFOX_CarLockPlayerData>.JsonSaveFile(path, this);
    }

    void AddOrUpdateVehicle(string vehicleID, int password, vector position, string vehicleType)
    {
        SHADOWFOX_VehicleRecordData record;
        if (Vehicles.Find(vehicleID, record))
        {
            record.VehiclePassword = password;
            record.LastKnownPosition = position;
            record.LastUpdateTime = SHADOWFOX_CarLockHelpers.GetCurrentDateTimeString();
            record.VehicleType = vehicleType;
        }
        else
        {
            record = new SHADOWFOX_VehicleRecordData();
            record.VehiclePassword = password;
            record.FirstLockTime = SHADOWFOX_CarLockHelpers.GetCurrentDateTimeString();
            record.LastKnownPosition = position;
            record.LastUpdateTime = record.FirstLockTime;
            record.VehicleType = vehicleType;
            Vehicles.Insert(vehicleID, record);
        }
        Save();
    }

    static ref SHADOWFOX_CarLockPlayerData Get(string steamID)
    {
        return GetSHADOWFOX_PlayerData(steamID);
    }
}

static ref map<string, ref SHADOWFOX_CarLockPlayerData> g_SF_PlayerDataCache;
static SHADOWFOX_CarLockPlayerData GetSHADOWFOX_PlayerData(string steamID)
{
    if (!g_SF_PlayerDataCache) g_SF_PlayerDataCache = new map<string, ref SHADOWFOX_CarLockPlayerData>();
    SHADOWFOX_CarLockPlayerData data;
    if (g_SF_PlayerDataCache.Find(steamID, data)) return data;
    data = SHADOWFOX_CarLockPlayerData.Load(steamID);
    g_SF_PlayerDataCache.Insert(steamID, data);
    return data;
}

#endif
