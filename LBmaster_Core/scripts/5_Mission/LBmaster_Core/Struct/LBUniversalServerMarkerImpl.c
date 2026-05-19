class LBUniversalServerMarkerImpl : LBUniversalServerMarker {

	static ref LBUniversalServerMarkerImpl temp_marker_cache;

#ifdef LBmaster_Groups
	ref LBServerMarker advancedGroupsMapMarker;
#endif

#ifdef EXPANSIONMODNAVIGATION
	ref ExpansionMarkerData expansionServerMarker;
	ExpansionMarkerModule expansionMarkerModule;
#endif

#ifdef BASICMAP
	ref BasicMapCircleMarker basicMapCircleMarker;
	ref BasicMapMarker basicMapMarker;

	static ref array<ref BasicMapMarker> basicMapMarkers = new array<ref BasicMapMarker>();
#endif

#ifdef VanillaPPMap
	ref MarkerInfo vppMapMarker;
	static ServerMarkersCache vppMarkerCache;
#endif

	static void Setup() {
#ifdef BASICMAP
		if (m_BasicMapController) {
			m_BasicMapController.SetMarkers("LBmaster", new array<autoptr BasicMapMarker>);
		}
#endif
#ifdef VanillaPPMap
		EnScript.GetClassVar(g_Game.GetMission(), "m_ServerMarkersCache", 0, vppMarkerCache);
#endif
	}

	static void OnPlayerJoin(PlayerIdentity identity) {
#ifdef BASICMAP
		UpdateBasicMapMarkers(identity);
#endif
	}

	void LBUniversalServerMarkerImpl(string name_, vector position_, int color_, string icon_, bool displayMap, bool display3D, float radius_, int radiusColor, bool radiusStriked) {
		this.name = name_;
		this.position = position_;
		this.color = color_;
		this.icon = icon_;
		this.radius = radius_;
		LBLogger.Debug("Adding Temp Server: " + name + " at " + position + " With icon " + icon + " and color " + color + " with a radius of " + radius + "m", "Core");
#ifdef LBmaster_Groups
		CreateAdvancedGroupsMarker(displayMap, display3D, radiusColor, radiusStriked);
		LBLogger.Debug("Created Advanced Groups Temp Marker", "Core");
#endif
#ifdef EXPANSIONMODNAVIGATION
		CreateExpansionMarker(displayMap, display3D, radiusColor, radiusStriked);
		LBLogger.Debug("Created Expansion Map Temp Marker", "Core");
#endif
#ifdef BASICMAP
		CreateBasicMapMarker(displayMap, display3D, radiusColor, radiusStriked);
		LBLogger.Debug("Created Basic Map Temp Marker", "Core");
#endif
#ifdef VanillaPPMap
		CreateVPPMarker(displayMap, display3D, radiusColor, radiusStriked);
		LBLogger.Debug("Created VPP Map Temp Marker", "Core");
#endif
	}

	void ~LBUniversalServerMarkerImpl() {
		LBLogger.Debug("Deleting Temp Server: " + name + " at " + position + " With icon " + icon + " and color " + color + " with a radius of " + radius + "m", "Core");
#ifdef LBmaster_Groups
		RemoveAdvancedGroupsMarker();
		LBLogger.Debug("Deleted Advanced Groups Temp Marker", "Core");
#endif
#ifdef EXPANSIONMODNAVIGATION
		RemoveExpansionMarker();
		LBLogger.Debug("Deleted Expansion Map Temp Marker", "Core");
#endif
#ifdef BASICMAP
		RemoveBasicMapMarker();
		LBLogger.Debug("Deleted Basic Map Temp Marker", "Core");
#endif
#ifdef VanillaPPMap
		RemoveVPPMarker();
		LBLogger.Debug("Deleted VPP Map Temp Marker", "Core");
#endif
	}

	override void SetRadius(float radius_, bool sync = true) {
		this.radius = radius_;
#ifdef LBmaster_Groups
		if (advancedGroupsMapMarker) {
			advancedGroupsMapMarker.SetRadius(radius);
		}
#endif
#ifdef BASICMAP
		if (basicMapCircleMarker) {
			basicMapCircleMarker.SetRadius(radius);
		}
		if (sync)
			UpdateBasicMapMarkers();
#endif
	}

	override void SetName(string name_, bool sync = true) {
		this.name = name_;
#ifdef LBmaster_Groups
		if (advancedGroupsMapMarker) {
			advancedGroupsMapMarker.SetName(name);
		}
#endif
#ifdef EXPANSIONMODNAVIGATION
		if (expansionServerMarker) {
			expansionServerMarker.SetName(name);
			if (sync && GetExpansionSettings().GetMap()) {
				GetExpansionSettings().GetMap().Send(null);
			}
		}
#endif
#ifdef BASICMAP
		if (basicMapMarker) {
			basicMapMarker.SetName(name);
		}
		if (sync)
			UpdateBasicMapMarkers();
#endif
#ifdef VanillaPPMap
		if (vppMapMarker) {
			vppMapMarker.SetName(name);
		}
		if (sync)
			UpdateVPPMapMarkers();
#endif
	}

	override void SetPosition(vector position_, bool sync = true) {
		this.position = position_;
#ifdef LBmaster_Groups
		if (advancedGroupsMapMarker) {
			advancedGroupsMapMarker.SetPosition(position);
		}
#endif
#ifdef EXPANSIONMODNAVIGATION
		if (expansionServerMarker) {
			expansionServerMarker.SetPosition(position);
			if (sync && GetExpansionSettings().GetMap()) {
				GetExpansionSettings().GetMap().Send(null);
			}
		}
#endif
#ifdef BASICMAP
		if (basicMapMarker) {
			basicMapMarker.SetPosition(position);
		}
		if (sync)
			UpdateBasicMapMarkers();
#endif
#ifdef VanillaPPMap
		if (vppMapMarker) {
			vppMapMarker.SetPosition(position);
		}
		if (sync)
			UpdateVPPMapMarkers();
#endif
	}

	override void SetColor(int color_, bool sync = true) {
		this.color = color_;
		int a, r, g, b;
#ifdef LBmaster_Groups
		if (advancedGroupsMapMarker) {
			advancedGroupsMapMarker.SetColor(color);
		}
#endif
#ifdef EXPANSIONMODNAVIGATION
		if (expansionServerMarker) {
			expansionServerMarker.SetColor(color);
			if (sync && GetExpansionSettings().GetMap()) {
				GetExpansionSettings().GetMap().Send(null);
			}
		}
#endif
#ifdef BASICMAP
		if (basicMapMarker) {
			LBConverter.ARGBToComponents(color, a, r, g, b);
			basicMapMarker.SetARGB(a, r, g, b);
		}
		if (sync)
			UpdateBasicMapMarkers();
#endif
#ifdef VanillaPPMap
		if (vppMapMarker) {
			LBConverter.ARGBToComponents(color, a, r, g, b);
			vppMapMarker.SetColor(Vector(r, g, b));
		}
		if (sync)
			UpdateVPPMapMarkers();
#endif
	}

	override void SetIcon(string icon_, bool sync = true) {
		this.icon = icon_;
#ifdef LBmaster_Groups
		if (advancedGroupsMapMarker) {
			advancedGroupsMapMarker.SetIcon(icon);
		}
#endif
#ifdef EXPANSIONMODNAVIGATION
		if (expansionServerMarker) {
			expansionServerMarker.SetIconName(icon);
			if (sync && GetExpansionSettings().GetMap()) {
				GetExpansionSettings().GetMap().Send(null);
			}
		}
#endif
#ifdef BASICMAP
		if (basicMapMarker) {
			basicMapMarker.SetIcon(icon);
		}
		if (sync)
			UpdateBasicMapMarkers();
#endif
#ifdef VanillaPPMap
		if (vppMapMarker) {
			vppMapMarker.SetIconPath(icon);
		}
		if (sync)
			UpdateVPPMapMarkers();
#endif
	}

	// Helper for VPP Map Markers
#ifdef VanillaPPMap
	void CreateVPPMarker(bool displayMap, bool display3D, int radiusColor, bool radiusStriked) {
		int a, r, g, b;
		LBConverter.ARGBToComponents(color, a, r, g, b);
		vppMapMarker = new MarkerInfo(name, icon, Vector(r, g, b), position, true, display3D);
		array<ref MarkerInfo> markers = vppMarkerCache.GetCachedArray();
		markers.Insert(vppMapMarker);
		UpdateVPPMapMarkers();
	}

	void RemoveVPPMarker() {
		array<ref MarkerInfo> markers = vppMarkerCache.GetCachedArray();
		markers.RemoveItem(vppMapMarker);
		UpdateVPPMapMarkers();
	}

	void UpdateVPPMapMarkers() {
		array<ref MarkerInfo> markers = vppMarkerCache.GetCachedArray();
		GetRPCManager().SendRPC("RPC_MissionGameplay", "SessionMarkers", new Param1<ref array<ref MarkerInfo>>(markers), true, null);
	}
#endif

	// Helper for Basic Map
#ifdef BASICMAP
	void CreateBasicMapMarker(bool displayMap, bool display3D, int radiusColor, bool radiusStriked) {
		int a, r, g, b;
		LBConverter.ARGBToComponents(color, a, r, g, b);
		if (radius > 0) {
			basicMapCircleMarker = new BasicMapCircleMarker(name, position, icon, {r, g, b}, a, display3D);
			basicMapCircleMarker.SetShowCenterMarker(true);
			basicMapCircleMarker.SetRadius(radius);
			basicMapMarker = basicMapCircleMarker;
		} else {
			basicMapMarker = new BasicMapMarker(name, position, icon, {r, g, b}, a, display3D);
		}
		basicMapMarker.SetCanEdit(false);
		basicMapMarker.SetHideOnMap(!displayMap);
		basicMapMarker.SetHideOnPanel(!displayMap);
		basicMapMarkers.Insert(basicMapMarker);
		UpdateBasicMapMarkers();
	}

	void RemoveBasicMapMarker() {
		basicMapMarkers.RemoveItem(basicMapMarker);
		UpdateBasicMapMarkers();
	}

	static void UpdateBasicMapMarkers(PlayerIdentity player = null) {
		BasicMap().SetMarkersRemote("LBmaster", basicMapMarkers, player);
	}
#endif

	// Helper for Expansion Navigation Markers
#ifdef EXPANSIONMODNAVIGATION
	void CreateExpansionMarker(bool displayMap, bool display3D, int radiusColor, bool radiusStriked) {
		if (!CF_Modules<ExpansionMarkerModule>.Get(expansionMarkerModule))
			return;
		expansionServerMarker = new ExpansionServerMarkerData(name + Math.RandomInt(0, int.MAX), false);
		expansionServerMarker.SetName(name);
		expansionServerMarker.SetIconName(icon);
		expansionServerMarker.SetPosition(position);
		expansionServerMarker.SetColor(color);
		expansionServerMarker.Set3D(display3D);
		expansionServerMarker.ClearVisibility();
		if (displayMap)
			expansionServerMarker.SetVisibility(EXPANSION_MARKER_VIS_MAP);
		if (display3D)
			expansionServerMarker.SetVisibility(EXPANSION_MARKER_VIS_WORLD);
		if (GetExpansionSettings().GetMap())
			GetExpansionSettings().GetMap().AddServerMarker(expansionServerMarker);
		if (radius > 0) {
			// Expansion Map does not support radius
		}
	}

	void RemoveExpansionMarker() {
		if (expansionServerMarker && expansionMarkerModule)
			expansionMarkerModule.RemoveServerMarker(expansionServerMarker.GetUID());
	}
#endif

	// Helper for Advanced Groups Markers
#ifdef LBmaster_Groups
	void CreateAdvancedGroupsMarker(bool displayMap, bool display3D, int radiusColor, bool radiusStriked) {
		advancedGroupsMapMarker = LBStaticMarkerManager.Get.AddTempServerMarker(name, position, icon, color, false, display3D, displayMap, displayMap);
		if (radius > 0) {
			advancedGroupsMapMarker.SetRadius(radius, radiusColor, radiusStriked, true, -1);
		}
	}

	void RemoveAdvancedGroupsMarker() {
		if (advancedGroupsMapMarker && LBStaticMarkerManager.Get)
			LBStaticMarkerManager.Get.RemoveServerMarker(advancedGroupsMapMarker);
	}
#endif

}