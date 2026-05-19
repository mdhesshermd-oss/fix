class ActionPlaceLBAdmin : ActionContinuousBase {

	static string itemName = "";
	static string placementName = "";
	static ref ScriptCaller callback;

	void ActionPlaceATM() {
		m_CommandUID	= 0;
		m_StanceMask	= DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
	}

	override void CreateConditionComponents() {
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTNone;
	}

	override bool HasTarget() {
		return false;
	}

	override bool IsInstant() {
		return true;
	}

	override bool HasProgress() {
		return false;
	}

	override string GetText() {
		return "#lb_admin_place " + itemName;
	}

	override bool HasAlternativeInterrupt() {
		return true;
	}

	override bool IsDeploymentAction() {
		return true;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item) {
		//Action not allowed if player has broken legs
		if (player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
			return false;

		//Client
		if (!g_Game.IsDedicatedServer()) {
			return LBAdminHologram.IsPlacing();
		}
		//Server
		return true;
	}

	override void OnStartClient(ActionData action_data) {
		vector position = LBAdminHologram.g_LBAdminHologram.GetPosition();
		vector orientation = LBAdminHologram.g_LBAdminHologram.GetOrientation();
		string itemname = LBAdminHologram.g_LBAdminHologram.GetType();
		LBAdminHologram.StopPlacement();
		if (callback) {
			callback.Invoke(itemname, position, orientation);
		} else {
			g_Game.RPCSingleParam(null, LBmaster_Core_RPCs.ADMIN_PLACE, new Param4<vector, vector, string, string>(position, orientation, itemname, placementName), true);
		}
	}
}