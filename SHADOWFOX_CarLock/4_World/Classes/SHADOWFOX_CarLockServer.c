#ifndef SHADOWFOX_CARLOCK_DISABLE
#define SHADOWFOX_CARLOCK_DISABLE

class SHADOWFOX_CarLockServer
{
    void SHADOWFOX_CarLockServer()
    {
        SHADOWFOX_CarLockNewLogger.Get().LogInfo("SHADOWFOX_CarLockServer - Started");
    }

    void SHADOWFOX_CarLockRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!GetGame().IsServer()) return;
        Param2<CarScript, bool> params;
        if (!ctx.Read(params)) return;

        CarScript car = params.param1;
        if (!car) return;

        car.SetSF_CarLock(params.param2);
        if (params.param2) car.SetSF_SoundToPlay(1);
        else car.SetSF_SoundToPlay(2);
    }

    void SHADOWFOX_CarLockPasswordRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!GetGame().IsServer()) return;
        Param2<CarScript, int> params;
        if (!ctx.Read(params)) return;

        PlayerBase player = SHADOWFOX_CarLockHelpers.GetPlayerBySteamID(sender.GetPlainId());
        CarScript car = params.param1;
        if (!car || !player) return;

        int enteredPassword = params.param2;
        bool isOwner = (car.m_SF_OwnerId == player.m_SF_LowUid);
        ScriptRPC rpc = new ScriptRPC();

        if (car.m_SF_Password == -1)
        {
            if (isOwner)
            {
                car.m_SF_Password = enteredPassword;
                car.SetSF_CarLock(true);
                SHADOWFOX_CarLockNewLogger.Get().LogInfo("Password set: " + sender.GetName() + " for " + car.GetDisplayName());

                EntityAI itemInHands = player.GetHumanInventory().GetEntityInHands();
                if (itemInHands && itemInHands.IsKindOf("SHADOWFOX_CarLock"))
                {
                    itemInHands.Delete();
                }

                rpc.Write(true);
                rpc.Write(enteredPassword);
                rpc.Write(car.m_SF_OwnerId);
                rpc.Send(player, 78910, true, player.GetIdentity());
            }
        }
        else if (isOwner)
        {
            car.m_SF_Password = enteredPassword;
            SHADOWFOX_CarLockNewLogger.Get().LogInfo("Password changed: " + sender.GetName() + " for " + car.GetDisplayName());
            rpc.Write(true);
            rpc.Write(enteredPassword);
            rpc.Write(car.m_SF_OwnerId);
            rpc.Send(player, 78911, true, player.GetIdentity());
        }
        else
        {
            if (enteredPassword == car.m_SF_Password)
            {
                SHADOWFOX_CarLockNewLogger.Get().LogInfo("Password verified: " + sender.GetName() + " for " + car.GetDisplayName());
                rpc.Write(true);
                rpc.Write(enteredPassword);
                rpc.Write(car.m_SF_OwnerId);
                rpc.Send(player, 78910, true, player.GetIdentity());
            }
            else
            {
                rpc.Write(false);
                rpc.Write(-1);
                rpc.Write(-1);
                rpc.Send(player, 78910, true, player.GetIdentity());
            }
        }
    }
};

#endif
