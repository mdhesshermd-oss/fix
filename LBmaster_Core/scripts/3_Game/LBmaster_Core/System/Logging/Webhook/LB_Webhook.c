class LB_Webhook {

	bool enabled = false; // Enable / Disable that messages should be sent to the webhook
	string description; // Description to keep track of the Webhooks (only visible from the config)
	string webhookURL = ""; // URL of the Webhook when copying it from the Discord Channel (https://discord.com/api/webhooks/...)
	ref TStringArray webhookMessages = new TStringArray(); // List of Messages, which should be sent to this webhook. Use the names from below to send only specific messages to have the option for multiple webhooks with different purpose like chat logging and admin logging etc.
	ref TStringArray steamidFilter = new TStringArray(); // If this list contains any entries, only webhooks associated with this steamid will be sent and the rest is ignored
	ref map<string, ref LB_Webhook_Message> overwrittenMessages = new map<string, ref LB_Webhook_Message>(); // overwrite any message for example to not include any information, which should not be visible for public channels. An Entry will have the same structure like one entry in the `webhookMessages` list

	void SendMessage(string type, string message, TStringArray arguments, string steamid) {
		if (!IsSteamidFiltered(steamid)) {
			return;
		}
		if (!enabled) {
			LBLogger.Verbose("Disabled Webhook: " + description, "Webhook");
			return;
		}
		if (webhookMessages.Find(type) == -1) {
			LBLogger.Verbose("Message " + type + " not registered for Webhook: " + description, "Webhook");
			return;
		}
		LB_Webhook_Message overwrittenMessage;
		if (overwrittenMessages && overwrittenMessages.Find(type, overwrittenMessage) && overwrittenMessage && overwrittenMessage.IsValid()) {
			message = overwrittenMessage.GetFinalMessage(arguments);
		}
		RestApi api = GetRestApi();
		RestContext ctx = api.GetRestContext(webhookURL);
		ctx.SetHeader("application/json");
		LBLogger.Verbose("Sending Webhook message: " + message, "Webhook");
		ctx.POST(new LB_WebhookCallback(type, message, description), "", message);
	}

	bool IsSteamidFiltered(string steamid) {
		if (!steamidFilter || steamidFilter.Count() == 0)
			return true;
		return steamidFilter.Find(steamid) != -1;
	}

	void WriteToCtx(ParamsWriteContext ctx) {
		ctx.Write(enabled);
		ctx.Write(description);
		ctx.Write(webhookURL);
		ctx.Write(webhookMessages);
		ctx.Write(steamidFilter);
		int count = overwrittenMessages.Count();
		ctx.Write(count);
		foreach (string name, LB_Webhook_Message message : overwrittenMessages) {
			ctx.Write(name);
			ctx.Write(message);
		}
	}

	bool ReadFromCtx(ParamsReadContext ctx) {
		int count;
		if (!ctx.Read(enabled) || !ctx.Read(description) || !ctx.Read(webhookURL) || !ctx.Read(webhookMessages) || !ctx.Read(steamidFilter) || !ctx.Read(count))
			return false;
		for (int i = 0; i < count; ++i) {
			string name;
			LB_Webhook_Message message;
			if (!ctx.Read(name) || !ctx.Read(message))
				return false;
			overwrittenMessages.Insert(name, message);
		}
		return true;
	}

	bool ValidateWebhookMessages(JsonSerializer serializer) {
		bool ok = true;
		foreach (string name, LB_Webhook_Message message : overwrittenMessages) {
			ok = message.ValidateLBWebhookMessage(serializer, name + "#" + description) && ok;
		}
		if (!steamidFilter)
			steamidFilter = new TStringArray();
		LBLogger.Debug("Checking " + steamidFilter.Count() + " steamids in webhook " + description, "Core");
		for (int i = 0; i < steamidFilter.Count(); ++i) {
			if (steamidFilter.Get(i).Trim().Length() != 17) {
				LBLogger.Debug("Removed Steamid \"" + steamidFilter.Get(i) + "\" from Webhook " + description + " for invalid Steamid", "Core");
				steamidFilter.RemoveOrdered(i);
				--i;
			}
		}
		return ok;
	}

	string ToListString(int column) {
		if (description.Length() > 0)
			return description;
		if (webhookURL.Length() > 0)
			return webhookURL;
		return "NEW";
	}

}
