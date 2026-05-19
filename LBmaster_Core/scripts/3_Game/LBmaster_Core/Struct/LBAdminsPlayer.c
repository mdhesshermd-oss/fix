class LBAdminsPlayer {

	string steamid = "STEAMID"; // The steamid of this admin. Remeber that there can only be one admin per `{... steamid ...}`. Each other Admin will need their own entry with steamid, comment, ...
	string ingameNameForPermissions = ""; // if set to `""` the Admin can join with any name to receive their admin permissions. If set to any value like `"Peter"`, the Player needs to join with the ingame name Peter to have admin permissions, otherwise he will be treated as a normal player
	string comment = "Comment"; // Comment field to remember which admin it is, because Steamids are hard to recognize
	bool grantAllPermissions = false; // Set to 1 to give the Admin all available permissions regardless of their permissionGroups
	ref TStringSet permissionGroups = new TStringSet(); // Permission Groups the admin is associated with. An Admin can be in multiple groups at the same time. There are 4 default groups, but you can add them yourself by adding a new entry in the `groups` list below. The name of that group must exactly match the entry in the list

	[NonSerialized()]
	ref LBAdminsGroup grantedPermissions = new LBAdminsGroup();

	void InitGrantedPermissions(array<ref LBAdminsGroup> groups) {
		grantedPermissions = new LBAdminsGroup();
		foreach (LBAdminsGroup permissions : groups) {
			if (permissionGroups.Find(permissions.name) == -1)
				continue;
			foreach (string permission, int level : permissions.permissions) {
				grantedPermissions.Insert(permission, level);
			}
		}
	}

	void ToExpected() {
		steamid = "{STEAMID}";
	}

	string ToListString(int column) {
		if (column == 0)
			return comment;
		return steamid;
	}

	void WriteToCtx(ParamsWriteContext ctx) {
		ctx.Write(steamid);
		ctx.Write(grantAllPermissions);
		int count = grantedPermissions.Count();
		ctx.Write(count);
		LBLogger.Verbose("Writing " + count + " Granted Permissions", "Core");
		foreach (string permission, int level : grantedPermissions.permissions) {
			ctx.Write(permission);
			ctx.Write(level);
		}
	}

	bool HasPermission(string permission, bool cannotUseGrantAllPermissions = false) {
		if (grantAllPermissions && !cannotUseGrantAllPermissions)
			return true;
		int level;
		return grantedPermissions.permissions.Find(permission, level) && level > 0;
	}

	bool ReadFromCtx(ParamsReadContext ctx) {
		if (!ctx.Read(steamid))
			return false;
		if (!ctx.Read(grantAllPermissions))
			return false;
		int count = 0;
		if (!ctx.Read(count))
			return false;
		LBLogger.Verbose("Trying to read " + count + " Granted Permissions", "Core");
		grantedPermissions.Clear();
		for (int i = 0; i < count; i++) {
			string permission;
			int level;
			if (!ctx.Read(permission))
				return false;
			if (!ctx.Read(level))
				return false;
			grantedPermissions.Insert(permission, level);
		}
		return true;
	}

}
