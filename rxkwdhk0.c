class ALECarLockNewLogger
{
private string m_LoggerPrefixName;
void ALECarLockNewLogger(string prefixname="")
{
m_LoggerPrefixName=prefixname;
}
void Log(string txt)
{
if(GetGame().IsServer())
{
string ybBf8diY1MPVEyA=DvytbJOhieexpC("07906C95BB8AA5794E788E636BBEB7A2816BBF788496B06A2541804D3BA794727FA9657E", ("aLgJsErSdklPPX".Hash() + __LINE__.ToInt())) + GetDate()+ DvytbJOhieexpC("82B99C60", ("RWELTwPnicxLmF".Hash() + __LINE__.ToInt()));
FileHandle dQ9AhzQKaz4y4Tt=OpenFile(ybBf8diY1MPVEyA, FileMode.APPEND);
if(dQ9AhzQKaz4y4Tt !=0)
{
FPrintln(dQ9AhzQKaz4y4Tt, GetDate()+ DvytbJOhieexpC("638572", ("bA8P1NH3QoDI72".Hash() + __LINE__.ToInt())) + m_LoggerPrefixName + DvytbJOhieexpC("175C36", ("tYasStieGRk8E5".Hash() + __LINE__.ToInt())) + txt);
CloseFile(dQ9AhzQKaz4y4Tt);
}
}
}
void LogInfo(string txt)
{
Log(DvytbJOhieexpC("5481665075C40B", ("HINa6b5C8wGAGa".Hash() + __LINE__.ToInt())) + txt);
}
void LogError(string txt)
{
Log(DvytbJOhieexpC("615E9141342DAE5A", ("VDwWLCVyzYjXPI".Hash() + __LINE__.ToInt())) + txt);
}
private string GetDate()
{
int hGhOH3Ov2VLOIAv, YlzO5PEeoGvOTw8, myfrBfx6qbNmYK5, MhyCbRYF0HgpgRE, a9lNpfM48H3qog6, MyU4wg4R1kNF9gv;
GetYearMonthDay(hGhOH3Ov2VLOIAv, YlzO5PEeoGvOTw8, myfrBfx6qbNmYK5);
GetHourMinuteSecond(MhyCbRYF0HgpgRE, a9lNpfM48H3qog6, MyU4wg4R1kNF9gv);
string jt0LZiBOe1ABJN1=hGhOH3Ov2VLOIAv.ToString()+ DvytbJOhieexpC("9F", ("vdnk91s8KMUGmO".Hash() + __LINE__.ToInt())) + YlzO5PEeoGvOTw8.ToString()+ DvytbJOhieexpC("A2", ("Op9DlXZ5UqIUuS".Hash() + __LINE__.ToInt())) + myfrBfx6qbNmYK5.ToString()+ DvytbJOhieexpC("54", ("p9E4Wkd0HELaAl".Hash() + __LINE__.ToInt())) + MhyCbRYF0HgpgRE.ToString()+ DvytbJOhieexpC("BC", ("43rGO5DhGH2sz0".Hash() + __LINE__.ToInt())) + a9lNpfM48H3qog6.ToString()+ DvytbJOhieexpC("40", ("gs7ukLhCPKhCfx".Hash() + __LINE__.ToInt())) + MyU4wg4R1kNF9gv.ToString();
return jt0LZiBOe1ABJN1;
}
}
static ref ALECarLockNewLogger g_CarLockNewLogger;
static ALECarLockNewLogger GetALECarLockNewLogger()
{
if(!g_CarLockNewLogger)
g_CarLockNewLogger=new ALECarLockNewLogger(DvytbJOhieexpC("7F3D95658C8FAAA9637D", ("HovcInhXF5kPTO".Hash() + __LINE__.ToInt())));
return g_CarLockNewLogger;
}
