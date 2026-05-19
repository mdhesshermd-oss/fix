class LBConfigBasePlayer : LBConfigBase {

	const static int CURRENT_VERSION = 0;

	[NonSerialized()]
	string _filename;
	[NonSerialized()]
	bool saveLater;

	void InitIdentity(PlayerIdentity identity) {}

}