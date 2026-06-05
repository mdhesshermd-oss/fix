#ifndef SHADOWFOX_CARLOCK_DISABLE
#define SHADOWFOX_CARLOCK_DISABLE

modded class Lockpick
{
    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionDetachCarLockPick);
    }
};

modded class Screwdriver
{
    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionDetachCarLockPick);
    }
};

modded class Pliers
{
    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionDetachCarLockPick);
    }
};

#endif
