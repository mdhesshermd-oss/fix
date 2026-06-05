#ifndef SHADOWFOX_CARLOCK_DISABLE
#define SHADOWFOX_CARLOCK_DISABLE

modded class VicinityItemManager
{
    override bool CanShowItemInVicinity(EntityAI item)
    {
        CarScript car;
        if (Class.CastTo(car, item))
        {
            if (car.m_SF_IsLocked) return false;
        }
        return super.CanShowItemInVicinity(item);
    }
};

#endif
