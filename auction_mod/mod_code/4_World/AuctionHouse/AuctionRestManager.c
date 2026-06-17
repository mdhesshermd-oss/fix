class AuctionRestManager
{
    private static ref AuctionRestManager m_Instance;
    private ref AuctionConfig m_Config;

    void AuctionRestManager()
    {
        m_Config = AuctionConfig.Load();
    }

    static AuctionRestManager GetInstance()
    {
        if (!m_Instance)
            m_Instance = new AuctionRestManager();
        return m_Instance;
    }

    void GetMarketItems(Managed caller, string callback)
    {
        RestContext ctx = GetRestApi().GetRestContext(m_Config.API_URL);
        ctx.SetHeader("x-api-key", m_Config.API_KEY);
        ctx.GET(caller, callback, "/market/items");
    }

    void SellItem(string itemClass, string playerId, int price, int quantity, Managed caller, string callback)
    {
        RestContext ctx = GetRestApi().GetRestContext(m_Config.API_URL);
        ctx.SetHeader("x-api-key", m_Config.API_KEY);
        ctx.SetHeader("Content-Type", "application/json");

        string json = "{\"item_class\":\"" + itemClass + "\",\"player_id\":\"" + playerId + "\",\"price\":" + price + ",\"quantity\":" + quantity + "}";
        ctx.POST(caller, callback, "/market/sell", json);
    }

    void BuyItem(string itemClass, string playerId, int price, int quantity, Managed caller, string callback)
    {
        RestContext ctx = GetRestApi().GetRestContext(m_Config.API_URL);
        ctx.SetHeader("x-api-key", m_Config.API_KEY);
        ctx.SetHeader("Content-Type", "application/json");

        string json = "{\"item_class\":\"" + itemClass + "\",\"player_id\":\"" + playerId + "\",\"price\":" + price + ",\"quantity\":" + quantity + "}";
        ctx.POST(caller, callback, "/market/buy", json);
    }

    void GetClaims(string playerId, Managed caller, string callback)
    {
        RestContext ctx = GetRestApi().GetRestContext(m_Config.API_URL);
        ctx.SetHeader("x-api-key", m_Config.API_KEY);
        ctx.GET(caller, callback, "/market/claims/" + playerId);
    }

    void CollectClaim(string claimId, Managed caller, string callback)
    {
        RestContext ctx = GetRestApi().GetRestContext(m_Config.API_URL);
        ctx.SetHeader("x-api-key", m_Config.API_KEY);
        ctx.POST(caller, callback, "/market/claims/collect/" + claimId, "");
    }
}
