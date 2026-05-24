class ALECarLockData
{
ref TIntArray CarLockPasswords;
ref TIntArray CarLockOwners;
void ALECarLockData()
{
CarLockPasswords=new TIntArray;
CarLockOwners=new TIntArray;
}
void Save(string id)
{
if(!FileExist(ALE_CL_CONFIG_ROOT_CLIENT))
{
MakeDirectory(ALE_CL_CONFIG_ROOT_CLIENT);
}
if(!FileExist(ALE_CL_CONFIG_DIR_CLIENT))
{
MakeDirectory(ALE_CL_CONFIG_DIR_CLIENT);
}
string lLOmlI8AhHZy4sf=ALE_CL_DATA + hPEBKUeamcCueX("59", ("u9stv9vXArHdHx".Hash() + __LINE__.ToInt())) + id + hPEBKUeamcCueX("637E96A8AE", ("aGjGhsktKKL5kZ".Hash() + __LINE__.ToInt()));
JsonFileLoader<ALECarLockData>.JsonSaveFile(lLOmlI8AhHZy4sf, this);
}
static ref ALECarLockData Load(string id){
ref ALECarLockData gWOZgATr6jds0jx=new ALECarLockData();
string lLOmlI8AhHZy4sf=ALE_CL_DATA + hPEBKUeamcCueX("69", ("3YxzxLAintHwmm".Hash() + __LINE__.ToInt())) + id + hPEBKUeamcCueX("7881ABABC2", ("4ZiuukYxBAXPgz".Hash() + __LINE__.ToInt()));
if(FileExist(lLOmlI8AhHZy4sf)){
JsonFileLoader<ALECarLockData>.JsonLoadFile(lLOmlI8AhHZy4sf, gWOZgATr6jds0jx);
}
return gWOZgATr6jds0jx;
}
}
