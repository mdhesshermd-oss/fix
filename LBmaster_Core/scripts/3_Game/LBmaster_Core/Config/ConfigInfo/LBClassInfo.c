class LBClassInfo {

	string classname;
	string comment;
	ref map<string, string> variableComments = new map<string, string>();

	void LBClassInfo(string classname_, string comment_) {
		this.classname = classname_;
		this.comment = comment_;
	}

	void AddVariableComment(string var, string comment_) {
		variableComments.Insert(var, comment_);
	}

	bool GetVariableComment(string var, out string comment_) {
		return variableComments.Find(var, comment_);
	}

}