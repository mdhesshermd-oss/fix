#ifndef SHADOWFOX_CARLOCK_DISABLE
#define SHADOWFOX_CARLOCK_DISABLE

modded class CarScript
{
    bool m_SF_IsLocked = false;
    int m_SF_OwnerId = -1;
    int m_SF_Password = -1;
    string m_SF_OwnerName = "";
    string m_SF_OwnerSteamId = "";
    int m_SF_SoundToPlay = 0;

    void CarScript()
    {
        RegisterNetSyncVariableBool("m_SF_IsLocked");
        RegisterNetSyncVariableInt("m_SF_OwnerId");
        RegisterNetSyncVariableInt("m_SF_Password");
        RegisterNetSyncVariableInt("m_SF_SoundToPlay");

        if (GetGame().IsServer())
        {
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.SF_CarLockLogInit, 10, false);
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.SF_CheckAutoLock, 5000, true);
        }
    }

    void SF_CarLockLogInit()
    {
        if (m_SF_OwnerId != -1 && m_SF_Password != -1)
        {
            SHADOWFOX_CarLockNewLogger.Get().LogInfo("CarLock Initialized: " + GetDisplayName() + " Owner: " + m_SF_OwnerName + " (" + m_SF_OwnerSteamId + ") at " + GetPosition().ToString());
        }
    }

    override void EEDelete(EntityAI parent)
    {
        super.EEDelete(parent);
        if (GetGame().IsServer() && m_SF_OwnerId != -1)
        {
            SHADOWFOX_CarLockNewLogger.Get().LogInfo("CarLock Deleted: " + GetDisplayName() + " Owner: " + m_SF_OwnerName + " at " + GetPosition().ToString());
        }
    }

    override void OnVariablesSynchronized()
    {
        super.OnVariablesSynchronized();
        if (m_SF_SoundToPlay == 1) PlaySF_LockSound();
        else if (m_SF_SoundToPlay == 2) PlaySF_UnlockSound();
        else if (m_SF_SoundToPlay == 3) PlaySF_AlarmSound();
        m_SF_SoundToPlay = 0;
    }

    void SetSF_SoundToPlay(int id)
    {
        m_SF_SoundToPlay = id;
        SetSynchDirty();
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
        if (!GetGame().IsServer()) return;
        auto config = GetSHADOWFOX_CarLockStandaloneConfig();
        if (!config.EnablePlayerDataTracking || m_SF_OwnerSteamId == "") return;

        SHADOWFOX_CarLockPlayerData data = SHADOWFOX_CarLockPlayerData.Get(m_SF_OwnerSteamId);
        if (data)
        {
            data.AddOrUpdateVehicle(GetDisplayName(), m_SF_Password, GetPosition(), GetType());
        }
    }

    void SF_CheckAutoLock()
    {
        if (!GetGame().IsServer()) return;
        auto config = GetSHADOWFOX_CarLockStandaloneConfig();
        if (!config.EnableAutoLock || m_SF_IsLocked || m_SF_OwnerId == -1) return;

        PlayerBase owner = SHADOWFOX_CarLockHelpers.GetPlayerBySteamID(m_SF_OwnerSteamId);
        if (!owner)
        {
            SetSF_CarLock(true);
            SetSF_SoundToPlay(1);
            return;
        }

        float dist = vector.Distance(GetPosition(), owner.GetPosition());
        if (dist > config.AutoLockDistance)
        {
            SetSF_CarLock(true);
            SetSF_SoundToPlay(1);
        }
    }

    void PlaySF_LockSound()
    {
        EffectSound sound = SEffectManager.PlaySound("SHADOWFOX_CarLock_SoundSet", GetPosition());
        if (sound) sound.SetAutodestroy(true);
    }

    void PlaySF_UnlockSound()
    {
        EffectSound sound = SEffectManager.PlaySound("SHADOWFOX_CarUnlock_SoundSet", GetPosition());
        if (sound) sound.SetAutodestroy(true);
    }

    void PlaySF_AlarmSound()
    {
        EffectSound sound = SEffectManager.PlaySound("SHADOWFOX_CarAlarm_SoundSet", GetPosition());
        if (sound) sound.SetAutodestroy(true);
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

    string GetCarLockOwnerName() { return m_SF_OwnerName; }
    string GetCarLockFullSteamID() { return m_SF_OwnerSteamId; }
};

#endif
