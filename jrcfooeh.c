#ifndef ALECARLOCKDISABLE
static ref ALECarLockCore m_CarLockCore;
static ALECarLockCore GetALECarLockCore(){
if(!m_CarLockCore){
m_CarLockCore=new ALECarLockCore;
}
return m_CarLockCore;
}
#endif
static PlayerBase CLGetPlayerByIdentity(PlayerIdentity identity)
{
if(!identity)
return null;
array<Man>oMZxVlekIIwBwbd=new array<Man>;
GetGame().GetPlayers(oMZxVlekIIwBwbd);
foreach(Man i4llghd8s2Mka21:oMZxVlekIIwBwbd)
{
PlayerBase FUu9FfWr5Ytw6zW=PlayerBase.Cast(i4llghd8s2Mka21);
if(FUu9FfWr5Ytw6zW && FUu9FfWr5Ytw6zW.GetIdentity()&& FUu9FfWr5Ytw6zW.GetIdentity().GetId()==identity.GetId())
return FUu9FfWr5Ytw6zW;
}
return null;
}
#ifndef ALECARLOCKDISABLE
class ALECarLockHelpers
{
static bool CreateInInventory(PlayerBase player, string classname, int quantity=1)
{
if(!player || classname=="" || quantity<=0)
return false;
EntityAI qeCsHzpttnUWqkY;
ItemBase KQtC2wrzYyqMHQZ;
qeCsHzpttnUWqkY=player.GetInventory().CreateInInventory(classname);
if(!qeCsHzpttnUWqkY)
{
vector sphXHIPjvLa9mgp=player.GetPosition();
qeCsHzpttnUWqkY=EntityAI.Cast(GetGame().CreateObject(classname, sphXHIPjvLa9mgp, false, true));
}
if(qeCsHzpttnUWqkY)
{
KQtC2wrzYyqMHQZ=ItemBase.Cast(qeCsHzpttnUWqkY);
if(KQtC2wrzYyqMHQZ)
{
KQtC2wrzYyqMHQZ.SetQuantity(quantity);
}
return true;
}
return false;
}
static bool CheckAdminBySteamID(string steamID)
{
if(steamID=="" || !GetGame().IsServer())
return false;
array<string>ebJ8gIKmTVLvGK4=GetALECarLockStandaloneConfig().Admins;
if(ebJ8gIKmTVLvGK4 && ebJ8gIKmTVLvGK4.Count()>0)
{
for(int FG5iDx2FMgfvjqa=0;FG5iDx2FMgfvjqa<ebJ8gIKmTVLvGK4.Count();FG5iDx2FMgfvjqa++)
{
if(ebJ8gIKmTVLvGK4[FG5iDx2FMgfvjqa]==steamID)
return true;
}
}
return false;
}
static bool IsAdmin(PlayerBase player)
{
if(!player)
return false;
if(GetGame().IsServer())
{
if(!player.GetIdentity())
return false;
return CheckAdminBySteamID(player.GetIdentity().GetPlainId());
}
return player.IsALECarLockAdmin();
}
static bool IsInSafeZone(vector position)
{
return false;
}
static PlayerBase GetPlayerBySteamID(string steamID)
{
if(steamID=="" || !GetGame().IsServer())
return null;
array<Man>oMZxVlekIIwBwbd=new array<Man>;
GetGame().GetPlayers(oMZxVlekIIwBwbd);
foreach(Man i4llghd8s2Mka21:oMZxVlekIIwBwbd)
{
PlayerBase FUu9FfWr5Ytw6zW=PlayerBase.Cast(i4llghd8s2Mka21);
if(FUu9FfWr5Ytw6zW && FUu9FfWr5Ytw6zW.GetIdentity())
{
if(FUu9FfWr5Ytw6zW.GetIdentity().GetPlainId()==steamID)
return FUu9FfWr5Ytw6zW;
}
}
return null;
}
static bool CanPlayerUseLock(PlayerBase player, CarScript vehicle)
{
if(!player || !vehicle)
return false;
if(vehicle.m_CarLockOwner==-1 || vehicle.m_CarLockPassword==-1)
return false;
if(IsAdmin(player))
return true;
if(vehicle.m_CarLockOwner==player.CLSteamlowID)
return true;
ALECarLockStandaloneConfig KXlCDUX0iPjGya3=GetALECarLockStandaloneConfig();
if(KXlCDUX0iPjGya3.EnablePasswordSharing)
{
if(player.HasPassword(vehicle.m_CarLockPassword, vehicle.m_CarLockOwner))
return true;
}
return false;
}
};
#endif
