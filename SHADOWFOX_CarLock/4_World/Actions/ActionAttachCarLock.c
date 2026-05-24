#ifndef SHADOWFOX_CARLOCK_DISABLE
#define SHADOWFOX_CARLOCK_DISABLE

class ActionAttachCarLockCB : ActionContinuousBaseCB
{
    override void CreateActionComponent()
    {
        m_ActionData.m_ActionComponent = new CAContinuousTime(3);
    }
};

class ActionAttachCarLock : ActionContinuousBase
{
    void ActionAttachCarLock()
    {
        m_CallbackClass = ActionAttachCarLockCB;
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
        return "Install Lock";
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        CarScript car = SHADOWFOX_CarLockTargetHelper.GetTargetCar(target);
        if (car && car.m_SF_OwnerId == -1)
        {
            return true;
        }
        return false;
    }

    override void OnFinishProgressServer(ActionData action_data)
    {
        CarScript car = SHADOWFOX_CarLockTargetHelper.GetTargetCar(action_data.m_Target);
        if (car)
        {
            car.m_SF_OwnerId = action_data.m_Player.m_SF_LowUid;
            car.m_SF_OwnerName = action_data.m_Player.GetIdentity().GetName();
            car.m_SF_OwnerSteamId = action_data.m_Player.GetIdentity().GetPlainId();
            car.SetSF_CarLock(false);

            action_data.m_MainItem.Delete();
            SHADOWFOX_CarLockNewLogger.Get().LogInfo("Lock attached to " + car.GetDisplayName() + " by " + car.m_SF_OwnerName);
        }
    }
};

#endif
