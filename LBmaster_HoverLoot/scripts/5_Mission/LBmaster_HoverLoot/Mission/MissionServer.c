modded class MissionServer {

	override void OnInitLB() {
		super.OnInitLB();
		LBAdmins.Get().RegisterPermission("hoverloot.change.config", false, false, "Allows the player to change the main hover loot config");
		LBAdmins.Get().OnRegisterFinished();
	}

}