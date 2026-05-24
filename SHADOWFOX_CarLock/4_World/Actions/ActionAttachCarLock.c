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
        return "#STR_shadowfox_carlock_install_lock";
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        CarScript car = SHADOWFOX_CarLockTargetHelper.GetTargetCar(target);
        if (car && car.m_SF_OwnerId == -1)
        {
            if (item && item.IsKindOf("CodeLock")) return true;
        }
        return false;
    }

    override void OnFinishProgressServer(ActionData action_data)
    {
        CarScript car = SHADOWFOX_CarLockTargetHelper.GetTargetCar(action_data.m_Target);
        if (car)
        {
            SHADOWFOX_CarLockNewLogger.Get().LogInfo("OnFinishProgressServer: Attaching lock to " + car.GetDisplayName());
            car.m_SF_OwnerId = action_data.m_Player.m_SF_LowUid;
            car.m_SF_OwnerName = action_data.m_Player.GetIdentity().GetName();
            car.m_SF_OwnerSteamId = action_data.m_Player.GetIdentity().GetPlainId();
            car.SetSF_CarLock(false);

            if (action_data.m_MainItem)
            {
                SHADOWFOX_CarLockNewLogger.Get().LogInfo("Deleting CodeLock item from hands");
                action_data.m_MainItem.Delete();
            }
            SHADOWFOX_CarLockNewLogger.Get().LogInfo("Lock attachment sequence complete");
        }
        else
        {
            SHADOWFOX_CarLockNewLogger.Get().LogError("OnFinishProgressServer: Target car not found!");
        }
    }
};

#endif
