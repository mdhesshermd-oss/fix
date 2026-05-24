#ifndef ALECARLOCKDISABLE
modded class PlayerBase extends ManBase
{
int CLSteamlowID=-1;
bool m_IsALECarLockAdmin=false;
ref ALECarLockMenu m_CarLockMenu;
override void Init()
{
super.Init();
RegisterNetSyncVariableInt(fAXewIhAwFQUDu("657B8FC04582BE77C7955F73", ("ElOn24qLWgchWQ".Hash() + __LINE__.ToInt())));
RegisterNetSyncVariableBool(fAXewIhAwFQUDu("8B7759B26B6541796392AA8ABC887DA48E7175", ("D15p9qFGwGbFwO".Hash() + __LINE__.ToInt())));
}
void SetLowSteamUID(string playerUID)
{
string Sr23j64V72HmH5Z="";
for(int KyXkNpdiKwCaKip=8;KyXkNpdiKwCaKip<17;KyXkNpdiKwCaKip++)
{
Sr23j64V72HmH5Z+=playerUID.Get(KyXkNpdiKwCaKip);
}
CLSteamlowID=Sr23j64V72HmH5Z.ToInt();
SetSynchDirty();
}
void SetALECarLockAdminStatus(bool isAdmin)
{
m_IsALECarLockAdmin=isAdmin;
SetSynchDirty();
}
bool IsALECarLockAdmin()
{
return m_IsALECarLockAdmin;
}
int GetAdminStatus()
{
if(m_IsALECarLockAdmin)
return 1;
return 0;
}
bool HasPassword(int pwd, int ownerID)
{
if(GetGame().IsClient())
{
ALECarLockData jpcQ2JgpoJhB0Yr=ALECarLockData.Load(CLSteamlowID.ToString());
if(jpcQ2JgpoJhB0Yr.CarLockPasswords.Count()>0)
{
for(int FG5iDx2FMgfvjqa=0;FG5iDx2FMgfvjqa<jpcQ2JgpoJhB0Yr.CarLockPasswords.Count();FG5iDx2FMgfvjqa++)
{
if(jpcQ2JgpoJhB0Yr.CarLockPasswords[FG5iDx2FMgfvjqa]==pwd && jpcQ2JgpoJhB0Yr.CarLockOwners[FG5iDx2FMgfvjqa]==ownerID)
{
return true;
}
}
}
return false;
}
return false;
}
override void SetActions()
{
super.SetActions();
AddAction(ActionALECarLockMenu);
AddAction(ActionALECarLock);
AddAction(ActionALECarUnLock);
AddAction(ActionDetachCarLockAdmin);
}
override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
{
super.OnRPC(sender, rpc_type, ctx);
bool rLKjJ9ygBnXbc9H;
bool WBAj06jAb5GFdS8;
int WHueh77NYW12CDz;
int puSOn6ot7RruOiE;
int TYLOhBYNoTADlb6;
ALECarLockData jpcQ2JgpoJhB0Yr;
int FG5iDx2FMgfvjqa;
bool ThJn0Ip001dFBdA;
bool JvMqi9U66QiCrlH;
if(rpc_type==78910)
{
if(ctx.Read(rLKjJ9ygBnXbc9H)&& ctx.Read(WHueh77NYW12CDz)&& ctx.Read(TYLOhBYNoTADlb6))
{
if(rLKjJ9ygBnXbc9H)
{
jpcQ2JgpoJhB0Yr=ALECarLockData.Load(CLSteamlowID.ToString());
ThJn0Ip001dFBdA=false;
for(FG5iDx2FMgfvjqa=0;FG5iDx2FMgfvjqa<jpcQ2JgpoJhB0Yr.CarLockPasswords.Count();FG5iDx2FMgfvjqa++)
{
if(jpcQ2JgpoJhB0Yr.CarLockPasswords[FG5iDx2FMgfvjqa]==WHueh77NYW12CDz && jpcQ2JgpoJhB0Yr.CarLockOwners[FG5iDx2FMgfvjqa]==TYLOhBYNoTADlb6)
{
ThJn0Ip001dFBdA=true;
break;
}
}
if(!ThJn0Ip001dFBdA)
{
jpcQ2JgpoJhB0Yr.CarLockPasswords.Insert(WHueh77NYW12CDz);
jpcQ2JgpoJhB0Yr.CarLockOwners.Insert(TYLOhBYNoTADlb6);
jpcQ2JgpoJhB0Yr.Save(CLSteamlowID.ToString());
}
					GetGame().GetMission().OnEvent(ChatMessageEventTypeID, new ChatMessageEventParams(CCDirect, "", "密码正确！已保存。", ""));
if(m_CarLockMenu)
{
m_CarLockMenu.Close();
m_CarLockMenu=null;
}
}
else
{
					GetGame().GetMission().OnEvent(ChatMessageEventTypeID, new ChatMessageEventParams(CCDirect, "", "密码错误！", ""));
if(m_CarLockMenu)
{
						m_CarLockMenu.SetOutputText("密码错误！");
}
}
}
}
if(rpc_type==78911)
{
if(ctx.Read(WBAj06jAb5GFdS8)&& ctx.Read(puSOn6ot7RruOiE)&& ctx.Read(TYLOhBYNoTADlb6))
{
if(WBAj06jAb5GFdS8)
{
jpcQ2JgpoJhB0Yr=ALECarLockData.Load(CLSteamlowID.ToString());
JvMqi9U66QiCrlH=false;
for(FG5iDx2FMgfvjqa=0;FG5iDx2FMgfvjqa<jpcQ2JgpoJhB0Yr.CarLockPasswords.Count();FG5iDx2FMgfvjqa++)
{
if(jpcQ2JgpoJhB0Yr.CarLockOwners[FG5iDx2FMgfvjqa]==TYLOhBYNoTADlb6)
{
jpcQ2JgpoJhB0Yr.CarLockPasswords[FG5iDx2FMgfvjqa]=puSOn6ot7RruOiE;
JvMqi9U66QiCrlH=true;
break;
}
}
if(!JvMqi9U66QiCrlH)
{
jpcQ2JgpoJhB0Yr.CarLockPasswords.Insert(puSOn6ot7RruOiE);
jpcQ2JgpoJhB0Yr.CarLockOwners.Insert(TYLOhBYNoTADlb6);
}
jpcQ2JgpoJhB0Yr.Save(CLSteamlowID.ToString());
					GetGame().GetMission().OnEvent(ChatMessageEventTypeID, new ChatMessageEventParams(CCDirect, "", "密码修改成功！", ""));
if(m_CarLockMenu)
{
m_CarLockMenu.Close();
m_CarLockMenu=null;
}
}
}
}
}
};
#endif
