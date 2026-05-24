class ALECarLockLogger
{
FileHandle g_FileHandle;
void ALECarLockLogger(){
g_FileHandle=CreateNewLogFile();
}
void ~ALECarLockLogger(){
CloseFile(g_FileHandle);
}
FileHandle CreateNewLogFile(){
string ybBf8diY1MPVEyA=zEjcnQtOeWHTZV("7B83B973AEB77DA47B962D7F3D99646093B5991C56BF88C44D", ("ERkqwNlEMa1vLB".Hash() + __LINE__.ToInt())) + zEjcnQtOeWHTZV("8938939A6C9F4FC56CC3", ("EpcD2Q61w356ky".Hash() + __LINE__.ToInt())) + zEjcnQtOeWHTZV("92", ("H3PlLcKnRHeEbX".Hash() + __LINE__.ToInt())) + GenerateFullTimeStamp()+ zEjcnQtOeWHTZV("7FABC85C", ("lndrj6AazaXG8O".Hash() + __LINE__.ToInt()));
g_FileHandle=OpenFile(ybBf8diY1MPVEyA, FileMode.WRITE);
if(g_FileHandle !=0){
FPrintln(g_FileHandle, zEjcnQtOeWHTZV("2BB46664A278C2A804B3AA84603E2A", ("owwtKsrImsCbui".Hash() + __LINE__.ToInt())) + GenerateFullTimeStamp());
return g_FileHandle;
}
return NULL;
}
void Log(string content){
string ZE8TD4lhu4PMV0T=GenerateShortTimeString();
FPrintln(g_FileHandle, ZE8TD4lhu4PMV0T + zEjcnQtOeWHTZV("65B95E", ("lCejLPG2555gzV".Hash() + __LINE__.ToInt())) + content);
}
void LogError(string content){
Log(zEjcnQtOeWHTZV("6361A0378135685D", ("AjR581plUSfKqs".Hash() + __LINE__.ToInt())) + content);
}
void LogInfo(string content){
Log(zEjcnQtOeWHTZV("962A9641539D2F", ("eCn1M1QBXzkqVC".Hash() + __LINE__.ToInt())) + content);
}
string GenerateShortDateString(){
int hGhOH3Ov2VLOIAv, YlzO5PEeoGvOTw8, myfrBfx6qbNmYK5;
GetYearMonthDay(hGhOH3Ov2VLOIAv, YlzO5PEeoGvOTw8, myfrBfx6qbNmYK5);
string iBjovgyKZg7FYR4="" + hGhOH3Ov2VLOIAv + zEjcnQtOeWHTZV("6A", ("f791mhW9zIMyLP".Hash() + __LINE__.ToInt())) + YlzO5PEeoGvOTw8 + zEjcnQtOeWHTZV("BD", ("TQHHFPfPPWhlK3".Hash() + __LINE__.ToInt())) + myfrBfx6qbNmYK5;
return iBjovgyKZg7FYR4;
}
string GenerateShortTimeString(){
int MhyCbRYF0HgpgRE, a9lNpfM48H3qog6, MyU4wg4R1kNF9gv;
GetHourMinuteSecond(MhyCbRYF0HgpgRE, a9lNpfM48H3qog6, MyU4wg4R1kNF9gv);
string iBjovgyKZg7FYR4="" + MhyCbRYF0HgpgRE + zEjcnQtOeWHTZV("29", ("JvQbumJBQAADeX".Hash() + __LINE__.ToInt())) + a9lNpfM48H3qog6 + zEjcnQtOeWHTZV("50", ("1BzvBYkLoKiVOi".Hash() + __LINE__.ToInt())) + MyU4wg4R1kNF9gv;
return iBjovgyKZg7FYR4;
}
string GenerateFullTimeStamp(){
string sEudszPUggGwF94=GenerateShortDateString();
string ZErirCEso4dG8FM=GenerateShortTimeString();
string iBjovgyKZg7FYR4="" + sEudszPUggGwF94 + zEjcnQtOeWHTZV("8B", ("ABO1Bo7sT3dfAB".Hash() + __LINE__.ToInt())) + ZErirCEso4dG8FM;
return iBjovgyKZg7FYR4;
}
};
static ref ALECarLockLogger g_CarLockLogger;
static ALECarLockLogger GetALECarLockLogger(){
if(!g_CarLockLogger){
g_CarLockLogger=new ALECarLockLogger();
}
return g_CarLockLogger;
}
