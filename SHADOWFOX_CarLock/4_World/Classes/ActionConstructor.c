#ifndef SHADOWFOX_CARLOCK
#define SHADOWFOX_CARLOCK

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
