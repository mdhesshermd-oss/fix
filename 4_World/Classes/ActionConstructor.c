#ifndef SHADOWFOX_CARLOCK_DISABLE
#define SHADOWFOX_CARLOCK_DISABLE

modded class ActionConstructor
{
    override void RegisterActions(TTypenameArray actions)
    {
        super.RegisterActions(actions);
        actions.Insert(ActionSHADOWFOX_CarLock);
        actions.Insert(ActionSHADOWFOX_CarUnLock);
        actions.Insert(ActionSHADOWFOX_CarLockMenu);
        actions.Insert(ActionAttachCarLock);
        actions.Insert(ActionDetachCarLock);
        actions.Insert(ActionDetachCarLockAdmin);
        actions.Insert(ActionDetachCarLockPick);
    }
};

#endif
