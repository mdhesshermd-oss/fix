modded class HandsPreview {

	PlayerBase customPlayer;

	void SetCustomPlayer(PlayerBase player) {
		if (!customPlayer)
			customPlayer = PlayerBase.Cast(g_Game.GetPlayer());
		if (Inventory.GetInstance())
			DeleteIcon();
		HandsContainer parent = HandsContainer.Cast(m_Parent);
		HandsHeader header = HandsHeader.Cast(parent.GetHeader());
		header.SetName("");
		header.SetCustomPlayer(player);
		customPlayer.GetOnItemAddedToHands().Remove(CreateNewIcon);
		customPlayer.GetOnItemRemovedFromHands().Remove(DeleteIcon);

		customPlayer = player;
		customPlayer.GetOnItemAddedToHands().Insert(CreateNewIcon);
		customPlayer.GetOnItemRemovedFromHands().Insert(DeleteIcon);
		ItemBase hands = ItemBase.Cast(customPlayer.GetHumanInventory().GetEntityInHands());
		if (hands)
			CreateNewIcon(hands);
	}

	void ResetCustomPlayer() {
		if (customPlayer) {
			customPlayer.GetOnItemAddedToHands().Remove(CreateNewIcon);
			customPlayer.GetOnItemRemovedFromHands().Remove(DeleteIcon);
		}
		if (Inventory.GetInstance())
			DeleteIcon();
		customPlayer = null;
		HandsContainer parent = HandsContainer.Cast(m_Parent);
		HandsHeader header = HandsHeader.Cast(parent.GetHeader());
		header.SetName("");
		header.SetCustomPlayer(null);
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		player.GetOnItemAddedToHands().Remove(CreateNewIcon);
		player.GetOnItemAddedToHands().Insert(CreateNewIcon);
		player.GetOnItemRemovedFromHands().Remove(DeleteIcon);
		player.GetOnItemRemovedFromHands().Insert(DeleteIcon);
		ItemBase hands = ItemBase.Cast(player.GetHumanInventory().GetEntityInHands());
		if (hands)
			CreateNewIcon(hands);
	}

}