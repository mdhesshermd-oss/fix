#ifndef SHADOWFOX_CARLOCK_DISABLE
#define SHADOWFOX_CARLOCK_DISABLE

class ActionDetachCarLockPickCB : ActionContinuousBaseCB
{
    override void CreateActionComponent()
    {
        m_ActionData.m_ActionComponent = new CAContinuousTime(15);
    }
};

class ActionDetachCarLockPick : ActionContinuousBase
{
    void ActionDetachCarLockPick()
    {
        m_CallbackClass = ActionDetachCarLockPickCB;
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
        m_FullBody = true;
        m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
    }

    override void CreateConditionComponents()
    {
        m_ConditionTarget = new CCTObject(7);
        m_ConditionItem = new CCINonRuined;
    }

    override string GetText()
    {
        return "Pick Car Lock";
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        CarScript car = SHADOWFOX_CarLockTargetHelper.GetTargetCar(target);
        if (car && car.m_SF_OwnerId != -1 && car.m_SF_IsLocked)
        {
            auto config = GetSHADOWFOX_CarLockStandaloneConfig();
            foreach (string tool : config.LockPickTools)
            {
                if (item.IsKindOf(tool)) return true;
            }
        }
        return false;
    }

    override void OnFinishProgressServer(ActionData action_data)
    {
        CarScript car = SHADOWFOX_CarLockTargetHelper.GetTargetCar(action_data.m_Target);
        if (car)
        {
            auto config = GetSHADOWFOX_CarLockStandaloneConfig();
            if (Math.RandomInt(0, 100) < config.LockPickChance)
            {
                car.SetSF_CarLock(false);
                SHADOWFOX_CarLockNewLogger.Get().LogInfo("Lock picked successfully on " + car.GetDisplayName() + " by " + action_data.m_Player.GetIdentity().GetName());
            }
            else
            {
                SHADOWFOX_CarLockNewLogger.Get().LogInfo("Lock pick failed on " + car.GetDisplayName() + " by " + action_data.m_Player.GetIdentity().GetName());
            }
            action_data.m_MainItem.DecreaseHealth(20, false);
        }
    }
};

#endif
