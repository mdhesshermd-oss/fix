class LBSpawnLoadoutEntry {

	string itemname; // Itemname of the entity
	InventoryLocationType location; // Location type (Ground, Attachment, Cargo, Hands)
	int slotId; // SlotId when the item is attached
	int row; // Row Index when item is in cargo
	int column; // Column Index when item is in cargo
	int idx; // Index of the Item when item is in cargo
	bool flipped; // Keep Track if the item is flipped in cargo
	ref array<ref LBSpawnLoadoutEntry> children = new array<ref LBSpawnLoadoutEntry>(); // All children of this Item (Attachments and Cargo items) with the same structure

	void Spawn(EntityAI parent) {
		EntityAI me;
		InventoryLocation loc = new InventoryLocation();
		if (location == InventoryLocationType.ATTACHMENT) {
			loc.SetAttachment(parent, null, slotId);
		} else if (location == InventoryLocationType.CARGO || location == InventoryLocationType.PROXYCARGO) {
			loc.SetCargo(parent, null, idx, row, column, flipped);
		} else if (location == InventoryLocationType.HANDS) {
			loc.SetHands(parent, null);
		}
		me = GameInventory.LocationCreateLocalEntity(loc, itemname, ECE_NONE, RF_NONE);
		SpawnChildren(me);
	}

	void SpawnChildren(EntityAI me) {
		if (me) {
			foreach (LBSpawnLoadoutEntry child : children) {
				child.Spawn(me);
			}
		}
	}

	static LBSpawnLoadoutEntry SaveEntry(EntityAI root) {
		LBSpawnLoadoutEntry entry = new LBSpawnLoadoutEntry();
		entry.itemname = root.GetType();
		InventoryLocation loc = new InventoryLocation();
		root.GetInventory().GetCurrentInventoryLocation(loc);
		entry.location = loc.GetType();
		entry.slotId = loc.GetSlot();
		entry.row = loc.GetRow();
		entry.column = loc.GetCol();
		entry.idx = loc.GetIdx();
		entry.flipped = loc.GetFlip();
		int attCount = root.GetInventory().AttachmentCount();
		for (int i = 0; i < attCount; ++i) {
			EntityAI att = root.GetInventory().GetAttachmentFromIndex(i);
			entry.children.Insert(SaveEntry(att));
		}
		CargoBase cargo = root.GetInventory().GetCargo();
		if (cargo) {
			int count = cargo.GetItemCount();
			for (i = 0; i < count; ++i) {
				EntityAI cargoItem = cargo.GetItem(i);
				entry.children.Insert(SaveEntry(cargoItem));
			}
		}
		HumanInventory inv = HumanInventory.Cast(root.GetInventory());
		if (inv && inv.GetEntityInHands()) {
			entry.children.Insert(SaveEntry(inv.GetEntityInHands()));
		}
		return entry;
	}

}