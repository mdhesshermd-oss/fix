#ifndef SHADOWFOX_CARLOCK_DISABLE
#define SHADOWFOX_CARLOCK_DISABLE

class ActionSHADOWFOX_CarLockCB : ActionContinuousBaseCB
{
    override void CreateActionComponent()
    {
        m_ActionData.m_ActionComponent = new CAContinuousTime(1);
    }
};

class ActionSHADOWFOX_CarLock : ActionContinuousBase
{
    void ActionSHADOWFOX_CarLock()
    {
        m_CallbackClass = ActionSHADOWFOX_CarLockCB;
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
        m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
    }

    override void CreateConditionComponents()
    {
        m_ConditionTarget = new CCTObject(7);
        m_ConditionItem = new CCINone;
    }

    override string GetText()
    {
        return "#STR_shadowfox_carlock_lock_user";
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        if (GetGame().IsServer()) return true;
        CarScript car = SHADOWFOX_CarLockTargetHelper.GetTargetCar(target);
        if (!car || car.m_SF_OwnerId == -1) return false;
        if (!car.m_SF_IsLocked && SHADOWFOX_CarLockHelpers.CanPlayerUseLock(player, car))
        {
            return true;
        }
        return false;
    }

    override void OnStartServer(ActionData action_data)
    {
        CarScript car = SHADOWFOX_CarLockTargetHelper.GetTargetCar(action_data.m_Target);
        if (car)
        {
            car.SetSF_CarLock(true);
            car.SetSF_SoundToPlay(1);
            SHADOWFOX_CarLockNewLogger.Get().LogInfo("Vehicle Locked by " + action_data.m_Player.GetIdentity().GetName());
        }
    }
};

#endif
