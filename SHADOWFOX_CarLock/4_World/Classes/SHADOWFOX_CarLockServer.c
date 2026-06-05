#ifndef SHADOWFOX_CARLOCK
#define SHADOWFOX_CARLOCK

class SHADOWFOX_CarLockServer
{
    void SHADOWFOX_CarLockServer()
    {
        SHADOWFOX_CarLockNewLogger.Get().LogInfo("SHADOWFOX_CarLockServer - Started");
        Print("[SHADOWFOX_CarLock] SHADOWFOX_CarLockServer started");
    }

    void SHADOWFOX_CarLockRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!GetGame().IsServer()) return;
        Param2<CarScript, bool> params;
        if (!ctx.Read(params)) return;

        PlayerBase player = SHADOWFOX_CarLockHelpers.GetPlayerBySteamID(sender.GetPlainId());
        CarScript car = params.param1;
        if (!car || !player) return;

        if (SHADOWFOX_CarLockHelpers.CanPlayerUseLock(player, car))
        {
            car.SetSF_CarLock(params.param2);
        }
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
                SHADOWFOX_CarLockNewLogger.Get().LogInfo("Password set by owner: " + sender.GetName());

                rpc.Write(true);
                rpc.Write(enteredPassword);
                rpc.Write(car.m_SF_OwnerId);
                rpc.Send(player, 78910, true, player.GetIdentity());
            }
        }
        else if (isOwner)
        {
            // Owner can change password or toggle lock
            // For simplicity, if they enter a different password, we treat it as a change.
            // If they enter the same password, we toggle the lock.
            if (enteredPassword == car.m_SF_Password)
            {
                car.SetSF_CarLock(!car.m_SF_IsLocked);
            }
            else
            {
                car.m_SF_Password = enteredPassword;
                car.SetSF_CarLock(true); // Lock it with new password
                SHADOWFOX_CarLockNewLogger.Get().LogInfo("Password changed by owner: " + sender.GetName());
            }

            rpc.Write(true);
            rpc.Write(enteredPassword);
            rpc.Write(car.m_SF_OwnerId);
            rpc.Send(player, 78911, true, player.GetIdentity());
        }
        else
        {
            if (enteredPassword == car.m_SF_Password)
            {
                car.SetSF_CarLock(false);
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
