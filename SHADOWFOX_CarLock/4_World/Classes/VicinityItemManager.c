#ifndef SHADOWFOX_CARLOCK
#define SHADOWFOX_CARLOCK

modded class VicinityItemManager
{
    override bool IsObjectIgnored(Object obj)
    {
        // Don't ignore cars in vicinity so they can be interacted with,
        // but the car's inventory locking logic in CarScript will handle the actual locking.
        return super.IsObjectIgnored(obj);
    }
};

#endif
