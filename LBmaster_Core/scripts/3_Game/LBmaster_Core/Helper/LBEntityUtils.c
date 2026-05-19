class LBEntityUtils {

	static bool ClearEntityCargo(EntityAI entity) {
		if (!entity)
			return false;
		GameInventory inv = entity.GetInventory();
		if (!inv)
			return false;
		CargoBase cargo = inv.GetCargo();
		if (!cargo)
			return false;
		int count = cargo.GetItemCount();
		for (int i = count - 1; i >= 0; --i) {
			EntityAI ent = cargo.GetItem(i);
			if (ent)
				ent.Delete();
		}
		return true;
	}

	static bool ClearEntityAttachments(EntityAI entity) {
		if (!entity)
			return false;
		GameInventory inv = entity.GetInventory();
		if (!inv)
			return false;
		int attCount = inv.AttachmentCount();
		for (int i = attCount - 1; i >= 0; --i) {
			EntityAI att = inv.GetAttachmentFromIndex(i);
			if (att)
				att.Delete();
		}
		return true;
	}

}