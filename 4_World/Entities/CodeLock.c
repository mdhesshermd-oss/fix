modded class CodeLock
{
    override void SetActions()
    {
        super.SetActions();
        
        // Добавление кастомных действий к стандартному CodeLock
        AddAction(ActionAttachCarLock);
        AddAction(ActionSHADOWFOX_CarLockMenu);
        
        Print("[SF_DEBUG] Actions injected into CodeLock class successfully");
    }
}