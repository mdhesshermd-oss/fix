#ifndef SHADOWFOX_CARLOCK
#define SHADOWFOX_CARLOCK

class SHADOWFOX_CarLockData
{
    ref TIntArray Passwords;
    ref TIntArray Owners;

    void SHADOWFOX_CarLockData()
    {
        Passwords = new TIntArray;
        Owners = new TIntArray;
    }

    void Save(string id)
    {
        string path = SHADOWFOX_CL_CONFIG_DIR_CLIENT + id + ".json";
        if (!FileExist(SHADOWFOX_CL_CONFIG_DIR_CLIENT))
        {
            MakeDirectory(SHADOWFOX_CL_CONFIG_DIR_CLIENT);
        }
        JsonFileLoader<SHADOWFOX_CarLockData>.JsonSaveFile(path, this);
    }

    static ref SHADOWFOX_CarLockData Load(string id)
    {
        SHADOWFOX_CarLockData data = new SHADOWFOX_CarLockData();
        string path = SHADOWFOX_CL_CONFIG_DIR_CLIENT + id + ".json";
        if (FileExist(path))
        {
            JsonFileLoader<SHADOWFOX_CarLockData>.JsonLoadFile(path, data);
        }
        return data;
    }
};

#endif
