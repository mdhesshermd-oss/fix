#ifndef SHADOWFOX_CARLOCK_DISABLE
#define SHADOWFOX_CARLOCK_DISABLE

class SHADOWFOX_CarLockHelpers
{
    static bool CreateInInventory(PlayerBase player, string classname, int quantity = 1)
    {
        if (!player || classname == "" || quantity <= 0) return false;
        EntityAI entity = player.GetInventory().CreateInInventory(classname);
        if (!entity)
        {
            entity = EntityAI.Cast(GetGame().CreateObject(classname, player.GetPosition(), false, true));
        }
        if (entity)
        {
            ItemBase item = ItemBase.Cast(entity);
            if (item) item.SetQuantity(quantity);
            return true;
        }
        return false;
    }

    static bool CheckAdminBySteamID(string steamID)
    {
        if (steamID == "" || !GetGame().IsServer()) return false;
        auto config = GetSHADOWFOX_CarLockStandaloneConfig();
        if (config && config.Admins)
        {
            foreach (string admin : config.Admins)
            {
                if (admin == steamID) return true;
            }
        }
        return false;
    }

    static bool IsAdmin(PlayerBase player)
    {
        if (!player) return false;
        if (GetGame().IsServer())
        {
            if (!player.GetIdentity()) return false;
            return CheckAdminBySteamID(player.GetIdentity().GetPlainId());
        }
        return player.m_SF_IsAdmin;
    }

    static PlayerBase GetPlayerBySteamID(string steamID)
    {
        if (steamID == "" || !GetGame().IsServer()) return null;
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

    static bool CanPlayerUseLock(PlayerBase player, CarScript vehicle)
    {
        if (!player || !vehicle) return false;
        if (vehicle.m_SF_OwnerId == -1 || vehicle.m_SF_Password == -1) return false;
        if (IsAdmin(player)) return true;
        if (vehicle.m_SF_OwnerId == player.m_SF_LowUid) return true;

        auto config = GetSHADOWFOX_CarLockStandaloneConfig();
        if (config.EnablePasswordSharing)
        {
            if (player.SF_HasPassword(vehicle.m_SF_Password, vehicle.m_SF_OwnerId))
                return true;
        }
        return false;
    }

    static string GetCurrentDateTimeString()
    {
        int year, month, day, hour, minute, second;
        GetYearMonthDay(year, month, day);
        GetHourMinuteSecond(hour, minute, second);
        return year.ToString() + "-" + month.ToStringLen(2) + "-" + day.ToStringLen(2) + " " + hour.ToStringLen(2) + ":" + minute.ToStringLen(2) + ":" + second.ToStringLen(2);
    }
};

static PlayerBase CLGetPlayerByIdentity(PlayerIdentity identity)
{
    if (!identity) return null;
    array<Man> players = new array<Man>;
    GetGame().GetPlayers(players);
    foreach (Man man : players)
    {
        PlayerBase player = PlayerBase.Cast(man);
        if (player && player.GetIdentity() && player.GetIdentity().GetId() == identity.GetId())
            return player;
    }
    return null;
}

#endif
