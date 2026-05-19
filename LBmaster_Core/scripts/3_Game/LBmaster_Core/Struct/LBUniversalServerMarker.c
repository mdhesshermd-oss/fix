class LBUniversalServerMarker {

	// Universal marker interface to add markers for different Map Mods.
	// Always keep a reference to this object. The destructor will remove the server marker again automatically
	// Check the LBUniversalServerMarkerImpl class in 5_Mission for the implementation of this class
	// Use GetElevatorMission().CreateServerMarker(...) to create a new marker anywhere in 3_Game, 4_World or 5_Mission

	// Compatible Mods:
	// LBmaster Advanced Groups
	// Expansion Navigation
	// Basic Map
	// (VPP Map)

	// Be aware, that some mods might not support all parameters
	// Also this implementation is not perfect, because of the way some map mods are structured, this applies in particular to the VPP Map as this is very outdated and hard to properly sync server markers

	string name, icon;
	vector position;
	int color;
	float radius;

	void SetName(string name_, bool sync = true);
	void SetPosition(vector position_, bool sync = true);
	void SetColor(int color_, bool sync = true);
	void SetIcon(string icon_, bool sync = true);
	void SetRadius(float radius_, bool sync = true);

}