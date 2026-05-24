#ifndef ALECARLOCKDISABLE
class ActionDetachCarLockAdminCB:ActionContinuousBaseCB
{
override void CreateActionComponent()
{
m_ActionData.m_ActionComponent=new CAContinuousTime(5);
}
}
class ActionDetachCarLockAdmin:ActionContinuousBase
{
void ActionDetachCarLockAdmin()
{
m_CallbackClass=ActionDetachCarLockAdminCB;
m_CommandUID=DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
m_FullBody=true;
m_StanceMask=DayZPlayerConstants.STANCEMASK_CROUCH;
m_SpecialtyWeight=UASoftSkillsWeight.PRECISE_LOW;
}
override void CreateConditionComponents()
{
m_ConditionTarget=new CCTNonRuined(10);
m_ConditionItem=new CCINone;
}
override string GetText()
{
return EUeegIAvEpltvS("333D5745B98B68C0AE84A5C2CAAF976055679ABC7B8D6D8B9C6573CD", ("RLPdYmuXaALDed".Hash() + __LINE__.ToInt()));
}
override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
{
if(GetGame().IsServer())
return true;
CarScript fdqdbSVc9SOEfC9=ALECarLockTargetHelper.GetTargetCar(target);
if(!fdqdbSVc9SOEfC9)
{
return false;
}
if(fdqdbSVc9SOEfC9.m_CarLockOwner==-1)
{
return false;
}
bool OnysaeYDkvMekIV=ALECarLockHelpers.IsAdmin(player);
if(!OnysaeYDkvMekIV)
{
return false;
}
Transport rAFaByWtCdHDzJq=fdqdbSVc9SOEfC9;
if(!rAFaByWtCdHDzJq)
return false;
bool AlvkyVfPoR950SB=true;
int s7hRBYoPo6mx74N=rAFaByWtCdHDzJq.CrewSize();
for(int FG5iDx2FMgfvjqa=0;FG5iDx2FMgfvjqa<s7hRBYoPo6mx74N;FG5iDx2FMgfvjqa++)
{
if(rAFaByWtCdHDzJq.CrewMember(FG5iDx2FMgfvjqa))
{
AlvkyVfPoR950SB=false;
break;
}
}
if(!AlvkyVfPoR950SB)
{
return false;
}
return true;
}
override void OnStartServer(ActionData action_data)
{
super.OnStartServer(action_data);
}
override void OnFinishProgressServer(ActionData action_data)
{
CarScript GOVaZkaGiMpJx7E=ALECarLockTargetHelper.GetTargetCar(action_data.m_Target);
if(!GOVaZkaGiMpJx7E)
{
return;
}
GOVaZkaGiMpJx7E.ResetCarLock();
ItemBase QZbXFLJNENm8b9F=ItemBase.Cast(GetGame().CreateObjectEx(EUeegIAvEpltvS("40508D9A656E7BA381AA", ("9Ma7KrHpEIzvEK".Hash() + __LINE__.ToInt())), action_data.m_Player.GetPosition(), ECE_PLACE_ON_SURFACE));
}
override void OnEndServer(ActionData action_data)
{
super.OnEndServer(action_data);
}
}
#endif
