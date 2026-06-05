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
            if (item && item.IsKindOf("Lockpick")) return true;
        }
        return false;
    }

    override void OnStartServer(ActionData action_data)
    {
        super.OnStartServer(action_data);
        CarScript car = SHADOWFOX_CarLockTargetHelper.GetTargetCar(action_data.m_Target);
        if (car && car.m_SF_OwnerSteamId != "")
        {
            PlayerBase owner = SHADOWFOX_CarLockHelpers.GetPlayerBySteamID(car.m_SF_OwnerSteamId);
            if (owner && owner.GetIdentity())
            {
                GetGame().RPCSingleParam(owner, ERPCs.RPC_USER_ACTION_MESSAGE, new Param1<string>("Your vehicle is being lockpicked!"), true, owner.GetIdentity());
            }
        }
    }

    override void OnFinishProgressServer(ActionData action_data)
    {
        CarScript car = SHADOWFOX_CarLockTargetHelper.GetTargetCar(action_data.m_Target);
        if (car)
        {
            // 40% success chance
            if (Math.RandomInt(0, 100) < 40)
            {
                car.SetSF_CarLock(false);
                SHADOWFOX_CarLockNewLogger.Get().LogInfo("Lock picked successfully on " + car.GetDisplayName() + " by " + action_data.m_Player.GetIdentity().GetName());

                PlayerBase owner = SHADOWFOX_CarLockHelpers.GetPlayerBySteamID(car.m_SF_OwnerSteamId);
                if (owner && owner.GetIdentity())
                {
                    GetGame().RPCSingleParam(owner, ERPCs.RPC_USER_ACTION_MESSAGE, new Param1<string>("Your vehicle has been lockpicked!"), true, owner.GetIdentity());
                }
            }
            else
            {
                SHADOWFOX_CarLockNewLogger.Get().LogInfo("Lock pick failed on " + car.GetDisplayName() + " by " + action_data.m_Player.GetIdentity().GetName());
            }

            // Lockpick breaks after use
            if (action_data.m_MainItem)
                action_data.m_MainItem.Delete();
        }
    }
};

#endif
