modded class HandsHeader {

	PlayerBase customPlayer = null;

	void SetCustomPlayer(PlayerBase player) {
		customPlayer = player;

	}

	override void UpdateInterval() {
		if (customPlayer) {
			EntityAI item = customPlayer.GetHumanInventory().GetEntityInHands();
			m_ItemHeader.Show(item != null);
			return;
		}
		super.UpdateInterval();
	}

}