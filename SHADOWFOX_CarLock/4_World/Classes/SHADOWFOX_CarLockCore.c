#ifndef SHADOWFOX_CARLOCK_DISABLE
#define SHADOWFOX_CARLOCK_DISABLE

class SHADOWFOX_CarLockCore
{
    ref SHADOWFOX_CarLockServer m_CarLockServer;

    void SHADOWFOX_CarLockCore()
    {
        SHADOWFOX_CarLockNewLogger.Get().LogInfo("SHADOWFOX_CarLockCore - Starting");
        if (GetGame().IsServer() || !GetGame().IsMultiplayer())
        {
            InitServerFolder();
            GetSHADOWFOX_CarLockStandaloneConfig();
            m_CarLockServer = new SHADOWFOX_CarLockServer;
        }
        else
        {
            InitClientFolder();
        }
        InitRPC();
    }

    void InitClientFolder()
    {
        if (!FileExist(SHADOWFOX_CL_CONFIG_ROOT_CLIENT))
        {
            MakeDirectory(SHADOWFOX_CL_CONFIG_ROOT_CLIENT);
        }
        if (!FileExist(SHADOWFOX_CL_CONFIG_DIR_CLIENT))
        {
            MakeDirectory(SHADOWFOX_CL_CONFIG_DIR_CLIENT);
        }
    }

    void InitServerFolder()
    {
        if (!FileExist(SHADOWFOX_CL_CONFIG_ROOT_SERVER))
        {
            MakeDirectory(SHADOWFOX_CL_CONFIG_ROOT_SERVER);
        }
        if (!FileExist(SHADOWFOX_CL_CONFIG_DIR_SERVER))
        {
            MakeDirectory(SHADOWFOX_CL_CONFIG_DIR_SERVER);
        }
    }

    void InitRPC()
    {
        if (GetGame().IsServer())
        {
            GetRPCManager().AddRPC("SHADOWFOX_CarLock", "SHADOWFOX_CarLockRequest", m_CarLockServer, SingleplayerExecutionType.Client);
            GetRPCManager().AddRPC("SHADOWFOX_CarLock", "SHADOWFOX_CarLockPasswordRequest", m_CarLockServer, SingleplayerExecutionType.Client);
        }
    }

    static void LogVehicleAction(string actionType, CarScript car, PlayerBase player)
    {
        if (!car || !player) return;
        string name = "Unknown";
        string uid = "Unknown";
        if (player.GetIdentity())
        {
            name = player.GetIdentity().GetName();
            uid = player.GetIdentity().GetPlainId();
        }
        string msg = actionType + " | " + car.GetDisplayName() + " | Player: " + name + " (" + uid + ") | Pos: " + car.GetPosition().ToString();
        SHADOWFOX_CarLockNewLogger.Get().Log(msg);
    }
};

static ref SHADOWFOX_CarLockCore g_SF_CarLockCore;
static SHADOWFOX_CarLockCore GetSHADOWFOX_CarLockCore()
{
    if (!g_SF_CarLockCore) g_SF_CarLockCore = new SHADOWFOX_CarLockCore;
    return g_SF_CarLockCore;
}

#endif
