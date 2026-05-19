class LBAPIRoute {

	ref LBAPIParserBase handler;
	string name;
	ref map<string, ref LBAPIRoute> subroutes = new map<string, ref LBAPIRoute>();

	LBAPIRoute RegisterSubroute(string name_) {
		LBAPIRoute route;
		if (subroutes.Find(name_, route))
			return route;
		route = new LBAPIRoute();
		route.name = name_;
		subroutes.Insert(name_, route);
		return route;
	}

	void RemoveChild(string name_) {
		subroutes.Remove(name_);
	}

	LBAPIRoute GetRoute(string name_) {
		return subroutes.Get(name_);
	}

}