class LB_Webhook_Message {

	string message;
	[NonSerialized()]
	bool valid = true;
	[NonSerialized()]
	ref TStringArray placeholders;

	LB_Webhook_Message Init(string webhookMessage, TStringArray placeholders_ = null) {
		this.message = webhookMessage;
		this.placeholders = placeholders_;
		return this;
	}

	string GetFinalMessage(TStringArray arguments) {
		int count = arguments.Count();
		string finalMessage = "" + message;
		for (int i = 0; i < count; i++) {
			string placeholder = "$$" + (i + 1) + "$$";
			if (placeholders && i < placeholders.Count())
				placeholder = placeholders.Get(i);
			finalMessage.Replace(placeholder, LB_Webhook_Escape.EscapeMessageContent(arguments.Get(i)));
		}
		return finalMessage;
	}

	bool IsValid() {
		return valid;
	}

	bool ValidateLBWebhookMessage(JsonSerializer serializer, string name) {
		LB_Webhook_Empty empty;
		string error = "None";
		string messageCopy = message + "";
		for (int i = 1; i < 15; i++) {
			messageCopy.Replace("$$" + i + "$$", "1");
		}
		if (!serializer.ReadFromString(empty, messageCopy, error)) {
			error.Replace("\n", "");
			error.Replace("\r", "");
			LBLogger.Fatal("Invalid Webhook Json for " + name + ". Error: " + error, "Webhook");
			valid = false;
			return false;
		}
		LBLogger.Debug("Webhook " + name + " is valid", "Webhook");
		valid = true;
		return true;
	}

}