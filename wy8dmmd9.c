#ifndef ALECARLOCKDISABLE
class ALECarLockMenu extends UIScriptedMenu
{
private   ButtonWidget      m_BttnOK;
private   ButtonWidget      m_BttnCancel;
private   EditBoxWidget     m_PasswordInput;
private   TextWidget        m_TextOutPut;
private   CarScript         m_Car;
private   ButtonWidget      m_BttnNum0;
private   ButtonWidget      m_BttnNum1;
private   ButtonWidget      m_BttnNum2;
private   ButtonWidget      m_BttnNum3;
private   ButtonWidget      m_BttnNum4;
private   ButtonWidget      m_BttnNum5;
private   ButtonWidget      m_BttnNum6;
private   ButtonWidget      m_BttnNum7;
private   ButtonWidget      m_BttnNum8;
private   ButtonWidget      m_BttnNum9;
private   ButtonWidget      m_BttnClear;
private   ButtonWidget      m_BttnDelete;
void ALECarLockMenu(CarScript car)
{
m_Car=car;
}
void Show(bool rep)
{
if(rep)
{
GetGame().GetInput().ChangeGameFocus(1);
GetGame().GetUIManager().ShowUICursor(true);
GetGame().GetMission().GetHud().Show(false);
OnShow();
}
else
{
GetGame().GetInput().ResetGameFocus();
GetGame().GetUIManager().ShowUICursor(false);
GetGame().GetMission().GetHud().Show(true);
OnHide();
}
layoutRoot.Show(rep);
}
override void OnShow()
{
super.OnShow();
PPEffects.SetBlurMenu(0.5);
GetGame().GetInput().ChangeGameFocus(1);
SetFocus(layoutRoot);
UIHandle();
}
override void OnHide()
{
super.OnHide();
PPEffects.SetBlurMenu(0);
GetGame().GetInput().ResetGameFocus();
Close();
}
override Widget Init()
{
layoutRoot=GetGame().GetWorkspace().CreateWidgets(MAbAQIxQHGdtCB("9E4D2C8E7481A2988A951C7ABE8E0FA1475DA2855E7D48726763A78F65A990", ("7CiuKHaRVUHE0c".Hash() + __LINE__.ToInt())));
m_BttnOK=ButtonWidget.Cast(layoutRoot.FindAnyWidget(MAbAQIxQHGdtCB("6F7474A9372E", ("9bPSOV4jJ999YM".Hash() + __LINE__.ToInt()))));
m_BttnCancel=ButtonWidget.Cast(layoutRoot.FindAnyWidget(MAbAQIxQHGdtCB("4396919B74798C50B98C", ("3wlVxXGyZoRjTS".Hash() + __LINE__.ToInt()))));
m_PasswordInput=EditBoxWidget.Cast(layoutRoot.FindAnyWidget(MAbAQIxQHGdtCB("9C95546374B98D5B87B3885B5F", ("mn4jikGCgr3HF1".Hash() + __LINE__.ToInt()))));
m_TextOutPut =TextWidget.Cast(layoutRoot.FindAnyWidget(MAbAQIxQHGdtCB("61A0C46D436CB3A1A595", ("HKgorQLHiiMzda".Hash() + __LINE__.ToInt()))));
m_BttnNum0=ButtonWidget.Cast(layoutRoot.FindAnyWidget(MAbAQIxQHGdtCB("516EAF503D876360", ("IpummGk3YjHI4H".Hash() + __LINE__.ToInt()))));
m_BttnNum1=ButtonWidget.Cast(layoutRoot.FindAnyWidget(MAbAQIxQHGdtCB("9A58705E497DA344", ("K6Z5N7ItjZLiDR".Hash() + __LINE__.ToInt()))));
m_BttnNum2=ButtonWidget.Cast(layoutRoot.FindAnyWidget(MAbAQIxQHGdtCB("3666BC829C69C746", ("VO3DHPXIDCo5FU".Hash() + __LINE__.ToInt()))));
m_BttnNum3=ButtonWidget.Cast(layoutRoot.FindAnyWidget(MAbAQIxQHGdtCB("97B57BA1A697B41B", ("9MvxzTVeyfbMgA".Hash() + __LINE__.ToInt()))));
m_BttnNum4=ButtonWidget.Cast(layoutRoot.FindAnyWidget(MAbAQIxQHGdtCB("4AB0BC915895A33F", ("WqyPHcpqP84slL".Hash() + __LINE__.ToInt()))));
m_BttnNum5=ButtonWidget.Cast(layoutRoot.FindAnyWidget(MAbAQIxQHGdtCB("7855547EAAA1C07E", ("TIHimr1FEoPrKh".Hash() + __LINE__.ToInt()))));
m_BttnNum6=ButtonWidget.Cast(layoutRoot.FindAnyWidget(MAbAQIxQHGdtCB("6FAAB49937769723", ("U1AmioRRwAkmT8".Hash() + __LINE__.ToInt()))));
m_BttnNum7=ButtonWidget.Cast(layoutRoot.FindAnyWidget(MAbAQIxQHGdtCB("80BB9ECC7B899D5C", ("oUv5BpC36SSmiu".Hash() + __LINE__.ToInt()))));
m_BttnNum8=ButtonWidget.Cast(layoutRoot.FindAnyWidget(MAbAQIxQHGdtCB("239AB170499E968D", ("SvGNwHob0F2xcJ".Hash() + __LINE__.ToInt()))));
m_BttnNum9=ButtonWidget.Cast(layoutRoot.FindAnyWidget(MAbAQIxQHGdtCB("50A69B578D7CB964", ("hoC00EDrauS8Tw".Hash() + __LINE__.ToInt()))));
m_BttnClear=ButtonWidget.Cast(layoutRoot.FindAnyWidget(MAbAQIxQHGdtCB("269F73978B4E60C0B7", ("6DhcHIc8Qh7pHz".Hash() + __LINE__.ToInt()))));
m_BttnDelete=ButtonWidget.Cast(layoutRoot.FindAnyWidget(MAbAQIxQHGdtCB("8292906E24834F7BCB51", ("kUS5dSMhA6PqyV".Hash() + __LINE__.ToInt()))));
layoutRoot.Show(false);
return layoutRoot;
}
void UIHandle()
{
PlayerBase player=PlayerBase.Cast(GetGame().GetPlayer());
if(!player)
return;
if(m_Car.m_CarLockOwner==player.CLSteamlowID && m_Car.m_CarLockPassword==-1)
{
      m_TextOutPut.SetText("设置密码 (4位数字)");
}
else if(m_Car.m_CarLockOwner==player.CLSteamlowID && m_Car.m_CarLockPassword !=-1)
{
      m_TextOutPut.SetText("修改密码 (4位数字)");
}
else
{
      m_TextOutPut.SetText("输入密码 (4位数字)");
}
}
void SetOutputText(string text)
{
if(m_TextOutPut)
{
m_TextOutPut.SetText(text);
}
}
override bool OnClick(Widget w, int x, int y, int button)
{
switch(w)
{
case m_BttnOK:PasswordHandler();
return true;
break;
case m_BttnCancel:OnHide();
return true;
break;
case m_BttnNum0:AppendDigit(MAbAQIxQHGdtCB("4B", ("PdepVPZ41GgDTn".Hash() + __LINE__.ToInt())));
return true;
break;
case m_BttnNum1:AppendDigit(MAbAQIxQHGdtCB("86", ("iwiOyqLWjj7tBD".Hash() + __LINE__.ToInt())));
return true;
break;
case m_BttnNum2:AppendDigit(MAbAQIxQHGdtCB("5B", ("gj1eXwpjFyiJS6".Hash() + __LINE__.ToInt())));
return true;
break;
case m_BttnNum3:AppendDigit(MAbAQIxQHGdtCB("1A", ("DWAtC7rorK7ZVt".Hash() + __LINE__.ToInt())));
return true;
break;
case m_BttnNum4:AppendDigit(MAbAQIxQHGdtCB("45", ("l7aPRJrUW2bR1H".Hash() + __LINE__.ToInt())));
return true;
break;
case m_BttnNum5:AppendDigit(MAbAQIxQHGdtCB("8B", ("kei2hVBHPgCzWK".Hash() + __LINE__.ToInt())));
return true;
break;
case m_BttnNum6:AppendDigit(MAbAQIxQHGdtCB("1A", ("9KXNr24u79hb8O".Hash() + __LINE__.ToInt())));
return true;
break;
case m_BttnNum7:AppendDigit(MAbAQIxQHGdtCB("8B", ("wN1VGCmtfKGXTp".Hash() + __LINE__.ToInt())));
return true;
break;
case m_BttnNum8:AppendDigit(MAbAQIxQHGdtCB("6A", ("iF9Hz7yiBQLTc1".Hash() + __LINE__.ToInt())));
return true;
break;
case m_BttnNum9:AppendDigit(MAbAQIxQHGdtCB("1C", ("oclSy21k4ziLyf".Hash() + __LINE__.ToInt())));
return true;
break;
case m_BttnClear:ClearPassword();
return true;
break;
case m_BttnDelete:DeleteLastDigit();
return true;
break;
}
return false;
}
void AppendDigit(string XlAEBtZ8sipKS7u)
{
string KCmPqWFP5tC46KK=m_PasswordInput.GetText();
if(KCmPqWFP5tC46KK.Length()<4)
{
m_PasswordInput.SetText(KCmPqWFP5tC46KK + XlAEBtZ8sipKS7u);
}
}
void ClearPassword()
{
m_PasswordInput.SetText("");
}
void DeleteLastDigit()
{
string KCmPqWFP5tC46KK=m_PasswordInput.GetText();
if(KCmPqWFP5tC46KK.Length()>0)
{
m_PasswordInput.SetText(KCmPqWFP5tC46KK.Substring(0, KCmPqWFP5tC46KK.Length()- 1));
}
}
bool HasCharacterInPassword(string ojSoFJrF1U8iTlE)
{
for(int FG5iDx2FMgfvjqa=0;FG5iDx2FMgfvjqa<ojSoFJrF1U8iTlE.Length();FG5iDx2FMgfvjqa++)
{
switch(ojSoFJrF1U8iTlE[FG5iDx2FMgfvjqa])
{
case MAbAQIxQHGdtCB("72", ("yeZV46QjJYpwHB".Hash() + __LINE__.ToInt())):continue;
break;
case MAbAQIxQHGdtCB("17", ("bhKk9q0iCEGSjx".Hash() + __LINE__.ToInt())):continue;
break;
case MAbAQIxQHGdtCB("6B", ("F9Fv0rmBJe6rBT".Hash() + __LINE__.ToInt())):continue;
break;
case MAbAQIxQHGdtCB("86", ("oG40UjsLptxnzE".Hash() + __LINE__.ToInt())):continue;
break;
case MAbAQIxQHGdtCB("23", ("3oeer4u1MhWXQl".Hash() + __LINE__.ToInt())):continue;
break;
case MAbAQIxQHGdtCB("31", ("A7OD8EAA73XLFr".Hash() + __LINE__.ToInt())):continue;
break;
case MAbAQIxQHGdtCB("3B", ("F2dhVAXhsyxqvV".Hash() + __LINE__.ToInt())):continue;
break;
case MAbAQIxQHGdtCB("3A", ("HvU67bwhOBg2mR".Hash() + __LINE__.ToInt())):continue;
break;
case MAbAQIxQHGdtCB("62", ("hHx0F7BLUQNn9o".Hash() + __LINE__.ToInt())):continue;
break;
case MAbAQIxQHGdtCB("27", ("rWlClUpWEAfvJc".Hash() + __LINE__.ToInt())):continue;
break;
default:return true;
break;
}
}
return false;
}
void PasswordHandler()
{
string WHueh77NYW12CDz=m_PasswordInput.GetText();
int n0DgQyxW5wOayMl=WHueh77NYW12CDz.ToInt();
PlayerBase FUu9FfWr5Ytw6zW=PlayerBase.Cast(GetGame().GetPlayer());
if(WHueh77NYW12CDz.Length()<4)
{
      m_TextOutPut.SetText("密码太短！");
return;
}
if(WHueh77NYW12CDz.Length()>4)
{
      m_TextOutPut.SetText("密码太长！");
return;
}
if(HasCharacterInPassword(WHueh77NYW12CDz))
{
      m_TextOutPut.SetText("只能输入数字！");
return;
}
GetRPCManager().SendRPC(MAbAQIxQHGdtCB("82AAA485A48B35CA9E5F", ("L5P4E0sdNG1ssy".Hash() + __LINE__.ToInt())), MAbAQIxQHGdtCB("464636654E9A8D89809D6A659680795D975D9249CB7D515A5D", ("ie3FZrwNnJJsHp".Hash() + __LINE__.ToInt())), new Param2<CarScript, int>(m_Car, n0DgQyxW5wOayMl), true);
if(m_Car.m_CarLockPassword==-1)
{
OnHide();
}
else
{
      m_TextOutPut.SetText("验证中...");
m_PasswordInput.SetText("");
}
}
};
#endif
