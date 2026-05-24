#ifndef ALECARLOCKDISABLE
modded class MissionGameplay extends MissionBase
{
void MissionGameplay()
{
m_CarLockCore=NULL;
GetALECarLockCore();
}
void TryToLockVehicleNearby(PlayerBase player)
{
array<Object>xa68fgvFcucmIBB=new array<Object>;
GetGame().GetObjectsAtPosition3D(player.GetPosition(), 5, xa68fgvFcucmIBB, NULL);
if(xa68fgvFcucmIBB.Count()<1)return;
for(int FG5iDx2FMgfvjqa=0;FG5iDx2FMgfvjqa<xa68fgvFcucmIBB.Count();FG5iDx2FMgfvjqa++)
{
CarScript GOVaZkaGiMpJx7E=CarScript.Cast(xa68fgvFcucmIBB[FG5iDx2FMgfvjqa]);
if(GOVaZkaGiMpJx7E)
{
if(!GOVaZkaGiMpJx7E.m_CarLock_IsLocked && GOVaZkaGiMpJx7E.m_CarLockOwner !=-1 &&(GOVaZkaGiMpJx7E.m_CarLockOwner==player.CLSteamlowID || player.HasPassword(GOVaZkaGiMpJx7E.m_CarLockPassword,GOVaZkaGiMpJx7E.m_CarLockOwner)))
GetRPCManager().SendRPC(uInqZQPvODtecR("69AA523B54C44254569F", ("dtA9kqehPZjtCe".Hash() + __LINE__.ToInt())), uInqZQPvODtecR("96689594B3653967B972648D877795A2AB", ("YByPM0Kxm0dRmd".Hash() + __LINE__.ToInt())),  new Param2<CarScript, bool>(GOVaZkaGiMpJx7E,true), true, NULL);
else if(GOVaZkaGiMpJx7E.m_CarLock_IsLocked && GOVaZkaGiMpJx7E.m_CarLockOwner !=-1 &&(GOVaZkaGiMpJx7E.m_CarLockOwner==player.CLSteamlowID || player.HasPassword(GOVaZkaGiMpJx7E.m_CarLockPassword,GOVaZkaGiMpJx7E.m_CarLockOwner)))
GetRPCManager().SendRPC(uInqZQPvODtecR("51934E3594769A509BC7", ("BFWdRLiFFYI8SO".Hash() + __LINE__.ToInt())), uInqZQPvODtecR("627B7E60BF622E8E8EA66E6C74B2B0D25E", ("J0sJBElIaexEC4".Hash() + __LINE__.ToInt())),  new Param2<CarScript, bool>(GOVaZkaGiMpJx7E,false), true, NULL);
return;
}
}
}
override void OnUpdate(float timeslice)
{
super.OnUpdate(timeslice);
PlayerBase FUu9FfWr5Ytw6zW=PlayerBase.Cast(GetGame().GetPlayer());
if(!GetUIManager().GetMenu() && GetUApi().GetInputByName(uInqZQPvODtecR("3A8B434A905477658A8867BDBD92635B", ("p3AL92hhPE0Fk5".Hash() + __LINE__.ToInt()))).LocalPress())
TryToLockVehicleNearby(FUu9FfWr5Ytw6zW);
}
};
#endif
