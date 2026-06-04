class AuctionServerModule
{
    void AuctionServerModule()
    {
        GetRPCManager().AddRPC("AuctionHouse", "RPC_SellRequest", this, SingleplayerExecutionType.Both);
        GetRPCManager().AddRPC("AuctionHouse", "RPC_BuyRequest", this, SingleplayerExecutionType.Both);
        GetRPCManager().AddRPC("AuctionHouse", "RPC_FetchItems", this, SingleplayerExecutionType.Both);
        GetRPCManager().AddRPC("AuctionHouse", "RPC_CheckClaims", this, SingleplayerExecutionType.Both);
    }

    void RPC_FetchItems(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type == CallType.Server)
            AuctionRestManager.GetInstance().GetMarketItems(this, "OnItemsFetched");
    }

    void OnItemsFetched(RestCallback cb)
    {
        string data;
        cb.GetBody(data);
        GetRPCManager().SendRPC("AuctionHouse", "RPC_ReceiveItems", new Param1<string>(data), true);
    }

    void RPC_SellRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        Param4<string, int, int, EntityAI> data;
        if (!ctx.Read(data)) return;

        if (type == CallType.Server && sender)
        {
            AuctionRestManager.GetInstance().SellItem(data.param1, sender.GetId(), data.param2, data.param3, this, "OnSellCompleted");
            if (data.param4) data.param4.Delete();
        }
    }

    void OnSellCompleted(RestCallback cb)
    {
        // Payout and notification logic
    }

    void RPC_BuyRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        Param3<string, int, int> data;
        if (!ctx.Read(data)) return;

        if (type == CallType.Server && sender)
        {
            PlayerBase player = PlayerBase.Cast(sender.GetPlayer());
            int totalCost = data.param2 * data.param3;

            #ifdef EXPANSIONMODMARKET
            if (ExpansionMoneyModule.GetBalance(player) >= totalCost)
            {
                ExpansionMoneyModule.RemoveMoney(player, totalCost);
                AuctionRestManager.GetInstance().BuyItem(data.param1, sender.GetId(), data.param2, data.param3, this, "OnBuyCompleted");
            }
            #endif
        }
    }

    void OnBuyCompleted(RestCallback cb)
    {
        // Notification logic
    }

    void RPC_CheckClaims(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type == CallType.Server && sender)
        {
            AuctionRestManager.GetInstance().GetClaims(sender.GetId(), this, "OnClaimsFetched");
        }
    }

    void OnClaimsFetched(RestCallback cb)
    {
        // DELIVERY LOGIC:
        // 1. Parse JSON for claims (item_class, quantity, claim_id)
        // 2. For each claim:
        //    EntityAI item = EntityAI.Cast(GetGame().CreateObjectEx(itemClass, player.GetPosition(), ECE_PLACE_ON_SURFACE));
        //    AuctionRestManager.GetInstance().CollectClaim(claimId, this, "OnClaimCollected");
    }
}
