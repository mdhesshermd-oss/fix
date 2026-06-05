#ifndef SHADOWFOX_CARLOCK
#define SHADOWFOX_CARLOCK

modded class MissionServer
{
    override void OnInit()
    {
        super.OnInit();
        GetSHADOWFOX_CarLockCore();
    }

    override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity)
    {
        super.InvokeOnConnect(player, identity);
        if (player)
        {
            player.SF_SetLowSteamUID(identity.GetPlainId());
            player.m_SF_IsAdmin = SHADOWFOX_CarLockHelpers.CheckAdminBySteamID(identity.GetPlainId());
            player.SetSynchDirty();
        }
    }
};

#endif
