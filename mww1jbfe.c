#ifndef ALECARLOCKDISABLE
class ActionALECarUnLock:ActionContinuousBase
{
void ActionALECarUnLock()
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
return jqcapvkhIzbURn("119D945D41A975BC9CC0B88475A87982BEC895A7544B729CA457B8B0", ("qOOM5lIgEOanTC".Hash() + __LINE__.ToInt()));
}
return jqcapvkhIzbURn("3B5161465870854D66578B887B6C829877B877A5574EA7D1C99EA5", ("sJCzW0HtT1pdM5".Hash() + __LINE__.ToInt()));
}
override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
{
if(GetGame().IsServer())
return true;
CarScript fdqdbSVc9SOEfC9=ALECarLockTargetHelper.GetTargetCar(target);
if(fdqdbSVc9SOEfC9 && fdqdbSVc9SOEfC9.m_CarLock_IsLocked && ALECarLockHelpers.CanPlayerUseLock(player, fdqdbSVc9SOEfC9))
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
GOVaZkaGiMpJx7E.SetCarLock(false);
GOVaZkaGiMpJx7E.SetSoundToPlay(2);
			string eVL5XVPKsYtRwgP="车辆已解锁";
ALECarLockCore.LogVehicleAction(eVL5XVPKsYtRwgP, GOVaZkaGiMpJx7E, action_data.m_Player);
}
}
};
#endif 
