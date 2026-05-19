class LBVersionUpdateCheckCallback : RestCallback {

	static bool received = false;
	static string errorMessage = "";
	static ref LBServerUpdateResponse updateResponse = new LBServerUpdateResponse();

	private string ip;
	private static int retryCount = 0;

	void LBVersionUpdateCheckCallback(string ip_) {
		this.ip = ip_;
	}

	override void OnError(int errorCode) {
		errorMessage = "Receive Error";
		RetryRequest("Could not read data from Update Server ! " + ip + " Error: " + errorCode + ": " + EnumTools.EnumToString(ERestResultState, errorCode));
	}

	override void OnTimeout() {
		errorMessage = "#lb_message_update_timeout";
		RetryRequest("Could not read data from Update Server ! " + ip + " Error: Timeout");
	}

	override void OnSuccess(string data, int dataSize) {
		JsonSerializer serial = new JsonSerializer();
		string jsonError;
		if (!serial.ReadFromString(updateResponse, data, jsonError)) {
			errorMessage = "Json Error";
			LBLogger.Error("Could not read data from Update Server ! Error: Invalid Json: " + jsonError, "UpdateCheck");
			Error("[UpdateCheck] LBmaster Update Check could not read data from Update Server ! Error: Invalid Json: " + jsonError);
			return;
		}
		g_Game.lb_new_mods_list = updateResponse.lb_new_mods_list;
		Print("Received a list of " + updateResponse.lb_new_mods_list.Count() + " mods");
		received = true;
		if (updateResponse.oldVersions.Count() == 0) {
			LBLogger.Admin("All Mods are up to date !", "UpdateCheck");
			g_Game.AdminLog("[UpdateCheck] All LBmaster mods are up to date !");
			Print("[UpdateCheck] All LBmaster mods are up to date !");
		} else {
			LBLogger.Error("Some of your Mods are not up to Date !", "UpdateCheck");
			g_Game.AdminLog("[UpdateCheck] Some of your Mods are not up to Date !");
			Print("[UpdateCheck] Some of your Mods are not up to Date !");
			foreach (LBInstalledModInfo oldInfo : updateResponse.oldVersions) {
				LBLogger.Error(oldInfo.message, "UpdateCheck");
				g_Game.AdminLog("[UpdateCheck] " + oldInfo.message);
				Print("[UpdateCheck] " + oldInfo.message);
			}
		}
	}

	override void OnFileCreated(string fileName, int dataSize) {}

	void RetryRequest(string errorMsg) {
		++retryCount;
		if (retryCount > 5) {
			LBLogger.Error(errorMsg, "UpdateCheck");
			Error("[UpdateCheck] LBmaster Update Check " + errorMsg);
		} else {
			LBLogger.Debug("(" + retryCount + "/5): " + errorMsg + " Retrying...", "UpdateCheck");
			ServerUpdateRequestPacket checker = new ServerUpdateRequestPacket();
			checker.Load();
			checker.Send();
		}
	}
}