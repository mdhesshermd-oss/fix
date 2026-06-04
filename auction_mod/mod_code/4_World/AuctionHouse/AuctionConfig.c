class AuctionConfig
{
    string API_URL = "http://localhost:8000";
    string API_KEY = "super-secret-key";
    bool UseExpansionCurrency = true;

    void Save()
    {
        if (!FileExist("$profile:AuctionHouse"))
            MakeDirectory("$profile:AuctionHouse");
        JsonFileLoader<AuctionConfig>.JsonSaveFile("$profile:AuctionHouse/config.json", this);
    }

    static ref AuctionConfig Load()
    {
        ref AuctionConfig settings = new AuctionConfig();
        if (FileExist("$profile:AuctionHouse/config.json"))
            JsonFileLoader<AuctionConfig>.JsonLoadFile("$profile:AuctionHouse/config.json", settings);
        else
            settings.Save();
        return settings;
    }
}
