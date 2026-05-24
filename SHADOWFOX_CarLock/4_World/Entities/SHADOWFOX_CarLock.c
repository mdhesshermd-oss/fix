#ifndef SHADOWFOX_CARLOCK_DISABLE
#define SHADOWFOX_CARLOCK_DISABLE

class SHADOWFOX_CarLock : ItemBase
{
    override void Init()
    {
        super.Init();
        Print("[SHADOWFOX_CarLock] Init called for SHADOWFOX_CarLock item");
    }

    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionAttachCarLock);
        AddAction(ActionSHADOWFOX_CarLockMenu);
    }
}

#endif
