class TierMapReader {

	ref TIntArray flags;
	ref TStringArray tierGroups;
	int flagSize, worldSize;
	int bytesPerCoord = 0;

	void TierMapReader() {
#ifdef PLATFORM_LINUX
		return;
#endif
		LoadDefinitions();
		LoadAreaFlags();
	}

	void LoadDefinitions() {
		tierGroups = new TStringArray();
		string file = "$mission:cfglimitsdefinition.xml";
		if (!FileExist(file)) {
			LBLogger.Error("Could not find " + file, "Core");
			return;
		}
		FileHandle handle = OpenFile(file, FileMode.READ);
		if (handle == 0) {
			LBLogger.Error("Could not open " + file, "Core");
			return;
		}
		string content;
		ReadFile(handle, content, 10000000);
		CloseFile(handle);
		TStringArray lines = new TStringArray();
		content.Split("\n", lines);
		bool readingValues = false;
		foreach (string line : lines) {
			if (line.Contains("<valueflags>")) {
				readingValues = true;
				continue;
			} else if (line.Contains("</valueflags>")) {
				readingValues = false;
				break;
			} else if (readingValues) {
				if (line.Contains("value")) {
					string name = ReadXmlValue(line, "name");
					LBLogger.Debug("Found Value Flag: " + name, "Core");
					tierGroups.Insert(name);
				}
			}
		}
	}

	static string ReadXmlValue(string line, string value) {
		int index = line.IndexOf(value + "=");
		if (index == -1)
			index = line.IndexOf(value + " =");
		if (index == -1)
			return "";
		int start = line.IndexOfFrom(index, "\"");
		int end = line.IndexOfFrom(start + 1, "\"");
		if (start == -1 || end == -1)
			return "";
		int len = end - start - 1;
		return line.Substring(start + 1, len);
	}

	void LoadAreaFlags() {
		flags = new TIntArray();
		FileHandle handle = OpenFile("$mission:areaflags.map", FileMode.READ);
		if (handle == 0) {
			LBLogger.Debug("Could not open areaflags.map", "Core");
			return;
		}
		int header[6];
		int total = ReadFile(handle, header, 24);
		int flagsBits = header[4];
		int sizeSq = header[0] * header[0];
		int skip = flagsBits / 8 * sizeSq;
		flagSize = header[0];
		worldSize = header[2];
		LBLogger.Debug("Read: " + total + " bytes. Size: " + flagSize + " World Size: " + worldSize + " Skipping next " + skip + " bytes of usage flags", "Core");
		int content[10000];
		while (skip > 0) {
			int count = 40000;
			if (skip < count)
				count = skip;
			int read = ReadFile(handle, content, count);
			if (read == 0)
				break;
			total += read;
			skip -= read;
		}
		LBLogger.Debug("Reading Rest of the file...", "Core");
		flags.Reserve(sizeSq / 4);
		int reserved = 1;
		while (true) {
			int more = ReadFile(handle, content, 40000);
			if (more <= 0)
				break;
			total += more;
			int moreInts = more / 4;
			if (flags.Count() + moreInts > reserved * sizeSq) {
				++reserved;
				flags.Reserve(reserved * sizeSq / 4);
				LBLogger.Debug("Extended Flag Array Size", "Core");
			}
			for (int i = 0; i < moreInts; ++i) {
				flags.Insert(content[i]);
			}
		}
		CloseFile(handle);
		if (sizeSq != 0)
			bytesPerCoord = flags.Count() * 4 / sizeSq;
		LBLogger.Debug("Total Size: " + total + " Read Flag Bytes: " + flags.Count(), "Core");
	}

	int GetFlagsAt(vector pos) {
#ifdef PLATFORM_LINUX
		return 0;
#endif
		float x = pos[0];
		float z = pos[2];
		int flagX = x * flagSize / worldSize;
		int flagZ = z * flagSize / worldSize;
		int output = 0;
		for (int i = 0; i < bytesPerCoord; ++i) {
			int offset = flagZ * bytesPerCoord * flagSize + flagX * bytesPerCoord + i;
			int index = offset / 4;
			int byte = offset % 4;
			int value = flags.Get(index);
			int flag = (value >> (8 * byte)) & 0xff;
			output |= flag << (8 * i);
			//LBLogger.Debug("Offset for " + pos + " at (" + flagX + "," + flagZ + ") is " + offset + " Reading byte: " + i + " Value: " + value + "  byte: " + byte + " Flag: " + flag + " All Flags: " + output + " Groups: " + FlagsToGroups(output), "Core");
		}
		return output;
	}

	private TStringArray FlagsToGroups(int flagsI) {
		TStringArray groups = new TStringArray();
		for (int i = 0; i < tierGroups.Count(); ++i) {
			if ((flagsI & (1 << i)) != 0) {
				groups.Insert(tierGroups.Get(i));
			}
		}
		return groups;
	}

	TStringArray GetTierGroupsAt(vector pos) {
		int flagsI = GetFlagsAt(pos);
		return FlagsToGroups(flagsI);
	}

}