#ifndef ALECARLOCKDISABLE
class ActionALECarLockMenu:ActionInteractBase
{
private string m_StringID=JlzJVOImtORAeD("6D3E7DB55E6B91754454B0A0736B7D8A5F5CB29C55B18AB0B29454", ("J4DUFT5bUFjFmC".Hash() + __LINE__.ToInt()));
void ActionALECarLockMenu()
{
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
return JlzJVOImtORAeD("5F", ("H8Uw6MqkzrQd5A".Hash() + __LINE__.ToInt())) + m_StringID;
}
override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
{
if(GetGame().IsServer())
return true;
CarScript fdqdbSVc9SOEfC9=ALECarLockTargetHelper.GetTargetCar(target);
if(!fdqdbSVc9SOEfC9)
return false;
if(ALECarLockHelpers.IsAdmin(player))
{
m_StringID=JlzJVOImtORAeD("AB5F59AFB0C99B81B892AFB06A6654788584606144C090785FC96D", ("QJHO5Ns23DGJp1".Hash() + __LINE__.ToInt()));
return true;
}
if(fdqdbSVc9SOEfC9.m_CarLockOwner==-1 && !fdqdbSVc9SOEfC9.m_CarLock_IsLocked)
{
EntityAI OBE9FFSdW56RmbW=player.GetHumanInventory().GetEntityInHands();
if(OBE9FFSdW56RmbW && OBE9FFSdW56RmbW.IsKindOf(JlzJVOImtORAeD("8A8D334FB15E765BAE4D", ("J0CANHfAYnFJTF".Hash() + __LINE__.ToInt()))))
{
m_StringID=JlzJVOImtORAeD("896853A493677B474FC29C9D5055634E6453B6BA66B184A49466A5", ("aX5NUBbHjV2Ju7".Hash() + __LINE__.ToInt()));
return true;
}
return false;
}
if(fdqdbSVc9SOEfC9.m_CarLock_IsLocked && fdqdbSVc9SOEfC9.m_CarLockOwner==player.CLSteamlowID && fdqdbSVc9SOEfC9.m_CarLockPassword==-1)
{
m_StringID=JlzJVOImtORAeD("5540569E5560489F6F79AA807A7C92BD91655B7597C7687A6BBA48", ("mePSjTtKLF088q".Hash() + __LINE__.ToInt()));
return true;
}
if(fdqdbSVc9SOEfC9.m_CarLock_IsLocked && fdqdbSVc9SOEfC9.m_CarLockOwner==player.CLSteamlowID && fdqdbSVc9SOEfC9.m_CarLockPassword !=-1)
{
m_StringID=JlzJVOImtORAeD("57A94B66455EB7C0AC6F65C7AA7EAF52B4639BAFC46B8B54B3898694729F", ("UWbdC07vhjTmtC".Hash() + __LINE__.ToInt()));
return true;
}
if(fdqdbSVc9SOEfC9.m_CarLock_IsLocked && fdqdbSVc9SOEfC9.m_CarLockOwner !=player.CLSteamlowID && !player.HasPassword(fdqdbSVc9SOEfC9.m_CarLockPassword, fdqdbSVc9SOEfC9.m_CarLockOwner)&& fdqdbSVc9SOEfC9.m_CarLockPassword !=-1)
{
m_StringID=JlzJVOImtORAeD("8CA3A87B825A6578BD857971869E4E88729AAB6B8D5D83BD88BE71C46A", ("hE5JrGg34l887P".Hash() + __LINE__.ToInt()));
return true;
}
if(fdqdbSVc9SOEfC9.m_CarLock_IsLocked && player.HasPassword(fdqdbSVc9SOEfC9.m_CarLockPassword, fdqdbSVc9SOEfC9.m_CarLockOwner)&& fdqdbSVc9SOEfC9.m_CarLockPassword !=-1)
{
m_StringID=JlzJVOImtORAeD("9D495F94B37E4BB2867387AF886D477DBE76C39854726C55C3C0", ("u04KT76yDHSUBN".Hash() + __LINE__.ToInt()));
return true;
}
return false;
}
override void OnStartClient(ActionData action_data)
{
CarScript GOVaZkaGiMpJx7E=ALECarLockTargetHelper.GetTargetCar(action_data.m_Target);
if(GOVaZkaGiMpJx7E)
{
OpenCarLockMenu(GOVaZkaGiMpJx7E);
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
