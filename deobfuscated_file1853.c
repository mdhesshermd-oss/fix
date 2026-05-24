#ifndef ALECARLOCKDISABLE
class ALECarLockServer
{
void ALECarLockServer()
{
GetALECarLockNewLogger().LogInfo(hmWYVbGgGkEwGS("433E267D5D7CA38C81743392B668928F39893751985C54646FBD", ("aXyvZpuxnNbNcN".Hash() + __LINE__.ToInt())));
}
void ALECarLockRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
if(!GetGame().IsServer())
return;
Param2<CarScript, bool>jQhxs80yRxFpE3f;
if(!ctx.Read(jQhxs80yRxFpE3f))
return;
PlayerBase FUu9FfWr5Ytw6zW=CLGetPlayerByIdentity(sender);
CarScript GOVaZkaGiMpJx7E=jQhxs80yRxFpE3f.param1;
if(!GOVaZkaGiMpJx7E)
{
return;
}
GOVaZkaGiMpJx7E.SetCarLock(jQhxs80yRxFpE3f.param2);
if(jQhxs80yRxFpE3f.param2)
{
GOVaZkaGiMpJx7E.SetSoundToPlay(1);
}else GOVaZkaGiMpJx7E.SetSoundToPlay(2);
}
void ALECarLockPasswordRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
{
if(!GetGame().IsServer())
return;
Param2<CarScript, int>jQhxs80yRxFpE3f;
if(!ctx.Read(jQhxs80yRxFpE3f))
return;
PlayerBase FUu9FfWr5Ytw6zW=CLGetPlayerByIdentity(sender);
CarScript GOVaZkaGiMpJx7E=jQhxs80yRxFpE3f.param1;
if(!GOVaZkaGiMpJx7E)
{
return;
}
string fhn82otWJ39kDaA=sender.GetPlainId();
int rfnLnhmFUoPpUP9=jQhxs80yRxFpE3f.param2;
bool ELsVBqYGTDCAFCn=(GOVaZkaGiMpJx7E.m_CarLockOwner==FUu9FfWr5Ytw6zW.CLSteamlowID);
ScriptRPC PT83trUzTgs5LRI=new ScriptRPC();
int YaW89gELLOMk9f5;
if(GOVaZkaGiMpJx7E.m_CarLockPassword==-1)
{
if(ELsVBqYGTDCAFCn)
{
GOVaZkaGiMpJx7E.SetCarLockPassword(rfnLnhmFUoPpUP9);
GOVaZkaGiMpJx7E.SetCarLock(true);
         GetALECarLockNewLogger().LogInfo("设置密码成功: " + sender.GetName()+ " - 车辆: " + GOVaZkaGiMpJx7E.GetDisplayName());
if(FUu9FfWr5Ytw6zW)
{
EntityAI TIiNEsBnP5WGq4Z=FUu9FfWr5Ytw6zW.GetHumanInventory().GetEntityInHands();
if(TIiNEsBnP5WGq4Z && TIiNEsBnP5WGq4Z.IsKindOf(hmWYVbGgGkEwGS("427F5F6D9C73AB666FA9", ("AjgYfVP047LmdH".Hash() + __LINE__.ToInt()))))
{
TIiNEsBnP5WGq4Z.Delete();
}
}
PT83trUzTgs5LRI.Write(true);
PT83trUzTgs5LRI.Write(rfnLnhmFUoPpUP9);
PT83trUzTgs5LRI.Write(GOVaZkaGiMpJx7E.m_CarLockOwner);
PT83trUzTgs5LRI.Send(FUu9FfWr5Ytw6zW, 78910, true, FUu9FfWr5Ytw6zW.GetIdentity());
}
}
else if(ELsVBqYGTDCAFCn)
{
YaW89gELLOMk9f5=GOVaZkaGiMpJx7E.m_CarLockPassword;
GOVaZkaGiMpJx7E.SetCarLockPassword(rfnLnhmFUoPpUP9);
       GetALECarLockNewLogger().LogInfo("修改密码成功: " + sender.GetName()+ " - 车辆: " + GOVaZkaGiMpJx7E.GetDisplayName());
PT83trUzTgs5LRI.Write(true);
PT83trUzTgs5LRI.Write(rfnLnhmFUoPpUP9);
PT83trUzTgs5LRI.Write(GOVaZkaGiMpJx7E.m_CarLockOwner);
PT83trUzTgs5LRI.Send(FUu9FfWr5Ytw6zW, 78911, true, FUu9FfWr5Ytw6zW.GetIdentity());
}
else
{
if(rfnLnhmFUoPpUP9==GOVaZkaGiMpJx7E.m_CarLockPassword)
{
         GetALECarLockNewLogger().LogInfo("密码验证成功: " + sender.GetName()+ " - 车辆: " + GOVaZkaGiMpJx7E.GetDisplayName());
PT83trUzTgs5LRI.Write(true);
PT83trUzTgs5LRI.Write(rfnLnhmFUoPpUP9);
PT83trUzTgs5LRI.Write(GOVaZkaGiMpJx7E.m_CarLockOwner);
PT83trUzTgs5LRI.Send(FUu9FfWr5Ytw6zW, 78910, true, FUu9FfWr5Ytw6zW.GetIdentity());
}
else
{
PT83trUzTgs5LRI.Write(false);
PT83trUzTgs5LRI.Write(-1);
PT83trUzTgs5LRI.Write(-1);
PT83trUzTgs5LRI.Send(FUu9FfWr5Ytw6zW, 78910, true, FUu9FfWr5Ytw6zW.GetIdentity());
}
}
}
};
#endif
