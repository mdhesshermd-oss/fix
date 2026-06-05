#ifndef SHADOWFOX_CARLOCK_DISABLE
#define SHADOWFOX_CARLOCK_DISABLE

class SHADOWFOX_CarLockServer
{
    void SHADOWFOX_CarLockServer()
    {
        SHADOWFOX_CarLockNewLogger.Get().LogInfo("SHADOWFOX_CarLockServer - Started");
        Print("[SHADOWFOX_CarLock] SHADOWFOX_CarLockServer started");
    }

    void SHADOWFOX_CarLockRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        Print("[SHADOWFOX_CarLock] Server: Received SHADOWFOX_CarLockRequest from " + sender.GetName());
        if (!GetGame().IsServer()) return;
        Param2<CarScript, bool> params;
        if (!ctx.Read(params))
        {
            Print("[SHADOWFOX_CarLock] Server: Failed to read SHADOWFOX_CarLockRequest params");
            return;
        }

        PlayerBase player = SHADOWFOX_CarLockHelpers.GetPlayerBySteamID(sender.GetPlainId());
        CarScript car = params.param1;
        if (!car || !player)
        {
            Print("[SHADOWFOX_CarLock] Server: Car or Player object is null in request");
            return;
        }

        if (SHADOWFOX_CarLockHelpers.CanPlayerUseLock(player, car))
        {
            Print("[SHADOWFOX_CarLock] Server: Toggling lock for " + car.GetDisplayName() + " to " + params.param2);
            car.SetSF_CarLock(params.param2);
        }
    }

    void SHADOWFOX_CarLockPasswordRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        Print("[SHADOWFOX_CarLock] Server: Received SHADOWFOX_CarLockPasswordRequest from " + sender.GetName());
        if (!GetGame().IsServer()) return;
        Param2<CarScript, int> params;
        if (!ctx.Read(params))
        {
            Print("[SHADOWFOX_CarLock] Server: Failed to read SHADOWFOX_CarLockPasswordRequest params");
            return;
        }

        PlayerBase player = SHADOWFOX_CarLockHelpers.GetPlayerBySteamID(sender.GetPlainId());
        CarScript car = params.param1;
        if (!car || !player)
        {
            Print("[SHADOWFOX_CarLock] Server: Car or Player is null");
            return;
        }

        int enteredPassword = params.param2;
        bool isOwner = (car.m_SF_OwnerId == player.m_SF_LowUid);
        ScriptRPC rpc = new ScriptRPC();

        Print("[SHADOWFOX_CarLock] Server: Processing password for " + car.GetDisplayName() + ". Entered: " + enteredPassword + " IsOwner: " + isOwner);

        if (car.m_SF_Password == -1)
        {
            if (isOwner)
            {
                Print("[SHADOWFOX_CarLock] Server: New password set by owner");
                car.m_SF_Password = enteredPassword;
                car.SetSF_CarLock(true);
                SHADOWFOX_CarLockNewLogger.Get().LogInfo("Password set: " + sender.GetName() + " for " + car.GetDisplayName());

                rpc.Write(true);
                rpc.Write(enteredPassword);
                rpc.Write(car.m_SF_OwnerId);
                rpc.Send(player, 78910, true, player.GetIdentity());
            }
            else
            {
                Print("[SHADOWFOX_CarLock] Server: Non-owner tried to set password!");
            }
        }
        else if (isOwner)
        {
            // Owner is entering the password to toggle the lock
            if (enteredPassword == car.m_SF_Password)
            {
                Print("[SHADOWFOX_CarLock] Server: Owner verified password");
                car.SetSF_CarLock(!car.m_SF_IsLocked);

                rpc.Write(true);
                rpc.Write(enteredPassword);
                rpc.Write(car.m_SF_OwnerId);
                rpc.Send(player, 78911, true, player.GetIdentity());
            }
            else
            {
                Print("[SHADOWFOX_CarLock] Server: Owner entered wrong password");
                rpc.Write(false);
                rpc.Write(-1);
                rpc.Write(-1);
                rpc.Send(player, 78911, true, player.GetIdentity());
            }
        }
        else
        {
            if (enteredPassword == car.m_SF_Password)
            {
                Print("[SHADOWFOX_CarLock] Server: Password verified for non-owner");
                SHADOWFOX_CarLockNewLogger.Get().LogInfo("Password verified: " + sender.GetName() + " for " + car.GetDisplayName());

                car.SetSF_CarLock(false); // Unlock the car upon successful password entry

                rpc.Write(true);
                rpc.Write(enteredPassword);
                rpc.Write(car.m_SF_OwnerId);
                rpc.Send(player, 78910, true, player.GetIdentity());
            }
            else
            {
                Print("[SHADOWFOX_CarLock] Server: Wrong password entered");
                rpc.Write(false);
                rpc.Write(-1);
                rpc.Write(-1);
                rpc.Send(player, 78910, true, player.GetIdentity());
            }
        }
    }
};

#endif
