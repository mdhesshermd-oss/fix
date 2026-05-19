modded class HandsContainer {

	HandsPreview LBGetHandsPreview() {
		return m_HandsPreview;
	}

	override void ElectricityIcon() {
		if (g_Game.GetPlayer())
			super.ElectricityIcon();
	}

}