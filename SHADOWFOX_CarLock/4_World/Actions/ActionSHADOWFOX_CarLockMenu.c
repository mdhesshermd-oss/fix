#ifndef SHADOWFOX_CARLOCK
#define SHADOWFOX_CARLOCK

class ActionSHADOWFOX_CarLockMenu : ActionInteractBase
{
    void ActionSHADOWFOX_CarLockMenu()
    {
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
        return "Manage Vehicle Lock";
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        CarScript car = SHADOWFOX_CarLockTargetHelper.GetTargetCar(target);
        if (car && car.m_SF_OwnerId != -1) return true;
        return false;
    }

    override void OnStartClient(ActionData action_data)
    {
        CarScript car = SHADOWFOX_CarLockTargetHelper.GetTargetCar(action_data.m_Target);
        if (car)
        {
            if (!action_data.m_Player.m_SF_CarLockMenu)
                action_data.m_Player.m_SF_CarLockMenu = new SHADOWFOX_CarLockMenu(car);

            GetGame().GetUIManager().ShowScriptedMenu(action_data.m_Player.m_SF_CarLockMenu, null);
        }
    }
};

#endif
