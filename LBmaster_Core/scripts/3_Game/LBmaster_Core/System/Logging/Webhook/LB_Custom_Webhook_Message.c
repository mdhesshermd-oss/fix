class LB_Custom_Webhook_Message : LB_Webhook_Message {

	string webhookUrl;

	void LB_Custom_Webhook_Message(string url, string content, TStringArray placeholders_ = null) {
		this.webhookUrl = url;
		this.message = content;
		this.placeholders = placeholders_;
		JsonSerializer serial = new JsonSerializer();
		if (!ValidateLBWebhookMessage(serial, "Dynamic")) {
			Error("Failed to validate Webhook");
		}
	}

}