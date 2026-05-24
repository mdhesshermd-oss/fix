#ifndef ALECARLOCKDISABLE
class ActionDetachCarLockCB:ActionContinuousBaseCB
{
override void CreateActionComponent()
{
m_ActionData.m_ActionComponent=new CAContinuousTime(15);
}
}
class ActionDetachCarLock:ActionContinuousBase
{
void ActionDetachCarLock()
{
m_CallbackClass=ActionDetachCarLockCB;
m_CommandUID=DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
m_FullBody=true;
m_StanceMask=DayZPlayerConstants.STANCEMASK_CROUCH;
m_SpecialtyWeight=UASoftSkillsWeight.PRECISE_LOW;
}
override void CreateConditionComponents()
{
m_ConditionTarget=new CCTNonRuined(10);
m_ConditionItem=new CCINonRuined;
}
override string GetText()
{
PlayerBase FUu9FfWr5Ytw6zW=PlayerBase.Cast(GetGame().GetPlayer());
if(FUu9FfWr5Ytw6zW && ALECarLockHelpers.IsAdmin(FUu9FfWr5Ytw6zW))
{
return LFtKbcCULbZZgf("69A7597BB170655D74905B67708991A25D6FAABCA16B9AB77FA59D73", ("1k6sJnNNnI4a22".Hash() + __LINE__.ToInt()));
}
return LFtKbcCULbZZgf("695F517D527691A4BB6DCC6BACBC708EB67E7CC046A77C666C9965", ("EVFfU4dA1GEXni".Hash() + __LINE__.ToInt()));
}
override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
{
if(GetGame().IsServer())
return true;
CarScript fdqdbSVc9SOEfC9=ALECarLockTargetHelper.GetTargetCar(target);
if(!fdqdbSVc9SOEfC9)
return false;
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
return false;
if(ALECarLockHelpers.CanPlayerUseLock(player, fdqdbSVc9SOEfC9))
{
return true;
}
return false;
}
override void OnFinishProgressServer(ActionData action_data)
{
if(action_data.m_MainItem && action_data.m_MainItem.GetHierarchyRootPlayer()==action_data.m_Player)
{
CarScript GOVaZkaGiMpJx7E=ALECarLockTargetHelper.GetTargetCar(action_data.m_Target);
if(GOVaZkaGiMpJx7E)
{
GOVaZkaGiMpJx7E.ResetCarLock();
ItemBase QZbXFLJNENm8b9F=ItemBase.Cast(GetGame().CreateObjectEx(LFtKbcCULbZZgf("3D542F4EBFA067A0C171", ("opJmIDGCtL9I1K".Hash() + __LINE__.ToInt())), action_data.m_Player.GetPosition(), ECE_PLACE_ON_SURFACE));
                GetALECarLockNewLogger().LogInfo("车锁已拆卸: 玩家 " + action_data.m_Player.GetIdentity().GetName()+ " (SteamID: " + action_data.m_Player.GetIdentity().GetPlainId()+ ")");
}
}
}
}
#endif
