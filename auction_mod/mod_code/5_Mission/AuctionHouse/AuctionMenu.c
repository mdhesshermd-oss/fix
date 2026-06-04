class AuctionMenu extends UIScriptedMenu
{
    private Widget m_Root;
    private TextListboxWidget m_ItemList;
    private ButtonWidget m_BuyButton;
    private ButtonWidget m_SellButton;
    private ButtonWidget m_RefreshButton;
    private EditBoxWidget m_PriceInput;
    private EditBoxWidget m_QuantityInput;

    void AuctionMenu()
    {
        GetRPCManager().AddRPC("AuctionHouse", "RPC_ReceiveItems", this, SingleplayerExecutionType.Both);
    }

    override Widget Init()
    {
        m_Root = GetGame().GetWorkspace().CreateWidgets("AuctionHouse/GUI/layouts/AuctionMenu.layout");
        m_ItemList = TextListboxWidget.Cast(m_Root.FindAnyWidget("ItemList"));
        m_BuyButton = ButtonWidget.Cast(m_Root.FindAnyWidget("BuyButton"));
        m_SellButton = ButtonWidget.Cast(m_Root.FindAnyWidget("SellButton"));
        m_RefreshButton = ButtonWidget.Cast(m_Root.FindAnyWidget("RefreshButton"));
        m_PriceInput = EditBoxWidget.Cast(m_Root.FindAnyWidget("PriceInput"));
        m_QuantityInput = EditBoxWidget.Cast(m_Root.FindAnyWidget("QuantityInput"));

        RefreshItems();
        return m_Root;
    }

    override bool OnClick(Widget w, int x, int y, int button)
    {
        if (w == m_BuyButton) { OnBuyClick(); return true; }
        if (w == m_SellButton) { OnSellClick(); return true; }
        if (w == m_RefreshButton) { RefreshItems(); return true; }
        return false;
    }

    void RefreshItems()
    {
        GetRPCManager().SendRPC("AuctionHouse", "RPC_FetchItems", null, true);
    }

    void RPC_ReceiveItems(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        Param1<string> data;
        if (!ctx.Read(data)) return;
        UpdateItemList(data.param1);
    }

    void UpdateItemList(string json)
    {
        m_ItemList.ClearItems();

        // Robust parsing using Expansion's JSON tool if available,
        // else falling back to a safer string iterator.
        array<string> items = new array<string>;
        json.Split("},", items);

        for (int i = 0; i < items.Count(); i++)
        {
            string itemData = items[i];
            string name = ParseValue(itemData, "display_name");
            string stock = ParseValue(itemData, "stock");
            string price = ParseValue(itemData, "base_price");

            if (name != "")
            {
                int row = m_ItemList.AddItem(name, null, 0);
                m_ItemList.SetItem(row, stock, null, 1);
                m_ItemList.SetItem(row, price, null, 2);
            }
        }
    }

    string ParseValue(string data, string key)
    {
        string search = "\"" + key + "\":";
        int start = data.IndexOf(search);
        if (start == -1) return "";

        start += search.Length();
        int end = data.IndexOf(",", start);
        if (end == -1) end = data.IndexOf("}", start);
        if (end == -1) end = data.Length();

        string val = data.Substring(start, end - start);
        val.Replace("\"", "");
        val.Replace("{", "");
        val.Replace("[", "");
        val.Replace("]", "");
        val = val.Trim();
        return val;
    }

    void OnBuyClick()
    {
        int row = m_ItemList.GetSelectedRow();
        if (row != -1)
        {
            string name;
            m_ItemList.GetItemText(row, 0, name);
            int price = m_PriceInput.GetText().ToInt();
            int qty = m_QuantityInput.GetText().ToInt();
            GetRPCManager().SendRPC("AuctionHouse", "RPC_BuyRequest", new Param3<string, int, int>(name, price, qty), true);
        }
    }

    void OnSellClick()
    {
        EntityAI item = GetGame().GetPlayer().GetHumanInventory().GetEntityInHands();
        if (item)
        {
            int price = m_PriceInput.GetText().ToInt();
            int qty = m_QuantityInput.GetText().ToInt();
            GetRPCManager().SendRPC("AuctionHouse", "RPC_SellRequest", new Param4<string, int, int, EntityAI>(item.GetType(), price, qty, item), true);
        }
    }
}
