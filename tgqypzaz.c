#ifndef ALECARLOCKDISABLE
class ActionDetachCarLockPickCB:ActionContinuousBaseCB
{
override void CreateActionComponent()
{
m_ActionData.m_ActionComponent=new CAContinuousTime(90);
}
}
class ActionDetachCarLockPick:ActionContinuousBase
{
CarScript m_TargetCar=null;
void ActionDetachCarLockPick()
{
m_CallbackClass=ActionDetachCarLockPickCB;
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
return RrqhCtsHjDgEdk("274AAA5F445D5D5C937DC3CB844C95A97A76725B604CB2C6", ("SkN08RWF1y3hfj".Hash() + __LINE__.ToInt()));
}
override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
{
if(GetGame().IsServer())
return true;
if(!item)
return false;
ALECarLockStandaloneConfig KXlCDUX0iPjGya3=GetALECarLockStandaloneConfig();
if(!KXlCDUX0iPjGya3.LockPickTools || KXlCDUX0iPjGya3.LockPickTools.Count()==0)
return false;
bool Vos9EMlGHpTKifS=false;
string ebYX3R6p6vlwtIQ=item.GetType();
for(int FG5iDx2FMgfvjqa=0;FG5iDx2FMgfvjqa<KXlCDUX0iPjGya3.LockPickTools.Count();FG5iDx2FMgfvjqa++)
{
if(ebYX3R6p6vlwtIQ==KXlCDUX0iPjGya3.LockPickTools[FG5iDx2FMgfvjqa])
{
Vos9EMlGHpTKifS=true;
break;
}
}
if(!Vos9EMlGHpTKifS)
return false;
CarScript fdqdbSVc9SOEfC9=ALECarLockTargetHelper.GetTargetCar(target);
if(!fdqdbSVc9SOEfC9)
return false;
if(fdqdbSVc9SOEfC9.m_CarLockOwner==-1 || fdqdbSVc9SOEfC9.m_CarLockPassword==-1)
return false;
if(fdqdbSVc9SOEfC9.m_CarLockOwner==player.CLSteamlowID)
return false;
Transport rAFaByWtCdHDzJq=fdqdbSVc9SOEfC9;
if(!rAFaByWtCdHDzJq)
return false;
bool AlvkyVfPoR950SB=true;
int s7hRBYoPo6mx74N=rAFaByWtCdHDzJq.CrewSize();
for(int KyXkNpdiKwCaKip=0;KyXkNpdiKwCaKip<s7hRBYoPo6mx74N;KyXkNpdiKwCaKip++)
{
if(rAFaByWtCdHDzJq.CrewMember(KyXkNpdiKwCaKip))
{
AlvkyVfPoR950SB=false;
break;
}
}
if(!AlvkyVfPoR950SB)
return false;
return true;
}
override void OnStartServer(ActionData action_data)
{
CarScript GOVaZkaGiMpJx7E=ALECarLockTargetHelper.GetTargetCar(action_data.m_Target);
if(GOVaZkaGiMpJx7E)
{
m_TargetCar=GOVaZkaGiMpJx7E;
GOVaZkaGiMpJx7E.SetSoundToPlay(3);
}
}
override void OnFinishProgressServer(ActionData action_data)
{
if(!action_data.m_MainItem)
return;
PlayerBase FUu9FfWr5Ytw6zW=action_data.m_Player;
if(!FUu9FfWr5Ytw6zW)
return;
ALECarLockStandaloneConfig KXlCDUX0iPjGya3=GetALECarLockStandaloneConfig();
float sqAYDdGkL3090pa=Math.RandomFloat01()* 100;
bool WBAj06jAb5GFdS8=sqAYDdGkL3090pa<KXlCDUX0iPjGya3.LockPickChance;
if(WBAj06jAb5GFdS8)
{
if(m_TargetCar)
{
m_TargetCar.ResetCarLock();
                GetALECarLockNewLogger().LogInfo("撬锁成功: 玩家 " + FUu9FfWr5Ytw6zW.GetIdentity().GetName()+ " (SteamID: " + FUu9FfWr5Ytw6zW.GetIdentity().GetPlainId()+ ") 在位置 " + m_TargetCar.GetPosition().ToString());
}
}
else
{
            GetALECarLockNewLogger().LogInfo("撬锁失败: 玩家 " + FUu9FfWr5Ytw6zW.GetIdentity().GetName()+ " (SteamID: " + FUu9FfWr5Ytw6zW.GetIdentity().GetPlainId()+ ")");
}
action_data.m_MainItem.DecreaseHealth("", "", 25);
}
override void OnEndServer(ActionData action_data)
{
if(m_TargetCar)
{
m_TargetCar.StopAlarm();
}
}
}
#endif
