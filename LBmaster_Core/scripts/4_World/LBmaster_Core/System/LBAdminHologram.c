class LBAdminHologram {

	static ref LBAdminHologram g_LBAdminHologram;

	private EntityAI obj;
	private float rotation = 0, height = 0;
	private vector positionOffset, orientationOffset, originalPos, originalDir;
	private bool surfaceAlign;
	private Widget infoWidget;
	private TextWidget txt_mouse, txt_lalt_mouse, txt_lcontrol;

	static LBAdminHologram StartPlacement(string type, float initialRotation, bool surfaceAlign_, vector posOff = "0 0 0", vector orientOff = "0 0 0") {
		StopPlacement();
		g_LBAdminHologram = new LBAdminHologram(type, surfaceAlign_, posOff, orientOff);
		g_LBAdminHologram.rotation = initialRotation;
		return g_LBAdminHologram;
	}

	static void StopPlacement() {
		g_LBAdminHologram = null;
	}

	void LBAdminHologram(string type, bool surfaceAlign_, vector posOff = "0 0 0", vector orientOff = "0 0 0") {
		surfaceAlign = surfaceAlign_;
		positionOffset = posOff;
		orientationOffset = orientOff;
		obj = EntityAI.Cast(g_Game.CreateObjectEx(type, vector.Zero, ECE_LOCAL));
		if (!surfaceAlign_) {
			vector bbox = obj.GetBoundingCenter();
			positionOffset[1] = positionOffset[1] + bbox[1];
		}
		g_Game.GetUpdateQueue(CALL_CATEGORY_SYSTEM).Insert(OnFrame);
		TStringArray hiddenSelections = new TStringArray();
		obj.ConfigGetTextArray("hiddenSelections", hiddenSelections);
		for (int i = 0; i < hiddenSelections.Count(); ++i) {
			obj.SetObjectTexture(i, "#(rgb,8,8,3)color(0.5,0.5,0.5,0.7)");
		}
		infoWidget = LBLayoutManager.Get().CreateLayout("HologramPlacementInfo");
		if (infoWidget) {
			TextWidget itemnameInfo = TextWidget.Cast(infoWidget.FindAnyWidget("txt_item"));
			if (itemnameInfo)
				itemnameInfo.SetText(type);
			txt_mouse = TextWidget.Cast(infoWidget.FindAnyWidget("txt_mouse"));
			txt_lalt_mouse = TextWidget.Cast(infoWidget.FindAnyWidget("txt_lalt_mouse"));
			txt_lcontrol = TextWidget.Cast(infoWidget.FindAnyWidget("txt_lcontrol"));
			if (!surfaceAlign_ && txt_lcontrol)
				txt_lcontrol.Show(false);
			UpdateInfoColors();
		}
	}

	void ~LBAdminHologram() {
		if (g_Game) {
			if (obj)
				g_Game.ObjectDelete(obj);
			g_Game.GetUpdateQueue(CALL_CATEGORY_SYSTEM).Remove(OnFrame);
			if (infoWidget)
				infoWidget.Unlink();
		}
	}

	void UpdateInfoColors() {
		if (txt_lalt_mouse) {
			if (surfaceAlign) {
				txt_lalt_mouse.SetColor(ARGB(255, 0, 255, 0));
			} else {
				txt_lalt_mouse.SetColor(-1);
			}
		}
		if (txt_lcontrol && txt_mouse) {
			if (g_Game.LBIsKeyDown(KeyCode.KC_LCONTROL)) {
				txt_lcontrol.SetColor(ARGB(255, 0, 255, 0));
				txt_mouse.SetColor(-1);
			} else {
				txt_mouse.SetColor(ARGB(255, 0, 255, 0));
				txt_lcontrol.SetColor(-1);
			}
		}
	}

	string GetType() {
		return obj.GetType();
	}

	vector GetPosition() {
		return originalPos;
	}

	vector GetOrientation() {
		return originalDir;
	}

	void OnFrame(float timeslice) {
		if (!obj)
			return;
		UpdateInfoColors();
		vector camPos = g_Game.GetCurrentCameraPosition();
		vector endPos = camPos + (g_Game.GetCurrentCameraDirection() * 10);
		vector contactPos, contactDir;
		float frac;
		Object hitObj;
		PhxInteractionLayers layers = PhxInteractionLayers.ITEM_SMALL | PhxInteractionLayers.ITEM_LARGE | PhxInteractionLayers.VEHICLE_NOTERRAIN | PhxInteractionLayers.BUILDING | PhxInteractionLayers.CHARACTER | PhxInteractionLayers.VEHICLE | PhxInteractionLayers.ROADWAY | PhxInteractionLayers.FIREGEOM | PhxInteractionLayers.DOOR | PhxInteractionLayers.WATERLAYER | PhxInteractionLayers.TERRAIN | PhxInteractionLayers.FENCE | PhxInteractionLayers.AI;
		DayZPhysics.RayCastBullet(camPos, endPos, layers, g_Game.GetPlayer(), hitObj, contactPos, contactDir, frac);

		if (GetUApi().GetInputByID(UANextAction).LocalRelease()) {
			if (!g_Game.LBIsKeyDown(KeyCode.KC_LCONTROL)) {
				rotation -= 15;
			} else {
				height -= 0.01;
			}
		}

		if (GetUApi().GetInputByID(UAPrevAction).LocalRelease()) {
			if (!g_Game.LBIsKeyDown(KeyCode.KC_LCONTROL)) {
				rotation += 15;
			} else {
				height += 0.01;
			}
		}
		if (g_Game.LBIsKeyPressed(KeyCode.KC_LMENU))
			surfaceAlign = !surfaceAlign;
		vector camRotation = g_Game.GetCurrentCameraDirection().VectorToAngles();
		vector orient = Vector(rotation + camRotation[0], 0, 0);
		vector orientation = AlignToSurface(contactDir, orient + orientationOffset);
		obj.SetOrientation(orientation);
		vector mat[3];
		Math3D.YawPitchRollMatrix(orientation, mat);
		vector posOff = (positionOffset + Vector(0, height, 0)).Multiply3(mat);
		obj.SetPosition(contactPos + posOff);
		if (surfaceAlign)
			originalPos = contactPos + Vector(0, height, 0);
		else
			originalPos = contactPos + posOff + Vector(0, height, 0);
		originalDir = orientation - orientationOffset;

		if (IsRestrictedFromAdvancedPlacing()) {
			StopPlacement();
			return;
		}
	}

	vector AlignToSurface(vector normal, vector orient) {
		if (!surfaceAlign || normal.LengthSq() < 0.1)
			return orient;
		return AlignToNormal(normal, orient);
	}

	static vector AlignToNormal(vector normal, vector orient) {
		vector mat0[3];
		vector mat1[3];
		vector mat2[3];

		vector angles = normal.VectorToAngles();
		angles[1] = angles[1] + 270;

		angles = angles.GetRelAngles();
		if (angles[1] == 180)
			angles[1] = 0;

		orient[0] = orient[0] + (360 - angles[0]);

		Math3D.YawPitchRollMatrix(orient, mat0);
		Math3D.YawPitchRollMatrix(angles, mat1);
		Math3D.MatrixMultiply3(mat1, mat0, mat2);

		return Math3D.MatrixToAngles(mat2);
	}

	bool IsRestrictedFromAdvancedPlacing() {
		PlayerBase player = PlayerBase.Cast(g_Game.GetPlayer());
		if (!player)
			return true;
		if (player.IsJumpInProgress())
			return true;
		if (player.IsSwimming())
			return true;
		if (player.IsClimbingLadder())
			return true;
		if (player.IsRaised())
			return true;
		if (player.IsClimbing())
			return true;
		if (player.IsRestrained())
			return true;
		if (player.IsUnconscious())
			return true;
		if (!player.IsAlive())
			return true;

		return false;
	}

	static bool IsPlacing() {
		return g_LBAdminHologram != null;
	}

}