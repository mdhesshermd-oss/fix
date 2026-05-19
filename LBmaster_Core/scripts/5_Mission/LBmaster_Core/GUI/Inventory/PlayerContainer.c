modded class PlayerContainer {

	override void SetPlayer(PlayerBase player) {
		array<EntityAI> shown = new array<EntityAI>();
		foreach (EntityAI ent, Container con : m_ShowedItems) {
			if (con) {
				(Container.Cast(con.m_Parent)).Remove(con);
			}
		}
		m_ShowedItems.Clear();
		RecomputeOpenedContainers();
		GetMainWidget().Update();
		Refresh();
		UpdateSelectionIcons();
		super.SetPlayer(player);
	}

}