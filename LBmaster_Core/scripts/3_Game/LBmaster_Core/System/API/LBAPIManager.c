class LBAPIManager {

	static ref LBAPIRoute rootRoute = new LBAPIRoute();
	static ref array<ref LBAPIEndpointHandler> handlers = new array<ref LBAPIEndpointHandler>();
	static int port = 2302;

	static void Delete() {
		LBLogger.Info("Deleting LBAPIManager...", "Core");
		rootRoute = null;
		handlers = null;
	}

	static void StartHandler(string name, string url, bool restricted, string apiKey, int failedDelay) {
		StartHandler(name, url + "/poll?", url + "/response?", restricted, apiKey, failedDelay);
	}

	static void StartHandler(string name, string poll, string response, bool restricted, string apiKey, int failedDelay) {
#ifdef SERVER
		if (IsCLIParam("port")) {
			string portStr;
			if (GetCLIParam("port", portStr))
				port = portStr.ToInt();

		}
		LBAPIEndpointHandler handler = new LBAPIEndpointHandler(name, poll, response, restricted, apiKey, failedDelay);
		handlers.Insert(handler);
		handler.StartRequest();
#endif
	}

	static void RegisterRoute(string path, LBAPIParserBase handler) {
#ifdef SERVER
		LBAPIRoute route = rootRoute;
		TStringArray parts = new TStringArray();
		path.Split("/", parts);
		foreach (string part : parts) {
			route = route.RegisterSubroute(part);
		}
		LBLogger.Verbose("Regsitered API route " + path + ". Existed: " + (route.handler != null), "Core");
		route.handler = handler;
#endif
	}

	static void UnregisterRoute(string path) {
#ifdef SERVER
		if (path.LastIndexOf("*") == path.Length() - 1) {
			UnregisterRoute(path.Substring(0, path.Length() - 1));
			return;
		}
		RemoveChild(path);
#endif
	}

	static void RemoveChild(string path) {
		if (!rootRoute)
			return;
		TStringArray parts = new TStringArray();
		path.Split("/", parts);
		LBAPIRoute current = rootRoute;
		LBAPIRoute parent = null;
		for (int i = 0; i < parts.Count(); ++i) {
			string part = parts.Get(i);
			if (part == "")
				continue;
			parent = current;
			current = current.GetRoute(part);
			if (!current)
				return;
		}
		if (parent && current)
			parent.RemoveChild(current.name);
	}

	static LBAPIRoute GetRoute(string path) {
		TStringArray parts = new TStringArray();
		path.Split("/", parts);
		LBAPIRoute current = rootRoute;
		for (int i = 0; i < parts.Count(); ++i) {
			string part = parts.Get(i);
			if (part == "")
				continue;
			current = current.GetRoute(part);
			if (!current)
				return null;
		}
		return current;
	}

	static LBAPIResponse OnAPIRequest(string apiName, string path, string content, bool restricted) {
#ifdef SERVER
		LBAPIRoute route = GetRoute(path);
		LBLogger.Info("Received API Request by " + apiName + " to " + path, "Core");
		LBLogger.Debug("Request Content: " + content, "Core");
		if (route) {
			if (route.handler && route.handler.IsValid()) {
				if (restricted && route.handler.restricted) {
					LBLogger.Error("API Enpoint " + apiName + " tried to call restricted endpoint: " + path, "Core");
					return LBAPIResponse.AccessDenied();
				}
				return route.handler.OnRequest(content);
			}
			TStringArray available = new TStringArray();
			foreach (LBAPIRoute child : route.subroutes)
				available.Insert(child.name);
			string availableContent;
			LBAPIParser.m_Serial.WriteToString(available, true, availableContent);
			return LBAPIResponse.NotFound("Handler Invalid: " + route.handler).SetData(availableContent);
		}
#endif
		return LBAPIResponse.NotFound("");
	}

}