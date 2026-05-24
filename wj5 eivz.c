class ALECarLockStandaloneConfig
{
int LockPickChance=50;
ref array<string>Admins;
ref array<string>LockPickTools;
bool EnableAutoLock=true;
int AutoLockDistance=100;
bool EnablePasswordSharing=true;
bool EnablePlayerDataTracking=true;
void ALECarLockStandaloneConfig()
{
Admins=new array<string>();
Admins.Insert(pZOYMTiZZdYpci("1E20337D697A4E3B3B9344331F1C3A3484", ("DFFmwuFQQrkImE".Hash() + __LINE__.ToInt())));
LockPickTools=new array<string>();
LockPickTools.Insert(pZOYMTiZZdYpci("8B5E8987C97A799D", ("PjTe4AbekFPsIc".Hash() + __LINE__.ToInt())));
LockPickTools.Insert(pZOYMTiZZdYpci("4D57CF5F8F4D539E846F98", ("DlvoyFXfviO6zs".Hash() + __LINE__.ToInt())));
}
static ref ALECarLockStandaloneConfig Load()
{
ALECarLockStandaloneConfig KXlCDUX0iPjGya3=new ALECarLockStandaloneConfig();
if(FileExist(pZOYMTiZZdYpci("04BF7B59A56B998B80856A286D907176988A67245476AB57555D8BAC62B25671AE", ("eRClBqb0g3pBFC".Hash() + __LINE__.ToInt()))))
{
JsonFileLoader<ALECarLockStandaloneConfig>.JsonLoadFile(pZOYMTiZZdYpci("828C78646F656D4C74663361475EA97E5E91904D9E72A5C19D589E8B79B5AF558B", ("Cc6EgF9KPHNNN6".Hash() + __LINE__.ToInt())), KXlCDUX0iPjGya3);
            GetALECarLockNewLogger().LogInfo("配置文件已加载: $profile:ALECarLock/Settings.json");
            GetALECarLockNewLogger().LogInfo("撬锁成功率: " + KXlCDUX0iPjGya3.LockPickChance + "%");
string cP8JkM11AVaqWlj;
if(KXlCDUX0iPjGya3.EnableAutoLock)
{
                cP8JkM11AVaqWlj="开启";
}
else
{
                cP8JkM11AVaqWlj="关闭";
}
            GetALECarLockNewLogger().LogInfo("自动上锁: " + cP8JkM11AVaqWlj);
            GetALECarLockNewLogger().LogInfo("自动上锁距离: " + KXlCDUX0iPjGya3.AutoLockDistance + "米");
string gjjjZksIYNktxhf;
if(KXlCDUX0iPjGya3.EnablePasswordSharing)
{
                gjjjZksIYNktxhf="开启";
}
else
{
                gjjjZksIYNktxhf="关闭";
}
            GetALECarLockNewLogger().LogInfo("密码共享: " + gjjjZksIYNktxhf);
string OzaZrafTxisIlHu;
if(KXlCDUX0iPjGya3.EnablePlayerDataTracking)
{
                OzaZrafTxisIlHu="开启";
}
else
{
                OzaZrafTxisIlHu="关闭";
}
            GetALECarLockNewLogger().LogInfo("玩家数据追踪: " + OzaZrafTxisIlHu);
            GetALECarLockNewLogger().LogInfo("管理员数量: " + KXlCDUX0iPjGya3.Admins.Count());
if(KXlCDUX0iPjGya3.LockPickTools && KXlCDUX0iPjGya3.LockPickTools.Count()>0)
{
                GetALECarLockNewLogger().LogInfo("撬锁工具数量: " + KXlCDUX0iPjGya3.LockPickTools.Count());
for(int KyXkNpdiKwCaKip=0;KyXkNpdiKwCaKip<KXlCDUX0iPjGya3.LockPickTools.Count();KyXkNpdiKwCaKip++)
{
                    GetALECarLockNewLogger().LogInfo("撬锁工具 #" +(KyXkNpdiKwCaKip+1)+ ": " + KXlCDUX0iPjGya3.LockPickTools[KyXkNpdiKwCaKip]);
}
}
else
{
                GetALECarLockNewLogger().LogInfo("警告: 未配置撬锁工具，无法撬锁");
}
if(KXlCDUX0iPjGya3.Admins.Count()>0)
{
for(int FG5iDx2FMgfvjqa=0;FG5iDx2FMgfvjqa<KXlCDUX0iPjGya3.Admins.Count();FG5iDx2FMgfvjqa++)
{
                    GetALECarLockNewLogger().LogInfo("管理员 #" +(FG5iDx2FMgfvjqa+1)+ ": " + KXlCDUX0iPjGya3.Admins[FG5iDx2FMgfvjqa]);
}
}
else
{
                GetALECarLockNewLogger().LogInfo("警告: 未配置任何管理员");
}
}
else
{
KXlCDUX0iPjGya3.Save();
            GetALECarLockNewLogger().LogInfo("配置文件不存在，已创建默认配置: $profile:ALECarLock/Settings.json");
            GetALECarLockNewLogger().LogInfo("请编辑配置文件添加管理员SteamID");
}
return KXlCDUX0iPjGya3;
}
void Save()
{
if(!FileExist(pZOYMTiZZdYpci("1556B780744E4F5C7B896F7F8663AF4F65488E", ("f7wV5Sig5AiKIZ".Hash() + __LINE__.ToInt()))))
{
MakeDirectory(pZOYMTiZZdYpci("4A9A9C8DA599A458288E5D632A877F97809674", ("JGw8bvoxCfpE07".Hash() + __LINE__.ToInt())));
}
JsonFileLoader<ALECarLockStandaloneConfig>.JsonSaveFile(pZOYMTiZZdYpci("4F739FA486506E70634C78727A5089579E4A8141A748C680688482C7199F57C5BE", ("ezMeS3uyRwCB8T".Hash() + __LINE__.ToInt())), this);
}
}
static ref ALECarLockStandaloneConfig g_CarLockStandaloneConfig;
static ALECarLockStandaloneConfig GetALECarLockStandaloneConfig()
{
if(!g_CarLockStandaloneConfig)
g_CarLockStandaloneConfig=ALECarLockStandaloneConfig.Load();
return g_CarLockStandaloneConfig;
}
