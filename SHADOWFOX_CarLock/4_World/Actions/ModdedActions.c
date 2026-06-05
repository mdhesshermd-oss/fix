#ifndef SHADOWFOX_CARLOCK
#define SHADOWFOX_CARLOCK

modded class ActionGetInTransport
{
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        CarScript car = SHADOWFOX_CarLockTargetHelper.GetTargetCar(target);
        if (car && car.m_SF_IsLocked) return false;
        return super.ActionCondition(player, target, item);
    }
};

modded class ActionCarDoors
{
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        CarScript car = SHADOWFOX_CarLockTargetHelper.GetTargetCar(target);
        if (car && car.m_SF_IsLocked) return false;
        return super.ActionCondition(player, target, item);
    }
};

modded class ActionCarDoorsOutside
{
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        CarScript car = SHADOWFOX_CarLockTargetHelper.GetTargetCar(target);
        if (car && car.m_SF_IsLocked) return false;
        return super.ActionCondition(player, target, item);
    }
};

modded class ActionDetach
{
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        CarScript car = SHADOWFOX_CarLockTargetHelper.GetTargetCar(target);
        if (car && car.m_SF_IsLocked) return false;
        return super.ActionCondition(player, target, item);
    }
};

modded class ActionDetachFromTarget
{
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        CarScript car = SHADOWFOX_CarLockTargetHelper.GetTargetCar(target);
        if (car && car.m_SF_IsLocked) return false;
        return super.ActionCondition(player, target, item);
    }
};

#endif
