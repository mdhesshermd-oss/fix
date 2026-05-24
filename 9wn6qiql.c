#ifndef ALECARLOCKDISABLE
modded class CarScript
{
bool m_CarLock_IsLocked=false;
int  m_CarLockOwner    =-1;
int  m_CarLockPassword =-1;
string m_CarLockOwnerName="";
string m_CarLockFullSteamID="";
int  m_SoundToPlay     =0;
vector m_LastOwnerPosition;
bool m_AutoLockScheduled=false;
protected EffectSound 		m_AlarmSound;
protected EffectSound 		m_BipSound;
void CarScript()
{
RegisterNetSyncVariableInt(geXrSIkHTogKDY("B6B35458C69DAD7067A55946CA", ("fRR128fsliz4Gy".Hash() + __LINE__.ToInt())));
RegisterNetSyncVariableBool(geXrSIkHTogKDY("B17F4A416138CE4F6A4465834A7FB2A27B48", ("yKWBc3c8ndMDAv".Hash() + __LINE__.ToInt())));
RegisterNetSyncVariableInt(geXrSIkHTogKDY("6270265E655EA06EB669724F56CC", ("kPs2MY8ZH8MoOs".Hash() + __LINE__.ToInt())));
RegisterNetSyncVariableInt(geXrSIkHTogKDY("9CA47284899E55ACBB4B7AB1A375C1B359", ("9Ee3THJamcXLmv".Hash() + __LINE__.ToInt())));
if(m_AlarmSound)m_AlarmSound.SoundStop();
if(GetGame().IsServer())
{
GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.CarLockLogInit, 10, false);
GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.CheckAutoLock, 5000, true);
}
}
void CarLockLogInit()
{
if(m_CarLockOwner !=-1 &&  m_CarLockPassword!=-1)
{
GetALECarLockLogger().Log(GetDisplayName()+ geXrSIkHTogKDY("302B5FC868528790956E", ("ocscFeKsUR8LEE".Hash() + __LINE__.ToInt())) + m_CarLockOwner + geXrSIkHTogKDY("7C27688AAE645D7BA88D563C", ("ydbcC2OGclqJlL".Hash() + __LINE__.ToInt())) + m_CarLockOwnerName + geXrSIkHTogKDY("0E25BB92AF5DB2767549", ("4IZUD9mNTbxO8z".Hash() + __LINE__.ToInt())) + m_CarLockFullSteamID + geXrSIkHTogKDY("0C57447097C437", ("9KKrN6vPoHEX0H".Hash() + __LINE__.ToInt())) + GetPosition()+geXrSIkHTogKDY("605E2CB97B80C86E6356693E", ("ION14nQ6gPEWFd".Hash() + __LINE__.ToInt()))+m_CarLock_IsLocked+geXrSIkHTogKDY("236D5167709EA1ABCC99B87A", ("kSO0clqMdMzaS7".Hash() + __LINE__.ToInt()))+m_CarLockPassword+ geXrSIkHTogKDY("17045C556BC3AD5C675865AA5C", ("s4k6bNoTgTZ3kj".Hash() + __LINE__.ToInt())));
}
}
void CarLockLogClaimed()
{
GetALECarLockLogger().Log(GetDisplayName()+ geXrSIkHTogKDY("204197C584BE736D7133", ("A9yxnYAQy3lkr0".Hash() + __LINE__.ToInt())) + m_CarLockOwner + geXrSIkHTogKDY("1B72BDBC9BAACE83A7759C40", ("SGaBXPoetsVQuc".Hash() + __LINE__.ToInt())) + m_CarLockOwnerName + geXrSIkHTogKDY("5F3576A54979BE7F398D", ("KuezBGFFfUTXGU".Hash() + __LINE__.ToInt())) + m_CarLockFullSteamID + geXrSIkHTogKDY("005E13AD979703", ("dZU1dJPTPcrafu".Hash() + __LINE__.ToInt())) + GetPosition()+geXrSIkHTogKDY("576E6063D05DB0AE5BB6AD29", ("116QQzX2Mu1q3m".Hash() + __LINE__.ToInt()))+m_CarLock_IsLocked+geXrSIkHTogKDY("2B482D589B625A5BC6B19252", ("ZESACelrC9QogL".Hash() + __LINE__.ToInt()))+m_CarLockPassword+ geXrSIkHTogKDY("761985517C785360CC44ADB8448D59BD7A", ("5mh3MdTqXiC8rJ".Hash() + __LINE__.ToInt())));
}
override void EEDelete(EntityAI parent)
{
super.EEDelete(parent);
if(GetGame().IsServer())
{
if(m_CarLockOwner !=-1 &&  m_CarLockPassword!=-1)
{
GetALECarLockLogger().Log(GetDisplayName()+ geXrSIkHTogKDY("2F73BFD498A48D7ABE51", ("Z9N4uifE44U46C".Hash() + __LINE__.ToInt())) + m_CarLockOwner + geXrSIkHTogKDY("6B1291B8CC7DC95C72BC551E", ("XWZTjPWlwz0LJD".Hash() + __LINE__.ToInt())) + m_CarLockOwnerName + geXrSIkHTogKDY("7B19A06B70B0B76C5822", ("Xxrs0hTdZxpYoH".Hash() + __LINE__.ToInt())) + m_CarLockFullSteamID + geXrSIkHTogKDY("7C7510AA559327", ("2zMoSPL0AAZ6wg".Hash() + __LINE__.ToInt())) + GetPosition()+geXrSIkHTogKDY("3D536FAAB251BC49B2AB5E42", ("fxq76EAh8Lyxgd".Hash() + __LINE__.ToInt()))+m_CarLock_IsLocked+geXrSIkHTogKDY("71572CBEB4D0618CC0865E2C", ("i2oem5qJVr7oBu".Hash() + __LINE__.ToInt()))+m_CarLockPassword+ geXrSIkHTogKDY("3222A58099707F4AB9BD3964975D4E6F4C67", ("sJECLvD4YEFtG4".Hash() + __LINE__.ToInt())));
}
}
}
void ~CarScript()
{
if(m_AlarmSound)m_AlarmSound.Stop();
}
override void OnVariablesSynchronized()
{
super.OnVariablesSynchronized();
switch(m_SoundToPlay)
{
case 0:return;
break;
case 1:
{
PlayCarLockSound();
m_SoundToPlay=0;
}
break;
case 2:
{
PlayCarUnlockSound();
m_SoundToPlay=0;
}
break;
case 3:
{
PlayCarLockAlarm();
m_SoundToPlay=0;
}
break;
default:return;
break;
}
}
void SetSoundToPlay(int id=0)
{
m_SoundToPlay=id;
SetSynchDirty();
thread ResetSoundToPlay();
}
void ResetSoundToPlay()
{
Sleep(1000);
m_SoundToPlay=0;
SetSynchDirty();
}
void SetCarLockPassword(int pw)
{
m_CarLockPassword=pw;
CarLockLogClaimed();
SetSynchDirty();
}
void SetCarLockOwner(int low_uid)
{
m_CarLockOwner=low_uid;
SetSynchDirty();
}
void SetCarLockOwnerName(string name)
{
m_CarLockOwnerName=name;
SetSynchDirty();
}
void SetCarLockFullSteamID(string steamID)
{
m_CarLockFullSteamID=steamID;
SetSynchDirty();
}
void SetCarLock(bool rep)
{
m_CarLock_IsLocked=rep;
SetSynchDirty();
SynchronizeValues();
if(rep && GetGame().IsServer())
{
UpdatePlayerVehicleData();
}
}
void StopAlarm()
{
if(m_AlarmSound)m_AlarmSound.SoundStop();
}
void PlayCarLockAlarm()
{
if(m_AlarmSound && m_AlarmSound.IsSoundPlaying())m_AlarmSound.SoundStop();
PlaySoundSet(m_AlarmSound,geXrSIkHTogKDY("7B9956AF627D9D6562736EA07D83536DA868C5CB7A819371694A8C79C3", ("emjpRzr0Q4G59e".Hash() + __LINE__.ToInt())) , 0, 0);
}
void PlayCarLockSound()
{
if(m_AlarmSound && m_AlarmSound.IsSoundPlaying())m_AlarmSound.SoundStop();
PlaySoundSet(m_BipSound,geXrSIkHTogKDY("A46EB47B4F7DCD819C8DA0BA6F6B7AA895459D6C88705C8D5875AA57", ("XBA0qo342tBkHZ".Hash() + __LINE__.ToInt())) , 0, 0);
}
void PlayCarUnlockSound()
{
if(m_AlarmSound && m_AlarmSound.IsSoundPlaying())m_AlarmSound.SoundStop();
PlaySoundSet(m_BipSound,geXrSIkHTogKDY("7B5796619B53B4828E68A14436A15294B0C29192AE658AA368616E944792", ("6MyGRCiX0FlnQo".Hash() + __LINE__.ToInt())) , 0, 0);
}
override bool CanDisplayAttachmentSlot(string slot_name)
{
if(!super.CanDisplayAttachmentSlot(slot_name))
return false;
return !m_CarLock_IsLocked;
}
override bool CanDisplayAttachmentCategory(string category_name)
{
if(!super.CanDisplayAttachmentCategory(category_name))
return false;
return !m_CarLock_IsLocked;
}
void SynchronizeValues()
{
if(GetGame().IsServer())
{
if(GetInventory())
{
if(m_CarLock_IsLocked)
GetInventory().LockInventory(HIDE_INV_FROM_SCRIPT);
else
GetInventory().UnlockInventory(HIDE_INV_FROM_SCRIPT);
}
SetSynchDirty();
}
}
override void OnStoreSave(ParamsWriteContext ctx)
{
super.OnStoreSave(ctx);
Param5<bool, int, int, string, string>jQhxs80yRxFpE3f=new Param5<bool, int, int, string, string>(m_CarLock_IsLocked, m_CarLockOwner, m_CarLockPassword, m_CarLockOwnerName, m_CarLockFullSteamID);
ctx.Write(jQhxs80yRxFpE3f);
}
override bool OnStoreLoad(ParamsReadContext ctx, int version)
{
if(!super.OnStoreLoad(ctx, version))
return false;
Param5<bool, int, int, string, string>ttJrrQNO7XpK8O2=new Param5<bool, int, int, string, string>(false, -1, -1, "", "");
if(ctx.Read(ttJrrQNO7XpK8O2))
{
m_CarLock_IsLocked=ttJrrQNO7XpK8O2.param1;
m_CarLockOwner=ttJrrQNO7XpK8O2.param2;
m_CarLockPassword=ttJrrQNO7XpK8O2.param3;
m_CarLockOwnerName=ttJrrQNO7XpK8O2.param4;
m_CarLockFullSteamID=ttJrrQNO7XpK8O2.param5;
}
else
{
Param3<bool, int, int>wumu52D5wwaG4WR=new Param3<bool, int, int>(false, -1, -1);
if(ctx.Read(wumu52D5wwaG4WR))
{
m_CarLock_IsLocked=wumu52D5wwaG4WR.param1;
m_CarLockOwner=wumu52D5wwaG4WR.param2;
m_CarLockPassword=wumu52D5wwaG4WR.param3;
}
}
SynchronizeValues();
return true;
}
override bool IsInventoryVisible()
{
return !m_CarLock_IsLocked &&(GetGame().GetPlayer()&&(!GetGame().GetPlayer().GetCommand_Vehicle()|| GetGame().GetPlayer().GetCommand_Vehicle().GetTransport()==this));
}
void ResetCarLock()
{
m_CarLockPassword=-1;
m_CarLockOwner=-1;
m_CarLock_IsLocked=false;
m_CarLockOwnerName="";
m_CarLockFullSteamID="";
SetSynchDirty();
SynchronizeValues();
SetSoundToPlay(2);
}
string GetCarLockOwnerName()
{
return m_CarLockOwnerName;
}
string GetCarLockFullSteamID()
{
return m_CarLockFullSteamID;
}
void CheckAutoLock()
{
if(!GetGame().IsServer())
return;
ALECarLockStandaloneConfig KXlCDUX0iPjGya3=GetALECarLockStandaloneConfig();
if(!KXlCDUX0iPjGya3.EnableAutoLock)
return;
if(m_CarLock_IsLocked || m_CarLockOwner==-1 || m_CarLockPassword==-1)
return;
if(m_CarLockFullSteamID=="")
return;
PlayerBase z6pW9aJ6Ft8KsyD=ALECarLockHelpers.GetPlayerBySteamID(m_CarLockFullSteamID);
PlayerVehicleData Rmrp8EAoIB0KVBG;
string AqK53cMbc90q2th;
vector xXdSCQx7zxeF7mR;
if(!z6pW9aJ6Ft8KsyD)
{
SetCarLock(true);
SetSoundToPlay(1);
            GetALECarLockNewLogger().LogInfo("自动上锁: 车主已离线 - " + GetDisplayName());
if(KXlCDUX0iPjGya3.EnablePlayerDataTracking && m_CarLockFullSteamID !="")
{
Rmrp8EAoIB0KVBG=GetPlayerVehicleData(m_CarLockFullSteamID);
if(Rmrp8EAoIB0KVBG)
{
AqK53cMbc90q2th=GetDisplayName()+ geXrSIkHTogKDY("90", ("VqfC2MnVajjAUj".Hash() + __LINE__.ToInt())) + GetPosition().ToString();
Rmrp8EAoIB0KVBG.AddOrUpdateVehicle(AqK53cMbc90q2th, m_CarLockPassword, GetPosition(), GetType());
}
}
return;
}
xXdSCQx7zxeF7mR=GetPosition();
vector NzssNrm1ITcDBxK=z6pW9aJ6Ft8KsyD.GetPosition();
float MGmmQOgNQe9Ha01=vector.Distance(xXdSCQx7zxeF7mR, NzssNrm1ITcDBxK);
if(MGmmQOgNQe9Ha01>KXlCDUX0iPjGya3.AutoLockDistance)
{
SetCarLock(true);
SetSoundToPlay(1);
            GetALECarLockNewLogger().LogInfo("自动上锁: 车主距离 " + MGmmQOgNQe9Ha01.ToString()+ " 米 - " + GetDisplayName());
if(KXlCDUX0iPjGya3.EnablePlayerDataTracking && m_CarLockFullSteamID !="")
{
Rmrp8EAoIB0KVBG=GetPlayerVehicleData(m_CarLockFullSteamID);
if(Rmrp8EAoIB0KVBG)
{
AqK53cMbc90q2th=GetDisplayName()+ geXrSIkHTogKDY("54", ("gkp2fWRTBpA89P".Hash() + __LINE__.ToInt())) + GetPosition().ToString();
Rmrp8EAoIB0KVBG.AddOrUpdateVehicle(AqK53cMbc90q2th, m_CarLockPassword, xXdSCQx7zxeF7mR, GetType());
}
}
}
}
void UpdatePlayerVehicleData()
{
if(!GetGame().IsServer())
return;
ALECarLockStandaloneConfig KXlCDUX0iPjGya3=GetALECarLockStandaloneConfig();
if(!KXlCDUX0iPjGya3.EnablePlayerDataTracking)
return;
if(m_CarLockFullSteamID=="" || m_CarLockPassword==-1)
return;
PlayerVehicleData Rmrp8EAoIB0KVBG=GetPlayerVehicleData(m_CarLockFullSteamID);
if(Rmrp8EAoIB0KVBG)
{
string AqK53cMbc90q2th=GetDisplayName()+ geXrSIkHTogKDY("42", ("UEBFo99mczfg4F".Hash() + __LINE__.ToInt())) + GetPosition().ToString();
Rmrp8EAoIB0KVBG.AddOrUpdateVehicle(AqK53cMbc90q2th, m_CarLockPassword, GetPosition(), GetType());
}
}
};
#endif
