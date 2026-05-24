#ifndef ALECARLOCKDISABLE
class  ActionAttachCarLockCB:ActionContinuousBaseCB
{
override void CreateActionComponent()
{
m_ActionData.m_ActionComponent=new CAContinuousTime(2);
}
};
class  ActionAttachCarLock:ActionContinuousBase
{
void  ActionAttachCarLock()
{
m_CallbackClass=ActionAttachCarLockCB;
m_CommandUID=DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
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
return rMLRyIEkGZGcab("6F635AA2BC5F5BAB79A65567B881AF997E5DAD864789AF3FA8ADB14D", ("mdpSN0bpxGCZEt".Hash() + __LINE__.ToInt()));
}
override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
{
if(GetGame().IsServer())
return true;
if(!item.IsKindOf(rMLRyIEkGZGcab("8239328082CDAB68956D", ("xHyQSodPoTFt5v".Hash() + __LINE__.ToInt()))))return false;
CarScript fdqdbSVc9SOEfC9=ALECarLockTargetHelper.GetTargetCar(target);
if(!fdqdbSVc9SOEfC9)return false;
if(fdqdbSVc9SOEfC9 && fdqdbSVc9SOEfC9.m_CarLockOwner==-1)
{
Transport rAFaByWtCdHDzJq=fdqdbSVc9SOEfC9;
if(!rAFaByWtCdHDzJq)return false;
bool NZ030PdRdCGkCRl=true;
int s7hRBYoPo6mx74N=rAFaByWtCdHDzJq.CrewSize();
for(int KyXkNpdiKwCaKip=0;KyXkNpdiKwCaKip<s7hRBYoPo6mx74N;KyXkNpdiKwCaKip++)
{
if(rAFaByWtCdHDzJq.CrewMember(KyXkNpdiKwCaKip))
NZ030PdRdCGkCRl=false;
}
if(!NZ030PdRdCGkCRl)return false;
return true;
}
return false;
}
override void OnFinishProgressClient(ActionData action_data)
{
if(action_data.m_MainItem.IsKindOf(rMLRyIEkGZGcab("27497DA2B1D053B565C7", ("TUCAmNBmBX8bAy".Hash() + __LINE__.ToInt()))))
{
if(action_data.m_Target && action_data.m_Target.GetObject())
{
CarScript PTdC7fAbnG6trJj=ALECarLockTargetHelper.GetTargetCar(action_data.m_Target);
if(PTdC7fAbnG6trJj)
{
OpenCarLockMenu(PTdC7fAbnG6trJj);
}
}
}
}
override void OnFinishProgressServer(ActionData action_data)
{
if(action_data.m_MainItem.IsKindOf(rMLRyIEkGZGcab("81338C8787C7387559B7", ("dItbVv40BGvXmV".Hash() + __LINE__.ToInt()))))
{
action_data.m_MainItem.Delete();
if(action_data.m_Target && action_data.m_Target.GetObject())
{
CarScript PTdC7fAbnG6trJj=ALECarLockTargetHelper.GetTargetCar(action_data.m_Target);
PlayerBase FUu9FfWr5Ytw6zW=action_data.m_Player;
if(PTdC7fAbnG6trJj && FUu9FfWr5Ytw6zW && FUu9FfWr5Ytw6zW.GetIdentity())
{
PTdC7fAbnG6trJj.SetCarLockOwner(FUu9FfWr5Ytw6zW.CLSteamlowID);
PTdC7fAbnG6trJj.SetCarLockOwnerName(FUu9FfWr5Ytw6zW.GetIdentity().GetName());
PTdC7fAbnG6trJj.SetCarLockFullSteamID(FUu9FfWr5Ytw6zW.GetIdentity().GetPlainId());
PTdC7fAbnG6trJj.SetCarLock(true);
PTdC7fAbnG6trJj.SetSoundToPlay(1);
					ALECarLockCore.LogVehicleAction("车锁安装", PTdC7fAbnG6trJj, FUu9FfWr5Ytw6zW);
}
}
}
}
void InitCarLockMenu(PlayerBase player, CarScript vehicle)
{
player.m_CarLockMenu=new ALECarLockMenu(vehicle);
player.m_CarLockMenu.Init();
}
void OpenCarLockMenu(CarScript vehicle)
{
PlayerBase FUu9FfWr5Ytw6zW=PlayerBase.Cast(GetGame().GetPlayer());
if(FUu9FfWr5Ytw6zW)
{
InitCarLockMenu(FUu9FfWr5Ytw6zW,vehicle);
GetGame().GetUIManager().ShowScriptedMenu(FUu9FfWr5Ytw6zW.m_CarLockMenu, NULL);
}
}
};
#endif
