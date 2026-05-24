#ifndef SHADOWFOX_CARLOCK_DISABLE
#define SHADOWFOX_CARLOCK_DISABLE

class ActionDetachCarLockCB : ActionContinuousBaseCB
{
    override void CreateActionComponent()
    {
        m_ActionData.m_ActionComponent = new CAContinuousTime(5);
    }
};

class ActionDetachCarLock : ActionContinuousBase
{
    void ActionDetachCarLock()
    {
        m_CallbackClass = ActionDetachCarLockCB;
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
        m_FullBody = true;
        m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
    }

    override void CreateConditionComponents()
    {
        m_ConditionTarget = new CCTObject(7);
        m_ConditionItem = new CCINone;
    }

    override string GetText()
    {
        return "#STR_shadowfox_carlock_detach_user";
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        CarScript car = SHADOWFOX_CarLockTargetHelper.GetTargetCar(target);
        if (car && car.m_SF_OwnerId != -1 && !car.m_SF_IsLocked)
        {
            if (car.m_SF_OwnerId == player.m_SF_LowUid || SHADOWFOX_CarLockHelpers.IsAdmin(player))
                return true;
        }
        return false;
    }

    override void OnFinishProgressServer(ActionData action_data)
    {
        CarScript car = SHADOWFOX_CarLockTargetHelper.GetTargetCar(action_data.m_Target);
        if (car)
        {
            SHADOWFOX_CarLockHelpers.CreateInInventory(action_data.m_Player, "CodeLock");
            car.m_SF_OwnerId = -1;
            car.m_SF_Password = -1;
            car.m_SF_OwnerName = "";
            car.m_SF_OwnerSteamId = "";
            car.SetSF_CarLock(false);
            SHADOWFOX_CarLockNewLogger.Get().LogInfo("Lock removed (CodeLock given) from " + car.GetDisplayName() + " by " + action_data.m_Player.GetIdentity().GetName());
        }
    }
};

#endif
