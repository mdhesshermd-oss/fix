class HoverLootManager {

	static float currentLootProgress = 0.0;
	static ref map<EntityAI, int> lootQueueLocationsTypes = new map<EntityAI, int>();
	static ref map<EntityAI, ref InventoryLocation> lootQueueLocations = new map<EntityAI, ref InventoryLocation>();
	static ref array<EntityAI> lootQueue = new array<EntityAI>();
	static ref array<EntityAI> vicinityEntities = new array<EntityAI>();
	static ref array<CargoBase> vicinityCargos = new array<CargoBase>();
	static ref array<ref Param2<EntityAI, float>> laterCombine = new array<ref Param2<EntityAI, float>>();
	static EntityAI currentProcessed = null;

	static void OnUpdateStatic(float timeslice) {
		if (!g_Game.GetUIManager() || !g_Game.GetUIManager().GetMenu() || g_Game.GetUIManager().GetMenu().GetID() != MENU_INVENTORY) {
			laterCombine.Clear();
			return;
		}
		if (GetUApi() && GetUApi().GetInputByName("UALBMHoverLoot").LocalValue() > 0.5) {
			OnHover();
		}
		for (int i = 0; i < laterCombine.Count(); ++i) {
			EntityAI ent = laterCombine.Get(i).param1;
			float quantity = laterCombine.Get(i).param2;
			if (!ent) {
				laterCombine.Remove(i);
				--i;
				continue;
			}
			if (ent.GetQuantity() != quantity) {
				laterCombine.Remove(i);
				--i;
				QueueItem(ent);
				continue;
			}
		}
		ProcessLootQueue(timeslice);
	}

	private static void ProcessLootQueue(float timeslice) {
		if (lootQueue.Count() == 0) {
			currentLootProgress = 0;
			currentProcessed = null;
			return;
		}
		currentLootProgress += timeslice;
		EntityAI looting = lootQueue.Get(0);
		currentProcessed = looting;
		if (currentLootProgress < HoverLootConfig.Get.lootTimerSeconds)
			return;
		currentLootProgress -= HoverLootConfig.Get.lootTimerSeconds;
		lootQueue.RemoveOrdered(0);
		InventoryLocation oldLoc = lootQueueLocations.Get(looting);
		int type = lootQueueLocationsTypes.Get(looting);
		lootQueueLocations.Remove(looting);
		lootQueueLocationsTypes.Remove(looting);
		InventoryLocation loc = new InventoryLocation();
		if (!looting || !oldLoc || !looting.GetInventory() || !looting.GetInventory().GetCurrentInventoryLocation(loc))
			return;
#ifdef DIAG
		LBLogger.Debug("Transfering " + looting + " from " + loc.DumpToString() + " original " + oldLoc.DumpToString(), "HoverLoot");
#endif
		if (!loc.CompareLocationOnly(oldLoc))
			return;
		InventoryLocationType locType = loc.GetType();
#ifdef DIAG
		LBLogger.Debug("Type: " + EnumTools.EnumToString(InventoryLocationType, locType) + " (" + locType + ")", "HoverLoot");
#endif
		EntityAI parent = loc.GetParent();
		if (!parent || !parent.GetHierarchyRootPlayer()) {
			if (type != 1 || !MoveItemToGroundCargo(looting, loc)) {
#ifdef DIAG
				LBLogger.Debug("Moving " + looting + " from ground to inventory", "HoverLoot");
#endif
				if (looting.IsSplitable()) {
					array<EntityAI> items = new array<EntityAI>();
					g_Game.GetPlayer().GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
					foreach (EntityAI item : items) {
						if (item.GetType() == looting.GetType()) {
							if (item.CanBeCombined(looting, true, true)) {
								item.CombineItemsClient(looting, true);
								return;
							} else if (!item.IsMagazine() && item.CanBeCombined(looting, true, false)) {
								laterCombine.Insert(new Param2<EntityAI, float>(looting, looting.GetQuantity()));
								item.CombineItemsClient(looting, true);
								return;
							}
						}
					}
				}
				g_Game.GetPlayer().PredictiveTakeEntityToInventory(FindInventoryLocationType.ANY, looting);
			}
		} else if (locType == InventoryLocationType.CARGO) {
			if (type == 0 && MoveItemToGroundCargo(looting, loc)) {
				return;
			}
#ifdef DIAG
			LBLogger.Debug("Moving " + looting + " to Ground", "HoverLoot");
#endif
			g_Game.GetPlayer().PredictiveDropEntity(looting);
		}
	}

	private static bool MoveItemToGroundCargo(EntityAI looting, InventoryLocation from) {
		foreach (EntityAI vicinityItem : vicinityEntities) {
			InventoryLocation free = new InventoryLocation();
			if (vicinityItem && !Man.Cast(vicinityItem) && vicinityItem.GetInventory() && vicinityItem.IsInventoryVisible() && vicinityItem.IsInventoryVisible() && !vicinityItem.DisableVicinityIcon() && vicinityItem.GetInventory().FindFreeLocationFor(looting, FindInventoryLocationType.ANY_CARGO, free)) {
#ifdef DIAG
				LBLogger.Debug("Moving " + looting + " to Cargo of " + vicinityItem, "HoverLoot");
#endif
				g_Game.GetPlayer().PredictiveTakeToDst(from, free);
				return true;
			}
		}
		return false;
	}

	private static void OnHover() {
		ItemPreviewWidget preview = FindPreviewItemWidgetUnderCursor();
		QueuePreviewItem(preview);
	}

	private static void QueuePreviewItem(ItemPreviewWidget preview) {
		if (!preview)
			return;
		EntityAI item = preview.GetItem();
		if (!IsUnderMouse(preview))
			return;
		QueueItem(item);
		LBLayoutManager.Get().CreateLayout("HoverLoot", preview);
	}

	private static void QueueItem(EntityAI item) {
		if (!item || lootQueue.Find(item) != -1)
			return;
		InventoryLocation loc = new InventoryLocation();
		if (!item.GetInventory() || !item.GetInventory().GetCurrentInventoryLocation(loc))
			return;
		if (loc.GetType() != InventoryLocationType.GROUND && loc.GetType() != InventoryLocationType.CARGO)
			return;
		lootQueue.Insert(item);
		int type = 0;
		if (KeyState(KeyCode.KC_LSHIFT) || KeyState(KeyCode.KC_RSHIFT))
			type = 1;
		lootQueueLocations.Insert(item, loc);
		lootQueueLocationsTypes.Insert(item, type);
	}

	private static ItemPreviewWidget FindPreviewItemWidgetUnderCursor() {
		Widget w = GetWidgetUnderCursor();
		string hierarchy = "";
		ItemPreviewWidget preview = null;
		while (w) {
			if (!preview) {
				Widget found = null;
				for (int i = -1; i <= 8; ++i) {
					if (i == -1)
						found = w.FindAnyWidget("Render");
					else
						found = w.FindAnyWidget("Render" + i);
					if (found) {
						preview = ItemPreviewWidget.Cast(found);
						break;
					}
				}
			}
			hierarchy = hierarchy + w.ClassName() + "(" + w.GetName() + "," + w.GetUserID() + ") ";
			w = w.GetParent();
		}
		return preview;
	}

	private static bool IsUnderMouse(Widget w) {
		int mouseX, mouseY;
		GetMousePos(mouseX, mouseY);
		float wX, wY, wW, wH;
		w.GetScreenSize(wW, wH);
		w.GetScreenPos(wX, wY);

		return mouseX >= wX && mouseX <= wX + wW && mouseY >= wY && mouseY <= wY + wH;
	}

}