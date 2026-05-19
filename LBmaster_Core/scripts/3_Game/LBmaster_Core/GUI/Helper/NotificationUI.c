modded class NotificationUI {

	void LBClearNotifications() {
		for (int i = 0; i < m_Notifications.Count(); i++) {
			Widget w = m_Notifications.GetElement(i);
			delete w;
		}
		m_Notifications.Clear();
		UpdateTargetHeight();
	}

}