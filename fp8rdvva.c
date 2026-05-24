#ifndef ALECARLOCKDISABLE
class ActionALECarLockCB:ActionContinuousBaseCB
{
override void CreateActionComponent()
{
m_ActionData.m_ActionComponent=new CAContinuousTime(1);
}
};
class ActionALECarLock:ActionContinuousBase
{
void ActionALECarLock()
{
m_CallbackClass=ActionALECarLockCB;
m_CommandUID=DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
m_StanceMask=DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
}
override void CreateConditionComponents()
{
m_ConditionTarget=new CCTObject(7);
m_ConditionItem=new CCINone;
}
override string GetText()
{
PlayerBase FUu9FfWr5Ytw6zW=PlayerBase.Cast(GetGame().GetPlayer());
if(FUu9FfWr5Ytw6zW && ALECarLockHelpers.IsAdmin(FUu9FfWr5Ytw6zW))
{
return wRKnycSAotPtzb("307B4AAB9BAD8C6C4D4779A5A3BA4C975E80BB6DBA79BBAD91AF", ("68AaRrrORwP8go".Hash() + __LINE__.ToInt()));
}
return wRKnycSAotPtzb("295086965A9A96C29F5EA35371845769B87BB19584A0AE94B8", ("pmQCYDePQJpE14".Hash() + __LINE__.ToInt()));
}
override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
{
if(GetGame().IsServer())
return true;
CarScript fdqdbSVc9SOEfC9=ALECarLockTargetHelper.GetTargetCar(target);
if(!fdqdbSVc9SOEfC9 || fdqdbSVc9SOEfC9.m_CarLockOwner==-1)
return false;
if(!fdqdbSVc9SOEfC9.m_CarLock_IsLocked && ALECarLockHelpers.CanPlayerUseLock(player, fdqdbSVc9SOEfC9))
{
return true;
}
return false;
}
override void OnStartServer(ActionData action_data)
{
CarScript GOVaZkaGiMpJx7E=ALECarLockTargetHelper.GetTargetCar(action_data.m_Target);
if(GOVaZkaGiMpJx7E)
{
GOVaZkaGiMpJx7E.SetCarLock(true);
GOVaZkaGiMpJx7E.SetSoundToPlay(1);
			string eVL5XVPKsYtRwgP="车辆已锁定";
ALECarLockCore.LogVehicleAction(eVL5XVPKsYtRwgP, GOVaZkaGiMpJx7E, action_data.m_Player);
}
}
};
#endif 
