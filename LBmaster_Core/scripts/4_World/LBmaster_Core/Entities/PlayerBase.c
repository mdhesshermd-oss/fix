modded class PlayerBase {

	void PlayerBase() {
		if (LBPlayerUtils.players)
			LBPlayerUtils.players.Insert(this);
	}

	void ~PlayerBase() {
		if (LBPlayerUtils.players)
			LBPlayerUtils.players.RemoveItem(this);
	}

	static void StartAdminPlacementLB(string itemname, string placementName, int rotation, bool surfaceAlign = false, ScriptCaller callback = null, vector posOff = "0 0 0", vector orientOff = "0 0 0") {
		LBAdminHologram.StartPlacement(itemname, rotation, surfaceAlign, posOff, orientOff);
		ActionPlaceLBAdmin.itemName = itemname;
		ActionPlaceLBAdmin.placementName = placementName;
		ActionPlaceLBAdmin.callback = callback;
	}

	override void SetActions(out TInputActionMap InputActionMap) {
		super.SetActions(InputActionMap);
		AddAction(ActionPlaceLBAdmin, InputActionMap);
	}
}