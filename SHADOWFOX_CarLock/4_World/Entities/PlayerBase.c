#ifndef SHADOWFOX_CARLOCK_DISABLE
#define SHADOWFOX_CARLOCK_DISABLE

modded class PlayerBase
{
    int m_SF_LowUid = -1;
    bool m_SF_IsAdmin = false;
    ref SHADOWFOX_CarLockMenu m_SF_CarLockMenu;

    override void Init()
    {
        super.Init();
        RegisterNetSyncVariableInt("m_SF_LowUid");
        RegisterNetSyncVariableBool("m_SF_IsAdmin");
    }

    void SF_SetLowSteamUID(string playerUID)
    {
        string low = "";
        for (int i = 8; i < 17; i++)
        {
            low += playerUID.Get(i);
        }
        m_SF_LowUid = low.ToInt();
        SetSynchDirty();
    }

    bool SF_HasPassword(int pwd, int ownerID)
    {
        if (GetGame().IsClient())
        {
            SHADOWFOX_CarLockData data = SHADOWFOX_CarLockData.Load(m_SF_LowUid.ToString());
            for (int i = 0; i < data.Passwords.Count(); i++)
            {
                if (data.Passwords[i] == pwd && data.Owners[i] == ownerID)
                    return true;
            }
        }
        return false;
    }

    override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        super.OnRPC(sender, rpc_type, ctx);
        if (GetGame().IsClient())
        {
            if (rpc_type == 78910 || rpc_type == 78911)
            {
                bool success;
                int pwd, ownerId;
                if (ctx.Read(success) && ctx.Read(pwd) && ctx.Read(ownerId))
                {
                    if (success)
                    {
                        SHADOWFOX_CarLockData data = SHADOWFOX_CarLockData.Load(m_SF_LowUid.ToString());
                        bool found = false;
                        for (int i = 0; i < data.Passwords.Count(); i++)
                        {
                            if (data.Owners[i] == ownerId) { data.Passwords[i] = pwd; found = true; break; }
                        }
                        if (!found)
                        {
                            data.Passwords.Insert(pwd);
                            data.Owners.Insert(ownerId);
                        }
                        data.Save(m_SF_LowUid.ToString());
                        if (m_SF_CarLockMenu) m_SF_CarLockMenu.Close();
                    }
                    else
                    {
                        if (m_SF_CarLockMenu) m_SF_CarLockMenu.SetOutputText("Wrong Password!");
                    }
                }
            }
        }
    }
};

#endif
