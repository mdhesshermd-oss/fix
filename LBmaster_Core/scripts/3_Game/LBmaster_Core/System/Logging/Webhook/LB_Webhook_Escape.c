class LB_Webhook_Escape {
	string x;

	static ref LB_Webhook_Escape tmp = new LB_Webhook_Escape();
	static ref JsonSerializer serializer = new JsonSerializer();

	static string EscapeMessageContent(string content) {
		string copy = content + "";
		int replaceCount = copy.Replace("\\", "\\" + "\\");
		tmp.x = copy;
		string escaped;
		serializer.WriteToString(tmp, false, escaped);
		return escaped.Substring(6, escaped.Length() - 8);
	}
}