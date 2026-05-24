#ifndef ALECARLOCKDISABLE
class ALECarLockCore
{
ref ALECarLockServer m_CarLockServer;
void ALECarLockCore()
{
GetALECarLockNewLogger().LogInfo(SVaOhSvzqXtPGe("3536603C87CD4F884D7A4691A97D28215C5C878261D09F86520B", ("BC2m7Azdozfd9m".Hash() + __LINE__.ToInt())));
#ifdef CLDEBUG
GetALECarLockNewLogger().LogInfo(SVaOhSvzqXtPGe("4944936F412CA4828F6B0144977F56633A79", ("pfzzGTDNdyuKCc".Hash() + __LINE__.ToInt())));
#endif
if(GetGame().IsServer()|| !GetGame().IsMultiplayer())
{
InitServerFolder();
GetALECarLockStandaloneConfig();
m_CarLockServer=new ALECarLockServer;
}
else
{
InitClientFolder();
}
InitRPC();
}
void ~ALECarLockCore(){}
void InitClientFolder()
{
if(!FileExist(SVaOhSvzqXtPGe("79B9C0B1BB9E74779060949B26B174885AA194", ("5awAXW3S8aEMyY".Hash() + __LINE__.ToInt()))))
{
MakeDirectory(SVaOhSvzqXtPGe("28606F986950C291298DA5582C7AB59F966B8B", ("wEdqVRaC6Ux5w6".Hash() + __LINE__.ToInt())));
if(!FileExist(SVaOhSvzqXtPGe("727D8098BB506E5E688932482C97C97FB94E708A99707B80", ("MtqSH9zAtGVlmF".Hash() + __LINE__.ToInt()))))
{
MakeDirectory(SVaOhSvzqXtPGe("5A6FB8629CAFC982296EAB6C51B7D0A6544C987F6BB680BD", ("SKlCxq4BDUQ4TZ".Hash() + __LINE__.ToInt())));
}
}
}
void InitServerFolder()
{
if(!FileExist(SVaOhSvzqXtPGe("115DA3BD9F4E8DB83B6E3E59475669466B8752", ("9BCJvK9Zg6nVGC".Hash() + __LINE__.ToInt()))))
{
MakeDirectory(SVaOhSvzqXtPGe("6FB98089B584A186333EA966259F5D3B9C586B", ("e9c9uWvokM9Ig7".Hash() + __LINE__.ToInt())));
if(!FileExist(SVaOhSvzqXtPGe("65C78BAF465A8BB95868637A328E716F83475C0F686BB6D0", ("xEEE4DxvO0InnI".Hash() + __LINE__.ToInt()))))
{
MakeDirectory(SVaOhSvzqXtPGe("108160C2BBC76FB98F8F40557E86A7606D96AE4577C07166", ("1v1UmDruOBJoxA".Hash() + __LINE__.ToInt())));
}
}
}
void InitRPC()
{
if(GetGame().IsServer())
{
GetRPCManager().AddRPC(SVaOhSvzqXtPGe("7F347363B0B565A3AB63", ("yL1cilPDVtWv8Y".Hash() + __LINE__.ToInt())), SVaOhSvzqXtPGe("855A2E95A4BD7FB35DC2A8C173BE4C8E6D", ("OHt9z0ER5PvPHc".Hash() + __LINE__.ToInt())), m_CarLockServer, SingeplayerExecutionType.Client);
GetRPCManager().AddRPC(SVaOhSvzqXtPGe("9EA8479B60796D9F7D81", ("EWevVnXQ8fWIV6".Hash() + __LINE__.ToInt())), SVaOhSvzqXtPGe("575C8E3952BD91567E627190D18BA0C1906D808D8CCB8B74A7", ("oe5tYCiQxlJi0N".Hash() + __LINE__.ToInt())), m_CarLockServer, SingeplayerExecutionType.Client);
}
}
static void LogVehicleAction(string actionType, CarScript car, PlayerBase player)
{
if(!car || !player)
return;
string dE08JyD0xSTyj53=SVaOhSvzqXtPGe("AF5C64BCA5B7B2", ("A40FT4kIvhDIgN".Hash() + __LINE__.ToInt()));
string vFCi1yNhHg1MsBk=SVaOhSvzqXtPGe("8BBC696A5A7754", ("zWXSqAkdpH99vv".Hash() + __LINE__.ToInt()));
if(player.GetIdentity())
{
dE08JyD0xSTyj53=player.GetIdentity().GetName();
vFCi1yNhHg1MsBk=player.GetIdentity().GetPlainId();
}
string aUDSffyKgNgtD83=actionType + SVaOhSvzqXtPGe("2D25", ("LscpRLHDlf4AAa".Hash() + __LINE__.ToInt())) + car.GetDisplayName()+ SVaOhSvzqXtPGe("617ECD367C9D638EB85405", ("bOpSLpwi0JKiw4".Hash() + __LINE__.ToInt())) + dE08JyD0xSTyj53 + SVaOhSvzqXtPGe("666939AB62BB558D3D4270", ("GN4M5IKQfewsKo".Hash() + __LINE__.ToInt())) + vFCi1yNhHg1MsBk + SVaOhSvzqXtPGe("37125C627694B9788D774F6A984C", ("j7meXeD2H3tIKs".Hash() + __LINE__.ToInt())) + car.GetPosition();
GetALECarLockLogger().Log(aUDSffyKgNgtD83);
}
static void LogVehicleDetails(string actionType, CarScript car)
{
if(!car)
return;
string biQMRVrQP8cM4CA=car.GetCarLockOwnerName();
string vFCi1yNhHg1MsBk=car.GetCarLockFullSteamID();
if(biQMRVrQP8cM4CA=="")
biQMRVrQP8cM4CA=SVaOhSvzqXtPGe("5595C0A5BD827F", ("uifcEkabwKhNnk".Hash() + __LINE__.ToInt()));
if(vFCi1yNhHg1MsBk=="")
vFCi1yNhHg1MsBk=SVaOhSvzqXtPGe("5A827C88605D9E", ("xyUirQzHgUHq6N".Hash() + __LINE__.ToInt()));
string aUDSffyKgNgtD83=actionType + SVaOhSvzqXtPGe("6979", ("PkSjI7FYFfLPfw".Hash() + __LINE__.ToInt())) + car.GetDisplayName()+ SVaOhSvzqXtPGe("0272A958AEA6638E66", ("jsCycavNHTMjDy".Hash() + __LINE__.ToInt())) + biQMRVrQP8cM4CA + SVaOhSvzqXtPGe("4367458876B0C09397406E", ("Fq2RiL2OlwElJe".Hash() + __LINE__.ToInt())) + vFCi1yNhHg1MsBk + SVaOhSvzqXtPGe("402B8752CCA265A3B3B17B49", ("S2o73Mfvccx7TJ".Hash() + __LINE__.ToInt())) + car.GetPosition()+ SVaOhSvzqXtPGe("18306FA27E6E999E640C", ("BrdRfcb66qDmkj".Hash() + __LINE__.ToInt())) + car.m_CarLock_IsLocked + SVaOhSvzqXtPGe("4E4F6ABCC9BEB47E97644735", ("EJjiMmioGRyUQE".Hash() + __LINE__.ToInt())) + car.m_CarLockPassword + SVaOhSvzqXtPGe("75", ("fh8xGdeGvHYEgH".Hash() + __LINE__.ToInt()));
GetALECarLockLogger().Log(aUDSffyKgNgtD83);
}
};
#endif
