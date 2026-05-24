class VehicleRecordData
{
int VehiclePassword;
string FirstLockTime;
vector LastKnownPosition;
string LastUpdateTime;
string VehicleType;
void VehicleRecordData()
{
VehiclePassword=-1;
FirstLockTime="";
        LastKnownPosition="0 0 0";
LastUpdateTime="";
VehicleType="";
}
}
class PlayerVehicleData
{
string SteamID;
string PlayerName;
ref map<string, ref VehicleRecordData>Vehicles;
void PlayerVehicleData()
{
SteamID="";
PlayerName="";
Vehicles=new map<string, ref VehicleRecordData>();
}
static string GetFilePath(string steamID)
{
return GSDPEpaObjqmnB("10689A7EB0B8B38093569F9E4A7F668495A69F3B3A84C77C82", ("pmzQ5jmrB78xyq".Hash() + __LINE__.ToInt())) + steamID + GSDPEpaObjqmnB("6994CD8C66", ("UisL6lOKdVUgkf".Hash() + __LINE__.ToInt()));
}
static ref PlayerVehicleData Load(string steamID)
{
PlayerVehicleData jQhxs80yRxFpE3f=new PlayerVehicleData();
jQhxs80yRxFpE3f.SteamID=steamID;
string ybBf8diY1MPVEyA=GetFilePath(steamID);
if(FileExist(ybBf8diY1MPVEyA))
{
JsonFileLoader<PlayerVehicleData>.JsonLoadFile(ybBf8diY1MPVEyA, jQhxs80yRxFpE3f);
            GetALECarLockNewLogger().LogInfo("玩家数据已加载: " + steamID);
}
else
{
            GetALECarLockNewLogger().LogInfo("创建新玩家数据: " + steamID);
}
return jQhxs80yRxFpE3f;
}
void Save()
{
if(!FileExist(GSDPEpaObjqmnB("5BCB93599EC8C57A7137889A52797A6D4F4E95768D7A756C", ("KFeGIBZH44WTv9".Hash() + __LINE__.ToInt()))))
{
MakeDirectory(GSDPEpaObjqmnB("5CC8735794A34D4927257E49927FCF9559548B53585B8999", ("oWurULhCyB3pyC".Hash() + __LINE__.ToInt())));
}
string ybBf8diY1MPVEyA=GetFilePath(SteamID);
JsonFileLoader<PlayerVehicleData>.JsonSaveFile(ybBf8diY1MPVEyA, this);
        GetALECarLockNewLogger().LogInfo("玩家数据已保存: " + SteamID);
}
void AddOrUpdateVehicle(string vehicleID, int password, vector position, string vehicleType)
{
VehicleRecordData wOQLzTJfZKT4Ft6;
if(Vehicles.Contains(vehicleID))
{
wOQLzTJfZKT4Ft6=Vehicles.Get(vehicleID);
wOQLzTJfZKT4Ft6.VehiclePassword=password;
wOQLzTJfZKT4Ft6.LastKnownPosition=position;
wOQLzTJfZKT4Ft6.LastUpdateTime=GetCurrentDateTimeString();
wOQLzTJfZKT4Ft6.VehicleType=vehicleType;
}
else
{
wOQLzTJfZKT4Ft6=new VehicleRecordData();
wOQLzTJfZKT4Ft6.VehiclePassword=password;
wOQLzTJfZKT4Ft6.FirstLockTime=GetCurrentDateTimeString();
wOQLzTJfZKT4Ft6.LastKnownPosition=position;
wOQLzTJfZKT4Ft6.LastUpdateTime=wOQLzTJfZKT4Ft6.FirstLockTime;
wOQLzTJfZKT4Ft6.VehicleType=vehicleType;
Vehicles.Insert(vehicleID, wOQLzTJfZKT4Ft6);
}
Save();
}
void RemoveVehicle(string vehicleID)
{
if(Vehicles.Contains(vehicleID))
{
Vehicles.Remove(vehicleID);
Save();
}
}
VehicleRecordData GetVehicleRecord(string vehicleID)
{
if(Vehicles.Contains(vehicleID))
{
return Vehicles.Get(vehicleID);
}
return null;
}
static string GetCurrentDateTimeString()
{
int hGhOH3Ov2VLOIAv, YlzO5PEeoGvOTw8, myfrBfx6qbNmYK5, MhyCbRYF0HgpgRE, a9lNpfM48H3qog6, MyU4wg4R1kNF9gv;
GetYearMonthDay(hGhOH3Ov2VLOIAv, YlzO5PEeoGvOTw8, myfrBfx6qbNmYK5);
GetHourMinuteSecond(MhyCbRYF0HgpgRE, a9lNpfM48H3qog6, MyU4wg4R1kNF9gv);
string lFNUP59ASvMa7ZG=hGhOH3Ov2VLOIAv.ToString()+ GSDPEpaObjqmnB("84", ("RV5Cw0BDJiU0x7".Hash() + __LINE__.ToInt())) + YlzO5PEeoGvOTw8.ToStringLen(2)+ GSDPEpaObjqmnB("15", ("0z6q8x7nvr4Nyq".Hash() + __LINE__.ToInt())) + myfrBfx6qbNmYK5.ToStringLen(2)+ GSDPEpaObjqmnB("4B", ("uJoHZUxVlTm8cO".Hash() + __LINE__.ToInt())) + MhyCbRYF0HgpgRE.ToStringLen(2)+ GSDPEpaObjqmnB("2D", ("wjil0dvCIG3OZj".Hash() + __LINE__.ToInt())) + a9lNpfM48H3qog6.ToStringLen(2)+ GSDPEpaObjqmnB("8E", ("dOuhOj4tvV8sMm".Hash() + __LINE__.ToInt())) + MyU4wg4R1kNF9gv.ToStringLen(2);
return lFNUP59ASvMa7ZG;
}
}
static ref map<string, ref PlayerVehicleData>g_PlayerVehicleDataCache;
static PlayerVehicleData GetPlayerVehicleData(string steamID)
{
if(!g_PlayerVehicleDataCache)
{
g_PlayerVehicleDataCache=new map<string, ref PlayerVehicleData>();
}
if(g_PlayerVehicleDataCache.Contains(steamID))
{
return g_PlayerVehicleDataCache.Get(steamID);
}
PlayerVehicleData jQhxs80yRxFpE3f=PlayerVehicleData.Load(steamID);
g_PlayerVehicleDataCache.Insert(steamID, jQhxs80yRxFpE3f);
return jQhxs80yRxFpE3f;
}
