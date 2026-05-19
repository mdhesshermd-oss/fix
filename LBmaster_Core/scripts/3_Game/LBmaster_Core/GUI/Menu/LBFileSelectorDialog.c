class LBFileSelectorDialog : LBMenuBase {

	ButtonWidget btn_select, btn_back, btn_home, btn_close;
	TextWidget title;
	EditBoxWidget edit_current_path;
	TextListboxWidget list_files;
	ImageWidget img_preview;

	ref LBMenuDataFileSelector data;

	ref array<LBFileTree> folderStack = new array<LBFileTree>();
	private ref array<LBFileTree> added = new array<LBFileTree>();
	private bool finishedLoading = false;

	override void OnDataReceived() {
		data = LBMenuDataFileSelector.Cast(openParameter);
		if (!data) {
			data = new LBMenuDataFileSelector();
		}
		title.SetText("Loading File list... This can take a while depending on the amount of mods loaded");
		btn_select.Show(false);
		btn_back.Show(false);
		btn_home.Show(false);
		list_files.Show(false);
		img_preview.Show(false);
		LBFileList.Get(ScriptCaller.Create(OnFilesLoaded));
	}

	void OnFilesLoaded() {
		title.SetText(data.title);
		btn_select.Show(true);
		btn_back.Show(true);
		btn_home.Show(true);
		list_files.Show(true);
		img_preview.Show(true);
		btn_select.SetText(data.selectText);
		LBFileList list = LBFileList.Get(null);
		OnEnterFolder(list.GetRoot());
		EnterInitialFolder();
		finishedLoading = true;
	}

	override string GetLayoutName() {
		return "FileSelector";
	}

	override void OnHide() {
		super.OnHide();
		ResetPreview();
	}

	override ButtonWidget GetCloseButton() {
		return btn_close;
	}

	void LoadPath(string path) {
		if (!finishedLoading)
			return;
		LBLogger.Info("Loading Path: " + path, "Core");
		LBFileList list = LBFileList.Get(null);
		if (list.GetRoot().FindFileInFolders(path) || list.LoadByPrefix(path)) {
			EnterFolderByString(path);
		}
		if (path.LastIndexOf("/") == path.Length() - 1 && path.Length() > 0)
			edit_current_path.SetText(edit_current_path.GetText() + "/");
	}

	void EnterInitialFolder() {
		LBLogger.Verbose("Entering initial folder: " + data.initialFolder, "Core");
		EnterFolderByString(data.initialFolder);
	}

	void EnterFolderByString(string folder) {
		TStringArray parts = LBFileTree.SplitFile(folder);
		LBFileTree root = folderStack.Get(0);
		folderStack.Clear();
		folderStack.Insert(root);
		string selectedFile = "";
		foreach (string part : parts) {
			LBFileTree subFolder = root.FindFile(part);
			LBLogger.Verbose("Subfolder for " + part + ": " + subFolder, "Core");
			if (subFolder) {
				if (subFolder.IsFolder()) {
					root = subFolder;
					folderStack.Insert(root);
				} else {
					selectedFile = part;
					break;
				}
			} else {
				break;
			}
		}
		LoadCurrentFolder(selectedFile);
	}

	void OnEnterFolder(LBFileTree folder) {
		if (!folder || !folder.IsFolder())
			return;
		folderStack.Insert(folder);
		LoadCurrentFolder();
	}

	void OnBack() {
		if (folderStack.Count() == 1)
			return;
		folderStack.Remove(folderStack.Count() - 1);
		LoadCurrentFolder();
	}

	void LoadCurrentFolder(string selected = "") {
		LBLogger.Info("Loading current folder: " + selected, "Core");
		ResetPreview();
		added.Clear();
		LBFileTree currentFolder = GetCurrentFolder();
		list_files.ClearItems();
		LBLogger.Debug("Sorting " + currentFolder.children.Count() + " Children...", "Core");
		if (currentFolder.children.Count() < 250)
			currentFolder.SortChildren();
		LBLogger.Debug("Adding Children Folders...", "Core");
		foreach (LBFileTree child : currentFolder.children) {
			if (child.IsFolder() && data.IsInFilter(child)) {
				int col = list_files.AddItem("#lb_co_yes", null, 0);
				list_files.SetItem(col, child.name, null, 1);
				added.Insert(child);
			}
		}
		LBLogger.Debug("Adding Children Files...", "Core");
		foreach (LBFileTree child2 : currentFolder.children) {
			if (!child2.IsFolder() && data.IsInFilter(child2)) {
				col = list_files.AddItem("#lb_co_no", null, 0);
				list_files.SetItem(col, child2.name, null, 1);
				added.Insert(child2);
				if (child2.name == selected) {
					list_files.SelectRow(col);
					UpdatePreview();
				}
			}
		}
		LBLogger.Debug("Loaded files and folders", "Core");
		edit_current_path.SetText(currentFolder.fullPath);
	}

	void OnSelected() {
		LBFileTree selected = GetSelectedFile();
		if (selected) {
			data.OnSelect(selected.fullPath);
			return;
		}
		LBFileTree currentFolder = GetCurrentFolder();
		data.OnSelect(currentFolder.fullPath);
	}

	void ResetPreview() {
		img_preview.Show(false);
	}

	void UpdatePreview() {
		ResetPreview();
		LBFileTree selected = GetSelectedFile();
		if (!selected) {
			return;
		}
		if (IsImageFile(selected.ending)) {
			SetImageFit(img_preview, selected.fullPath);
		}
	}

	static bool IsImageFile(string ending) {
		TStringArray endings = {".paa", ".edds"};
		return endings.Find(ending) != -1;
	}

	LBFileTree GetCurrentFolder() {
		return folderStack.Get(folderStack.Count() - 1);
	}

	LBFileTree GetSelectedFile() {
		int selected = list_files.GetSelectedRow();
		if (selected < 0 || selected >= list_files.GetNumItems())
			return null;
		return added.Get(selected);
	}

	override bool OnChange(Widget w, int x, int y, bool finished) {
		if (super.OnChange(w, x, y, finished))
			return true;
		if (w == edit_current_path) {
			LoadPath(edit_current_path.GetText());
			return true;
		}
		return false;
	}

	override bool OnClick(Widget w, int x, int y, int button) {
		if (super.OnClick(w, x, y, button))
			return true;
		if (w == btn_back) {
			OnBack();
			return true;
		} else if (w == btn_select) {
			OnSelected();
			CloseMe();
			return true;
		} else if (w == btn_home) {
			LBFileTree root = folderStack.Get(0);
			folderStack.Clear();
			OnEnterFolder(root);
		}
		return false;
	}

	override bool OnItemSelected(Widget w, int x, int y, int row, int column,	int oldRow, int oldColumn) {
		if (super.OnItemSelected(w, x, y, row, column, oldRow, oldColumn))
			return true;
		if (w == list_files) {
			UpdatePreview();
		}
		return false;
	}

	override bool OnDoubleClick(Widget w, int x, int y, int button) {
		if (super.OnDoubleClick(w, x, y, button))
			return true;
		if (w == list_files) {
			LBFileTree current = GetSelectedFile();
			if (current && current.folder) {
				OnEnterFolder(current);
			} else if (current) {
				OnSelected();
				CloseMe();
			}
			return true;
		}
		return false;
	}

}