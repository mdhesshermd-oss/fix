class HoverLootProgress extends ScriptedWidgetEventHandler {

	protected Widget m_Parent;
	protected Widget m_Root;
	protected Widget m_RadialWidget;
	protected ImageWidget m_LoaderImage;
	protected ref RadialProgressBar m_Radial;

	void HoverLootProgress() {
		m_RadialWidget = null;
		m_LoaderImage = null;
		m_Radial = null;

		g_Game.GetUpdateQueue(CALL_CATEGORY_GUI).Insert(Update);
	}

	void ~HoverLootProgress() {
		if (g_Game)
			g_Game.GetUpdateQueue(CALL_CATEGORY_GUI).Remove(Update);
	}

	protected void OnWidgetScriptInit(Widget w) {
		m_Root = w;
		m_Parent = w.GetParent();

		m_RadialWidget = m_Root.FindAnyWidget("PBRadial1");
		m_LoaderImage = ImageWidget.Cast(m_Root.FindAnyWidget("cap_init_loader"));
		m_LoaderImage.SetRotation(0, 0, 0);
		if (m_RadialWidget)
			m_RadialWidget.GetScript(m_Radial);
		m_Root.Update();
		m_Parent.SetColor(HoverLootConfig.Get.backgroundColor.GetColorARGB());
	}

	protected void Update() {
		if (HoverLootManager.lootQueue.Count() == 0 || !g_Game.GetUIManager() || !g_Game.GetUIManager().GetMenu() || g_Game.GetUIManager().GetMenu().GetID() != MENU_INVENTORY) {
			HoverLootManager.lootQueue.Clear();
			HoverLootManager.lootQueueLocations.Clear();
			m_Parent.Unlink();
			return;
		}
		EntityAI item = GetItem();
		if (!item) {
			m_Parent.Unlink();
			return;
		}
		m_Parent.Show(HoverLootManager.lootQueue.Find(item) != -1);
		if (item != HoverLootManager.currentProcessed) {
			SetProgress(0);
			return;
		}
		float progress = Math.Max(0.0, HoverLootManager.currentLootProgress / HoverLootConfig.Get.lootTimerSeconds);
		SetProgress(Math.AbsFloat(progress * 100));
		if (progress >= 1.0) {
			m_Parent.Unlink();
		}
	}

	EntityAI GetItem() {
		return ItemPreviewWidget.Cast(m_Root.GetParent().GetParent()).GetItem();
	}

	private void SetProgress(float progress) {
		if (m_Radial)
			m_Radial.SetProgress(progress);
	}
}