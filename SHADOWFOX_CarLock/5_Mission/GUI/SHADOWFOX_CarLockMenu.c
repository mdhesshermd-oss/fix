#ifndef SHADOWFOX_CARLOCK_DISABLE
#define SHADOWFOX_CARLOCK_DISABLE

class SHADOWFOX_CarLockMenu extends UIScriptedMenu
{
    private ButtonWidget m_BttnOK;
    private ButtonWidget m_BttnCancel;
    private EditBoxWidget m_PasswordInput;
    private TextWidget m_TextOutPut;
    private CarScript m_Car;
    private ButtonWidget m_BttnNum0, m_BttnNum1, m_BttnNum2, m_BttnNum3, m_BttnNum4, m_BttnNum5, m_BttnNum6, m_BttnNum7, m_BttnNum8, m_BttnNum9;
    private ButtonWidget m_BttnClear;
    private ButtonWidget m_BttnDelete;

    void SHADOWFOX_CarLockMenu(CarScript car)
    {
        m_Car = car;
        Print("[SHADOWFOX_CarLock] UI: Constructor called for car: " + car.GetDisplayName());
    }

    override Widget Init()
    {
        Print("[SHADOWFOX_CarLock] UI: Init called");
        layoutRoot = GetGame().GetWorkspace().CreateWidgets("SHADOWFOX_CarLock/Data/GUI/carlockui.layout");
        if (!layoutRoot)
        {
            Print("[SHADOWFOX_CarLock] UI ERROR: Could not find layout file: SHADOWFOX_CarLock/Data/GUI/carlockui.layout");
            return null;
        }

        m_BttnOK = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BttnOK"));
        m_BttnCancel = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BttnCancel"));
        m_PasswordInput = EditBoxWidget.Cast(layoutRoot.FindAnyWidget("PasswordInput"));
        m_TextOutPut = TextWidget.Cast(layoutRoot.FindAnyWidget("TextOutPut"));
        m_BttnNum0 = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BttnNum0"));
        m_BttnNum1 = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BttnNum1"));
        m_BttnNum2 = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BttnNum2"));
        m_BttnNum3 = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BttnNum3"));
        m_BttnNum4 = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BttnNum4"));
        m_BttnNum5 = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BttnNum5"));
        m_BttnNum6 = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BttnNum6"));
        m_BttnNum7 = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BttnNum7"));
        m_BttnNum8 = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BttnNum8"));
        m_BttnNum9 = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BttnNum9"));
        m_BttnClear = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BttnClear"));
        m_BttnDelete = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BttnDelete"));

        layoutRoot.Show(false);
        Print("[SHADOWFOX_CarLock] UI: Init successful");
        return layoutRoot;
    }

    override void OnShow()
    {
        super.OnShow();
        Print("[SHADOWFOX_CarLock] UI: OnShow");
        PPEffects.SetBlurMenu(0.5);
        GetGame().GetInput().ChangeGameFocus(1);
        GetGame().GetUIManager().ShowUICursor(true);
        SetFocus(layoutRoot);
        UpdateUI();
    }

    override void OnHide()
    {
        super.OnHide();
        Print("[SHADOWFOX_CarLock] UI: OnHide");
        PPEffects.SetBlurMenu(0);
        GetGame().GetInput().ResetGameFocus();
        GetGame().GetUIManager().ShowUICursor(false);
    }

    void UpdateUI()
    {
        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
        if (!player) return;

        if (m_Car.m_SF_OwnerId == player.m_SF_LowUid && m_Car.m_SF_Password == -1)
            m_TextOutPut.SetText("Set Password (4 digits)");
        else if (m_Car.m_SF_OwnerId == player.m_SF_LowUid)
            m_TextOutPut.SetText("Change Password (4 digits)");
        else
            m_TextOutPut.SetText("Enter Password (4 digits)");
    }

    override bool OnClick(Widget w, int x, int y, int button)
    {
        if (w == m_BttnCancel) { Close(); return true; }
        if (w == m_BttnOK) { HandlePassword(); return true; }
        if (w == m_BttnClear) { m_PasswordInput.SetText(""); return true; }
        if (w == m_BttnDelete) { DeleteLast(); return true; }

        if (w == m_BttnNum0) Append("0");
        if (w == m_BttnNum1) Append("1");
        if (w == m_BttnNum2) Append("2");
        if (w == m_BttnNum3) Append("3");
        if (w == m_BttnNum4) Append("4");
        if (w == m_BttnNum5) Append("5");
        if (w == m_BttnNum6) Append("6");
        if (w == m_BttnNum7) Append("7");
        if (w == m_BttnNum8) Append("8");
        if (w == m_BttnNum9) Append("9");

        return false;
    }

    void Append(string s)
    {
        string current = m_PasswordInput.GetText();
        if (current.Length() < 4) m_PasswordInput.SetText(current + s);
    }

    void DeleteLast()
    {
        string current = m_PasswordInput.GetText();
        if (current.Length() > 0) m_PasswordInput.SetText(current.Substring(0, current.Length() - 1));
    }

    void HandlePassword()
    {
        string pwd = m_PasswordInput.GetText();
        Print("[SHADOWFOX_CarLock] UI: Handling password input: " + pwd);
        if (pwd.Length() != 4) { m_TextOutPut.SetText("Must be 4 digits!"); return; }

        int val = pwd.ToInt();
        GetRPCManager().SendRPC("SHADOWFOX_CarLock", "SHADOWFOX_CarLockPasswordRequest", new Param2<CarScript, int>(m_Car, val), true);
        m_TextOutPut.SetText("Verifying...");
        m_PasswordInput.SetText("");
    }
};

#endif
