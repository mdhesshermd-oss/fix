class LB_WebhookCallback : RestCallback {

	string type, message, webhookName;

	void LB_WebhookCallback(string messageType, string sentMessage, string name) {
		this.type = messageType;
		this.message = sentMessage;
		this.webhookName = name;
	}

	override void OnError(int errorCode) {
		if (errorCode == ERestResultState.EREST_ERROR_APPERROR) {
			return; // Everything fine
		}
		LBLogger.Error("Error while sending the Discord Webhook + " + webhookName + " ! Response Code: " + errorCode + " MessageType: " + type + " JSON: " + message, "Webhook");
	}

	override void OnTimeout() {
		LBLogger.Error("Error while sending the Discord Webhook " + webhookName + " ! Timeout. Message: " + type, "Webhook");
	}

	override void OnSuccess(string data, int dataSize) {
		LBLogger.Debug("Webhook " + webhookName + " was successfully sent. Message: " + type, "Webhook");
	}

	override void OnFileCreated(string fileName, int dataSize) {
		LBLogger.Fatal("A file should not have been created Webhook: " + webhookName + " ! Filename: " + fileName + " Message: " + type, "Webhook");
	}

}