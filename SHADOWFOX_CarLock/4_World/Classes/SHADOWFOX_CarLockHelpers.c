#ifndef SHADOWFOX_CARLOCK
#define SHADOWFOX_CARLOCK

class SHADOWFOX_CarLockTargetHelper
{
    static CarScript GetTargetCar(ActionTarget target)
    {
        CarScript car;
        if (Class.CastTo(car, target.GetObject())) return car;
        if (Class.CastTo(car, target.GetParent())) return car;
        return null;
    }
}

class SHADOWFOX_CarLockHelpers
{
    static PlayerBase GetPlayerBySteamID(string steamID)
    {
        array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);
        foreach (Man man : players)
        {
            PlayerBase player = PlayerBase.Cast(man);
            if (player && player.GetIdentity() && player.GetIdentity().GetPlainId() == steamID)
                return player;
        }
        return null;
    }

    static bool CanPlayerUseLock(PlayerBase player, CarScript car)
    {
        if (!player || !car) return false;
        if (car.m_SF_OwnerId == -1) return false;

        // Owner check
        if (car.m_SF_OwnerId == player.m_SF_LowUid) return true;

        // Admin check
        if (GetGame().IsServer() && player.GetIdentity())
        {
            auto config = GetSHADOWFOX_CarLockStandaloneConfig();
            foreach (string admin : config.Admins)
            {
                if (admin == player.GetIdentity().GetPlainId()) return true;
            }
        }
        else if (player.m_SF_IsAdmin) return true;

        // On the client, we can check the local password storage
        if (GetGame().IsClient())
        {
            SHADOWFOX_CarLockData data = SHADOWFOX_CarLockData.Load(player.m_SF_LowUid.ToString());
            for (int i = 0; i < data.Passwords.Count(); i++)
            {
                if (data.Owners[i] == car.m_SF_OwnerId && data.Passwords[i] == car.m_SF_Password)
                    return true;
            }
        }

        return false;
    }

    static string GetCurrentDateTimeString()
    {
        int year, month, day, hour, minute, second;
        GetYearMonthDay(year, month, day);
        GetHourMinuteSecond(hour, minute, second);

        string s_month = month.ToString(); if (month < 10) s_month = "0" + s_month;
        string s_day = day.ToString(); if (day < 10) s_day = "0" + s_day;
        string s_hour = hour.ToString(); if (hour < 10) s_hour = "0" + s_hour;
        string s_min = minute.ToString(); if (minute < 10) s_min = "0" + s_min;
        string s_sec = second.ToString(); if (second < 10) s_sec = "0" + s_sec;

        return year.ToString() + "-" + s_month + "-" + s_day + " " + s_hour + ":" + s_min + ":" + s_sec;
    }
}

#endif
