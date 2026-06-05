#ifndef SHADOWFOX_CARLOCK
#define SHADOWFOX_CARLOCK

modded class CarScript
{
    bool m_SF_IsLocked = false;
    int m_SF_OwnerId = -1;
    int m_SF_Password = -1;
    string m_SF_OwnerName = "";
    string m_SF_OwnerSteamId = "";

    void CarScript()
    {
        RegisterNetSyncVariableBool("m_SF_IsLocked");
        RegisterNetSyncVariableInt("m_SF_OwnerId");
        // Removed password from NetSync for security

        if (GetGame().IsServer())
        {
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.SF_CheckAutoLock, 5000, true);
        }
    }

    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionSHADOWFOX_CarLockMenu);
        AddAction(ActionDetachCarLockPick);
    }

    override void OnEngineStart()
    {
        super.OnEngineStart();
        if (GetGame().IsServer())
        {
            Human driver = GetDriver();
            PlayerBase player;
            if (Class.CastTo(player, driver))
            {
                if (m_SF_OwnerId == -1)
                {
                    m_SF_OwnerId = player.m_SF_LowUid;
                    m_SF_OwnerName = player.GetIdentity().GetName();
                    m_SF_OwnerSteamId = player.GetIdentity().GetPlainId();
                    SetSynchDirty();
                }
            }
        }
    }

    void SetSF_CarLock(bool locked)
    {
        m_SF_IsLocked = locked;
        if (GetGame().IsServer())
        {
            if (GetInventory())
            {
                if (m_SF_IsLocked) GetInventory().LockInventory(HIDE_INV_FROM_SCRIPT);
                else GetInventory().UnlockInventory(HIDE_INV_FROM_SCRIPT);
            }
            UpdateSF_PlayerData();
        }
        SetSynchDirty();
    }

    void UpdateSF_PlayerData()
    {
        if (!GetGame().IsServer() || m_SF_OwnerSteamId == "") return;
        SHADOWFOX_CarLockPlayerData data = SHADOWFOX_CarLockPlayerData.Get(m_SF_OwnerSteamId);
        if (data)
        {
            data.AddOrUpdateVehicle(GetDisplayName(), m_SF_Password, GetPosition(), GetType());
        }
    }

    void SF_CheckAutoLock()
    {
        if (!GetGame().IsServer() || m_SF_IsLocked || m_SF_OwnerId == -1) return;
        auto config = GetSHADOWFOX_CarLockStandaloneConfig();
        if (!config.EnableAutoLock) return;

        PlayerBase owner = SHADOWFOX_CarLockHelpers.GetPlayerBySteamID(m_SF_OwnerSteamId);
        if (!owner)
        {
            SetSF_CarLock(true);
            return;
        }

        float dist = vector.Distance(GetPosition(), owner.GetPosition());
        if (dist > config.AutoLockDistance)
        {
            SetSF_CarLock(true);
        }
    }

    override void OnStoreSave(ParamsWriteContext ctx)
    {
        super.OnStoreSave(ctx);
        ctx.Write(m_SF_IsLocked);
        ctx.Write(m_SF_OwnerId);
        ctx.Write(m_SF_Password);
        ctx.Write(m_SF_OwnerName);
        ctx.Write(m_SF_OwnerSteamId);
    }

    override bool OnStoreLoad(ParamsReadContext ctx, int version)
    {
        if (!super.OnStoreLoad(ctx, version)) return false;
        if (!ctx.Read(m_SF_IsLocked)) return false;
        if (!ctx.Read(m_SF_OwnerId)) return false;
        if (!ctx.Read(m_SF_Password)) return false;
        if (!ctx.Read(m_SF_OwnerName)) return false;
        if (!ctx.Read(m_SF_OwnerSteamId)) return false;

        if (GetGame().IsServer())
        {
            if (m_SF_IsLocked && GetInventory())
                GetInventory().LockInventory(HIDE_INV_FROM_SCRIPT);
        }
        return true;
    }
};

#endif
