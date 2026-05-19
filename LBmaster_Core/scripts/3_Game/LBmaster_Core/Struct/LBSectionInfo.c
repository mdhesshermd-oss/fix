class LBSectionInfo {

	bool newSection = true;
	int indentPixels = 10;
	bool opened = true;

	void LBSectionInfo(bool newSection_, int indent_, bool opened_) {
		this.newSection = newSection_;
		this.indentPixels = indent_;
		this.opened = opened_;
	}

}