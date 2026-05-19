class LBConfigInfo {

	static ref LBConfigInfo g_LBConfigInfo;

	static LBConfigInfo Get() {
		if (!g_LBConfigInfo)
			g_LBConfigInfo = new LBConfigInfo();
		return g_LBConfigInfo;
	}

	ref map<string, ref LBClassInfo> classes = new map<string, ref LBClassInfo>();
	ref TStringSet checkedComments = null;

	void RegisterClassComment(string classname, string comment) {
		GetClassInfo(classname, comment);
	}

	void RegisterClassVariableComment(string classname, string variable, string comment) {
		LBClassInfo info = GetClassInfo(classname, "");
		info.AddVariableComment(variable, comment);
	}

	void PrintMissingAdminMenu() {
		if (!checkedComments)
			return;
		LBLogger.Info("Checked comments: " + checkedComments.Count(), "Core");
		int total = 0, missing = 0;
		foreach (string classname, LBClassInfo info : classes) {
			LBLogger.Info("Checking " + classname + " with " + info.variableComments.Count() + " Comments", "Core");
			foreach (string varname, string comment : info.variableComments) {
				++total;
				if (checkedComments.Find(classname + "." + varname) == -1) {
					++missing;
					LBLogger.Error("Variable " + classname + "." + varname + " is not editable via admin menu", "Core");
				}
			}
		}
		LBLogger.Info("Checked all comments. Got " + checkedComments.Count() + " of " + total + ". Missing: " + missing, "Core");
		checkedComments = null;
	}

	LBClassInfo GetClassInfo(string classname, string comment) {
		LBClassInfo found;
		if (!classes.Find(classname, found)) {
			found = new LBClassInfo(classname, comment);
			classes.Insert(classname, found);
		} else if (comment != "")
			found.comment = comment;
		return found;
	}

	bool GetClassComment(string classname, out string comment) {
		LBClassInfo found;
		if (!classes.Find(classname, found)) {
			return false;
		}
		comment = found.comment;
		return true;
	}

	bool GetVariableComment(string classname, string varname, out string comment) {
		LBClassInfo found;
		if (!classes.Find(classname, found)) {
			return false;
		}
		if (checkedComments) {
			LBLogger.Debug("Checking comment for " + classname + "." + varname, "Core");
			checkedComments.Insert(classname + "." + varname);
		}
		return found.GetVariableComment(varname, comment);
	}

}