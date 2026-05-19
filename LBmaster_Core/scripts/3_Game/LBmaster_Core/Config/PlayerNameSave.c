class PlayerNameSave : LBConfigLoaderPlayer<PlayerNameSave_> {

	override void InitVars() {
		InitVarsInternal("Core/Players", "[Steamid].json", LBConfigType.DATA, false, "core.read.players"); // (do not edit)
	}

	override bool KeepBackups() {
		return true;
	}
}
// This file contains some basic info about a player
class PlayerNameSave_ : LBConfigBasePlayer {

	private string steamid; // Steamid of the Player Saved
	private string bohemiaId; // Bohemia Guid
	private string lastName; // Last known name, which is updated on joining
	private string lastGroupTag; // Last known group name
	private bool groupTagVisible = true; // If the group tag should be shown
	private string lastTag; // Last known Chat Tag
	private int lastOnlineTimestamp; // Last time the player joined the server
	private int firstJoinTimestamp; // first time the player joined the server
	private int playSessions = 1; // Keeps track how often a player joined the server

	[NonSerialized()]
	private int lastGroupTagRequested = 0;
	[NonSerialized()]
	private int lastTagRequested = 0;
	[NonSerialized()]
	bool online = false;

	override void InitIdentity(PlayerIdentity identity) {
		this.steamid = identity.GetPlainId();
		this.bohemiaId = identity.GetId();
		this.lastName = identity.GetName();
		this.firstJoinTimestamp = LBDate.Init(true).GetTimestamp();
		PlayerNameSave.Loader.SavePlayer(this);
	}

	void WriteToCtxSimple(ParamsWriteContext ctx) {
		if (steamid.Length() != 17) {
			ctx.Write(0);
		} else {
			int lower = steamid.Substring(0, 8).ToInt();
			int higher = steamid.Substring(8, 9).ToInt();
			ctx.Write(lower);
			ctx.Write(higher);
			ctx.Write(lastName);
		}
	}

	static PlayerNameSave_ ReadFromCtxSimple(ParamsReadContext ctx) {
		int lower, higher;
		if (!ctx.Read(lower) || lower == 0)
			return null;
		string name;
		if (!ctx.Read(higher) || !ctx.Read(name))
			return null;
		PlayerNameSave_ save = new PlayerNameSave_();
		save.lastName = name;
		string higherStr = "" + higher;
		string lowerStr = "" + lower;
		while (lowerStr.Length() < 8)
			lowerStr = "0" + lowerStr;
		while (higherStr.Length() < 9)
			higherStr = "0" + higherStr;
		save.steamid = lowerStr + higherStr;
		return save;
	}

	override bool OnLoad() {
		if (lastGroupTag.Length() > 1 && lastGroupTag[0] == "[" && lastGroupTag[lastGroupTag.Length() - 1] == "]") {
			lastGroupTag = lastGroupTag.Substring(1, lastGroupTag.Length() - 2);
			groupTagVisible = true;
			return true;
		}
		return false;
	}

	void InitOnline(TStringSet onlineSteamids) {
		if (onlineSteamids.Find(steamid) == -1) {
			online = false;
		} else {
			online = true;
		}
	}

	bool IsSearched(string search, array<Widget> extraWidgets) {
		if (extraWidgets && extraWidgets.Count() > 0 && CheckBoxWidget.Cast(extraWidgets.Get(0)) && CheckBoxWidget.Cast(extraWidgets.Get(0)).IsChecked()) {
			// filter offline players
			if (!online)
				return false;
		}
		return LBStringTools.ContainsIgnoreCase(steamid, search) || LBStringTools.ContainsIgnoreCase(lastName, search);
	}

	string ToListString(int column) {
		if (column == 1)
			return steamid;
		if (column == 2)
			return bohemiaId;
		return lastName;
	}

	string GetPlayerGroupTag(bool checkVisible) {
		int now = g_Game.GetTime();
		LBLogger.Verbose("Getting group tag of " + steamid + ": " + checkVisible + " + " + groupTagVisible + " Last Tag: " + lastGroupTag, "Core");
		if (now - lastGroupTagRequested < 100) {
			if (checkVisible && !groupTagVisible)
				return "";
			if (lastGroupTag == "")
				return "";
			return "[" + lastGroupTag + "]";
		}
		lastGroupTag = GetElevatorGame().GetPlayerGroupName(steamid, bohemiaId, groupTagVisible);
		lastGroupTagRequested = g_Game.GetTime();
		if (checkVisible && !groupTagVisible)
			return "";
		if (lastGroupTag == "")
			return "";
		return "[" + lastGroupTag + "]";
	}

	string GetTag() {
		int now = g_Game.GetTime();
		if (now - lastTagRequested < 100)
			return lastTag;
		lastTag = GetElevatorGame().GetPlayerTag(steamid, bohemiaId);
		lastTagRequested = g_Game.GetTime();
		return lastTag;
	}

	bool GetTagColor(inout LBColorConfig cfg) {
		return GetElevatorGame().GetPlayerTagColor(steamid, bohemiaId, cfg);
	}

	bool GetGroupTagColor(inout LBColorConfig cfg) {
		return GetElevatorGame().GetPlayerGroupColor(steamid, bohemiaId, cfg);
	}

	string GetName() {
		return lastName;
	}

	string GetSteamid() {
		return steamid;
	}

	string GetBohemiaId() {
		return bohemiaId;
	}

	int GetLastOnlineTimestamp() {
		return lastOnlineTimestamp;
	}

	int GetFirstJoinTimestamp() {
		return firstJoinTimestamp;
	}

	string GetLastOnlineString() {
		if (online)
			return "Now";
		LBDate date = LBDate.Init(lastOnlineTimestamp);
		return date.ToFormattedString();
	}

	string GetLastOnlineDiffString() {
		if (online)
			return "0s ago";
		int now = LBDate.Init(true).GetTimestamp();
		int diff = now - lastOnlineTimestamp;
		LBDate date = LBDate.Init(diff);
		return date.ToDiffString() + " ago";
	}

	string GetFirstOnlineString() {
		LBDate date = LBDate.Init(firstJoinTimestamp);
		return date.ToFormattedString();
	}

	string GetFirstOnlineDiffString() {
		int now = LBDate.Init(true).GetTimestamp();
		int diff = now - firstJoinTimestamp;
		LBDate date = LBDate.Init(diff);
		return date.ToDiffString() + " ago";
	}

	void UpdateLastOnlineTimestamp() {
		lastOnlineTimestamp = LBDate.Init(true).GetTimestamp();
	}

	int GetPlaySessions() {
		return playSessions;
	}

	void UpdateName(PlayerIdentity ident) {
		++playSessions;
		this.lastName = ident.GetName();
		PlayerNameSave.Loader.SavePlayer(this);
	}

	int GetHash() {
		return GetSteamid().Hash();
	}

}