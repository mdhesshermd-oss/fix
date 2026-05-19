class LBPlacementConfigBase : LBConfigBase {

	static const int CURRENT_VERSION = 1;
	ref array<ref LBPlacementEntry> objects = new array<ref LBPlacementEntry>(); // List of all Objects to spawn

	[NonSerialized()]
	ref array<Object> spawned = new array<Object>();

	void ~LBPlacementConfigBase() {
		DeleteAllObjects();
	}
#ifdef SERVER
	override void OnReceivedFromRPC(PlayerIdentity sender) {
		OnLoad();
	}

	override bool OnLoad() {
		SpawnAllObjects();
		return false;
	}
#endif

	TStringArray GetAvailableClassnames() {
		return null;
	}

	void SpawnAllObjects() {
		DeleteAllObjects();
		spawned = new array<Object>();
		int success = 0;
		foreach (LBPlacementEntry entry : objects) {
			Object obj = entry.SpawnObject();
			spawned.Insert(obj);
			if (obj)
				++success;
		}
		LBLogger.Info("Spawned " + success + " of " + objects.Count() + " Objects in " + this, "Core");
	}

	void DeleteAllObjects() {
		if (g_Game && spawned) {
			foreach (Object obj : spawned) {
				if (obj)
					g_Game.ObjectDelete(obj);
			}
			spawned.Clear();
		}
	}

	void AddObject(LBPlacementEntry entry) {
		objects.Insert(entry);
		spawned.Insert(entry.SpawnObject());
	}

	void RemoveObject(LBPlacementEntry entry) {
		int index = objects.Find(entry);
		if (index == -1)
			return;
		RemoveObject(index);
	}

	void RemoveObject(int index) {
		objects.Remove(index);
		Object spawnedObj = spawned.Get(index);
		if (spawnedObj)
			g_Game.ObjectDelete(spawnedObj);
		spawned.Remove(index);
	}
}