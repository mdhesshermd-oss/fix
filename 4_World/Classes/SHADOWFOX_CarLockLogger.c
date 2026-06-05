#ifndef SHADOWFOX_CARLOCK_DISABLE
#define SHADOWFOX_CARLOCK_DISABLE

class SHADOWFOX_CarLockLogger
{
    void Log(string txt)
    {
        SHADOWFOX_CarLockNewLogger.Get().Log(txt);
    }
};

static ref SHADOWFOX_CarLockLogger g_SF_OldLogger;
static SHADOWFOX_CarLockLogger GetSHADOWFOX_CarLockLogger()
{
    if (!g_SF_OldLogger) g_SF_OldLogger = new SHADOWFOX_CarLockLogger;
    return g_SF_OldLogger;
}

#endif
