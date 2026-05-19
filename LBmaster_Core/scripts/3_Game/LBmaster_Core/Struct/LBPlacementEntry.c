class LBPlacementEntry {

	string itemname; // Classname of the item to spawn
	vector position; // Position of the item to spawn at
	vector orientation; // Orientation of the object
	float scale; // Scale (default 1.0) to make objects smaller or bigger

	void LBPlacementEntry(string itemname_, vector position_, vector orientation_, float scale_ = 1.0) {
		this.itemname = itemname_;
		this.position = position_;
		this.orientation = orientation_;
		this.scale = scale_;
	}

	Object SpawnObject() {
		Object obj = g_Game.CreateObjectEx(itemname, position, ECE_NOPERSISTENCY_WORLD | ECE_CREATEPHYSICS);
		obj.SetPosition(position);
		obj.SetOrientation(orientation);
		if (scale != 1.0 && scale > 0)
			obj.SetScale(scale);
		obj.SetAffectPathgraph(true, false);
		if (obj.CanAffectPathgraph())
			g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(g_Game.UpdatePathgraphRegionByObject, 100, false, obj);
		return obj;
	}

	string ToListString(int column) {
		return itemname + " at " + position;
	}

}