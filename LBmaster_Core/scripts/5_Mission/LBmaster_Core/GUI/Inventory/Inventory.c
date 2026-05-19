modded class Inventory {

	void LBSetPlayer(PlayerBase player) {
		m_HandsArea.LBGetHandsContainer().LBGetHandsPreview().SetCustomPlayer(player);
		m_RightArea.SetCustomPlayer(player);
	}

	PlayerPreview LBGetPlayerPreview() {
		return m_PlayerPreview;
	}

	void LBResetPlayer() {
		m_HandsArea.LBGetHandsContainer().LBGetHandsPreview().ResetCustomPlayer();
		m_RightArea.SetCustomPlayer(PlayerBase.Cast(g_Game.GetPlayer()));
	}

}

modded class RightArea {

	void SetCustomPlayer(PlayerBase player) {
		GetPlayerContainer().SetPlayer(player);
	}

}