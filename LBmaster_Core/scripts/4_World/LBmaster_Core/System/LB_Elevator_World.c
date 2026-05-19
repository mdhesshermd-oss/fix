class LB_Elevator_World : LB_Elevator_Game {

	override LB_ATM_PlayerbaseBase LoadATMPlayer(Man player) {
		return new LB_ATM_Playerbase(player);
	}
	override string GetPlayerGroupName(string steamid, string bohemiaId, out bool visible) { // Overwrite this method to support other Group Mods
		LBLogger.Verbose("Trying to get Group name of " + steamid + " with Default Finder Implementation", "Core");
#ifdef SERVER
#ifdef LBGROUP_SYSTEM
		visible = false;
		LBGroup grp = LBGroupManager.Get().GetPlayersGroup(steamid);
		if (grp) {
#ifndef LB_DISABLE_CHAT
			if (ChatConfig.Get.forceDisplayGroupTagsInChat || grp.showTagInChat)
				visible = true;
#else
			if (grp.showTagInChat)
				visible = true;
#endif
			return grp.shortname;
		}
		return "";

#endif

#ifdef EXPANSIONMODGROUPS
		ExpansionPartyModule partyModule = null;
		if (!CF_Modules<ExpansionPartyModule>.Get(partyModule))
			return "";
		foreach (int partyId, ExpansionPartyData party : partyModule.GetAllParties()) {
			if (party.IsMember(bohemiaId)) {
				return "[" + party.GetPartyName() + "]";
			}
		}
		return "";
#endif
#endif

		return super.GetPlayerGroupName(steamid, bohemiaId, visible);
	}

	override bool GetPlayerGroupColor(string steamid, string bohemiaId, inout LBColorConfig cfg) {
#ifdef SERVER
#ifdef LBGROUP_SYSTEM
		LBGroup grp = LBGroupManager.Get().GetPlayersGroup(steamid);
		if (grp && grp.hasCustomChatColor) {
			cfg = LBColorConfig.Init(255, grp.chatColorR, grp.chatColorG, grp.chatColorB);
			return true;
		}
		return false;

#endif
#endif

		return super.GetPlayerGroupColor(steamid, bohemiaId, cfg);
	}

}
int lb_elevator_init_world = LB_ElevatorInitWorld();
int LB_ElevatorInitWorld() {
	LB_Elevator.Set(new LB_Elevator_World());
	return 0;
}
LB_Elevator_World GetElevatorWorld() {
	return LB_Elevator_World.Cast(LB_Elevator.Get());
}