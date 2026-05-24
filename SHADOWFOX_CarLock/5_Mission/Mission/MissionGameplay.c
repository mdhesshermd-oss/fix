#ifndef SHADOWFOX_CARLOCK_DISABLE
#define SHADOWFOX_CARLOCK_DISABLE

modded class MissionGameplay
{
    override void OnInit()
    {
        super.OnInit();
        GetSHADOWFOX_CarLockCore();
    }

    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);

        if (GetUApi().GetInputByName("SHADOWFOX_CarLockToggle").LocalPress())
        {
            TryToggleVehicleLock();
        }
    }

    void TryToggleVehicleLock()
    {
        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
        if (!player) return;

        CarScript car;
        // Search for nearby vehicle
        array<Object> objects = new array<Object>;
        GetGame().GetObjectsAtPosition3D(player.GetPosition(), 5.0, objects, null);

        foreach (Object obj : objects)
        {
            if (Class.CastTo(car, obj))
            {
                if (SHADOWFOX_CarLockHelpers.CanPlayerUseLock(player, car))
                {
                    bool targetState = !car.m_SF_IsLocked;
                    GetRPCManager().SendRPC("SHADOWFOX_CarLock", "SHADOWFOX_CarLockRequest", new Param2<CarScript, bool>(car, targetState), true);
                    return;
                }
            }
        }
    }
};

#endif
