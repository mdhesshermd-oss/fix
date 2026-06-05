#ifndef SHADOWFOX_CARLOCK_DISABLE
#define SHADOWFOX_CARLOCK_DISABLE

class SHADOWFOX_CarLockTargetHelper
{
    static CarScript GetTargetCar(ActionTarget target)
    {
        CarScript car;
        if (Class.CastTo(car, target.GetObject()))
            return car;
        if (Class.CastTo(car, target.GetParent()))
            return car;
        return null;
    }
};

#endif
