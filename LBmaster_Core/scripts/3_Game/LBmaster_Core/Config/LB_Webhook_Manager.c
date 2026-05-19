class LB_Webhook_Manager : LBConfigLoader<LB_Webhook_Manager_> {

	override void InitVars() {
		InitVarsInternal("Common", "Webhooks.json", LBConfigType.CONFIG, false); // (medium)
	}

}
/*With this file, you can configure Webhooks for various different events. You can have different webhooks for different events and edit the webhook to look the way you want to

For how to create a Discord Webhook message, please check out the [Discord Developer API](https://discord.com/developers/docs/resources/webhook#execute-webhook) and use a [Json Escape Tool](https://www.freeformatter.com/json-escape.html) to escape the special characters (for example `"` will become `\"`) Also check the predefined webhooks to see how the formatting works

Also use a [Json Validator](https://jsonformatter.curiousconcept.com/) to validate the webhooks.json file and also your webhook to make sure it works properly
*/
class LB_Webhook_Manager_ : LBConfigBase {

	static const int CURRENT_VERSION = 3;

	ref array<ref LB_Webhook> webhooks = new array<ref LB_Webhook>(); // List of all Webhooks
	ref map<string, ref LB_Webhook_Message> webhookMessages = new map<string, ref LB_Webhook_Message>(); // List of the raw Webhook messages with placeholders represented as `$$x$$` where x is a number. See below for information how to setup the message correctly.\nA complete list of all webhooks can be found [Here](/webhook/list.php)
	[NonSerialized()]
	bool needSave = false;

	override void OnReceivedFromRPC(PlayerIdentity sender) {
		OnLoad();
	}

	override bool OnLoad() {
		ValidateWebhookMessages();
		return false;
	}

	override void LoadDefault() {
		LB_Webhook example = new LB_Webhook();
		example.webhookURL = "https://discord.com/api/webhooks/....";
		example.enabled = false;
		example.description = "Test Webhook for demonstration";
		example.webhookMessages.Insert("GroupCreate");
		example.webhookMessages.Insert("GroupDelete");
		example.webhookMessages.Insert("GroupChat");
		example.webhookMessages.Insert("GlobalChat");
		example.webhookMessages.Insert("DirectChat");
		example.webhookMessages.Insert("ATMRobbingSuccess");
		webhooks.Insert(example);

		example = new LB_Webhook();
		example.webhookURL = "https://discord.com/api/webhooks/....";
		example.enabled = false;
		example.description = "Admin Webhook for demonstration";
		example.webhookMessages.Insert("GroupCreate");
		example.webhookMessages.Insert("GroupDelete");
		example.webhookMessages.Insert("GroupChat");
		example.webhookMessages.Insert("GlobalChat");
		example.webhookMessages.Insert("DirectChat");
		example.webhookMessages.Insert("ATMRobbingSuccess");
		example.overwrittenMessages.Insert("ATMRobbingSuccess", (new LB_Webhook_Message()).Init("{}"));
		webhooks.Insert(example);
	}

	override void WriteToCtx(ParamsWriteContext ctx) {
		int count = webhooks.Count();
		ctx.Write(count);
		foreach (LB_Webhook hook : webhooks) {
			hook.WriteToCtx(ctx);
		}
		count = webhookMessages.Count();
		ctx.Write(count);
		foreach (string name, LB_Webhook_Message message: webhookMessages) {
			ctx.Write(name);
			ctx.Write(message);
		}
	}

	override bool ReadFromCtx(ParamsReadContext ctx) {
		array<ref LB_Webhook> webhooksNew = new array<ref LB_Webhook>();
		map<string, ref LB_Webhook_Message> webhookMessagesNew = new map<string, ref LB_Webhook_Message>();
		int count;
		if (!ctx.Read(count))
			return false;
		for (int i = 0; i < count; ++i) {
			LB_Webhook hook = new LB_Webhook();
			if (!hook.ReadFromCtx(ctx))
				return false;
			webhooksNew.Insert(hook);
		}
		if (!ctx.Read(count))
			return false;
		for (i = 0; i < count; ++i) {
			string name;
			LB_Webhook_Message message;
			if (!ctx.Read(name) || !ctx.Read(message))
				return false;
			webhookMessagesNew.Insert(name, message);
		}
		webhooks = webhooksNew;
		webhookMessages = webhookMessagesNew;
		return true;
	}

	override void UpdateVersion() {
		if (version < 2) {
			DeleteWebhook("GlobalChat");
			DeleteWebhook("GroupChat");
			DeleteWebhook("GroupDelete");
			DeleteWebhook("GroupCreate");
			DeleteWebhook("DirectChat");
		}
	}

	void AfterRegister() {
		if (needSave)
			LB_Webhook_Manager.Loader.Save();
	}

	void ValidateWebhookMessages() {
		bool ok = true;
		JsonSerializer serializer = new JsonSerializer();
		foreach (string name, LB_Webhook_Message message : webhookMessages) {
			ok = message.ValidateLBWebhookMessage(serializer, name) && ok;
		}
		foreach (LB_Webhook webhook : webhooks) {
			ok = webhook.ValidateWebhookMessages(serializer) && ok;
		}
		LBLogger.Info("All Webhooks Valid ? " + ok, "Webhook");
	}

	void SendMessage(string type, TStringArray arguments, string steamid = "") {
		LBLogger.Verbose("Sending Webhook " + type + "...", "Webhook");
		LB_Webhook_Message found;
		if (webhookMessages.Find(type, found)) {
			SendMessageDynamic(found, arguments, type, steamid);
		} else {
			LBLogger.Error("Could not find Webhook Message: " + type, "Webhook");
		}
	}

	void SendMessageTo(string url, string type, TStringArray arguments, string steamid = "") {
		LBLogger.Verbose("Sending Webhook " + type + " to custom URL...", "Webhook");
		LB_Webhook_Message found;
		if (webhookMessages.Find(type, found)) {
			SendMessageDynamicTo(url, found, arguments, type, steamid);
		} else {
			LBLogger.Error("Could not find Webhook Message: " + type, "Webhook");
		}
	}

	void SendCustomMessageTo(string url, string message, TStringArray placeholders, TStringArray arguments, string steamid = "") {
		LB_Webhook_Message webhookMessage = new LB_Webhook_Message();
		webhookMessage.Init(message, placeholders);
		SendMessageDynamicTo(url, webhookMessage, arguments, "Dynamic", steamid);
	}

	private void SendMessageDynamicTo(string url, LB_Webhook_Message found, TStringArray arguments, string type, string steamid) {
		if (!found.IsValid())
			return;
		string sendMessage = found.GetFinalMessage(arguments);
		if (GetRestApi() == null)
			CreateRestApi();
		LB_Webhook webhook = new LB_Webhook();
		webhook.webhookURL = url;
		webhook.enabled = true;
		webhook.webhookMessages.Insert(type);
		webhook.SendMessage(type, sendMessage, arguments, steamid);
	}

	private void SendMessageDynamic(LB_Webhook_Message found, TStringArray arguments, string type, string steamid = "") {
		if (!found.IsValid())
			return;
		string sendMessage = found.GetFinalMessage(arguments);
		if (GetRestApi() == null)
			CreateRestApi();
		foreach (LB_Webhook webhook : webhooks) {
			webhook.SendMessage(type, sendMessage, arguments, steamid);
		}
	}

	void SendCustomWebhookMessage(LB_Custom_Webhook_Message message, TStringArray arguments, string steamid = "") {
		LB_Webhook webhook = new LB_Webhook();
		webhook.webhookURL = message.webhookUrl;
		webhook.enabled = true;
		webhook.webhookMessages.Insert("Dynamic");
		string sendMessage = message.GetFinalMessage(arguments);
		if (GetRestApi() == null)
			CreateRestApi();
		webhook.SendMessage("Dynamic", sendMessage, arguments, steamid);
	}

	void RegisterWebhook(string name, LB_Webhook_Message defaultMessage, TStringArray placeholders = null) {
		LB_Webhook_Message registered;
		if (!webhookMessages.Find(name, registered) || !registered) {
			registered = defaultMessage;
			webhookMessages.Insert(name, defaultMessage);
			needSave = true;
		}
		registered.placeholders = placeholders;
	}

	void RegisterWebhook(string name, string message, TStringArray placeholders = null) {
		RegisterWebhook(name, (new LB_Webhook_Message()).Init(message), placeholders);
	}

	private void DeleteWebhook(string name) {
		webhookMessages.Remove(name);
	}

}