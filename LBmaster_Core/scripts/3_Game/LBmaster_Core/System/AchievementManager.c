class AchievementManager {

	private static ref AchievementManager g_AchievementManager;

	static AchievementManager Get() {
		if (!g_AchievementManager)
			g_AchievementManager = new AchievementManager();
		return g_AchievementManager;
	}

	static void Delete() {
		g_AchievementManager = null;
	}

	private ref array<ref ScriptCaller> titleChangedEvent = new array<ref ScriptCaller>();
	private ref array<ref ScriptCaller> onAchievementGotEvent = new array<ref ScriptCaller>();

	// Public API to trigger achievements (Doesn't do anything unless the LBmaster Achievements mod is loaded !)

	void Trigger(LBPlayer_ player, string triggerName, int cooldown = 0); // Increment a Trigger with this name and save the player afterwards
	void TriggerOnce(LBPlayer_ player, string triggerName, int cooldown = 0); // Only Triggers when trigger never executed. Trigger will only be 0 or 1 after the trigger. If already 1 it's ignored to reduce amount of file writes
	void SetTrigger(LBPlayer_ player, string triggerName, int value, int cooldown = 0); // Set the value of a trigger. This can only increase and never decrease.
	void AddTrigger(LBPlayer_ player, string triggerName, int value, int cooldown = 0); // Add x to the value of a trigger. This can only increase and never decrease.

	void RegisterForAchievementGotEvent(ScriptCaller caller) { // Calls a method with Parameters (PlayerIdentity player, string achievementName) {}
		onAchievementGotEvent.Insert(caller);
	}
	void UnregisterForAchievementGotEvent(ScriptCaller caller) {
		onAchievementGotEvent.RemoveItem(caller);
	}

	void RegisterForTitleChangeEvent(ScriptCaller caller) { // Call a method with Parameters (PlayerIdentity player, string title, int color) {}
		titleChangedEvent.Insert(caller);
	}
	void UnregisterForTitleChangedEvent(ScriptCaller caller) {
		titleChangedEvent.RemoveItem(caller);
	}
	Param2<string, int> GetPlayerTitle(PlayerIdentity player); // Get the current title of the player
}