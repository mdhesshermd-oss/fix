#ifndef SHADOWFOX_CARLOCK_DISABLE
#define SHADOWFOX_CARLOCK_DISABLE

modded class ActionConstructor
{
    override void RegisterActions(TTypenameArray actions)
    {
        super.RegisterActions(actions);
        actions.Insert(ActionSHADOWFOX_CarLockMenu);
        actions.Insert(ActionDetachCarLockPick);
    }
};

#endif
