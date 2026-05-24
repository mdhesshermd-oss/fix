#ifndef SHADOWFOX_CARLOCK_DISABLE
#define SHADOWFOX_CARLOCK_DISABLE

class ActionDetachCarLockAdminCB : ActionContinuousBaseCB
{
    override void CreateActionComponent()
    {
        m_ActionData.m_ActionComponent = new CAContinuousTime(2);
    }
};

class ActionDetachCarLockAdmin : ActionContinuousBase
{
    void ActionDetachCarLockAdmin()
    {
        m_CallbackClass = ActionDetachCarLockAdminCB;
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
        return "Remove Lock (Admin)";
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        if (SHADOWFOX_CarLockHelpers.IsAdmin(player))
        {
            CarScript car = SHADOWFOX_CarLockTargetHelper.GetTargetCar(target);
            if (car && car.m_SF_OwnerId != -1) return true;
        }
        return false;
    }

    override void OnFinishProgressServer(ActionData action_data)
    {
        CarScript car = SHADOWFOX_CarLockTargetHelper.GetTargetCar(action_data.m_Target);
        if (car)
        {
            SHADOWFOX_CarLockHelpers.CreateInInventory(action_data.m_Player, "SHADOWFOX_CarLock");
            car.m_SF_OwnerId = -1;
            car.m_SF_Password = -1;
            car.m_SF_OwnerName = "";
            car.m_SF_OwnerSteamId = "";
            car.SetSF_CarLock(false);
            SHADOWFOX_CarLockNewLogger.Get().LogInfo("Lock removed by Admin: " + action_data.m_Player.GetIdentity().GetName());
        }
    }
};

#endif
