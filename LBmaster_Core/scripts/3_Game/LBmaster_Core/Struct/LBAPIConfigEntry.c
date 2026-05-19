class LBAPIConfigEntry {

	bool enabled = false; // Enable / Disable an endpoint
	string name = "Example"; // Name of it for logging
	string pollUrl; // URL to poll data from
	string responseUrl; // URL to send back the data
	string apiKey; // Optional API key if the endpoints requires an API key
	bool restrictedAccess = false; // When set to 1 some routes are going to be blocked

	void LBAPIConfigEntry(string pollUrl_, string responseUrl_, string apiKey_ = "") {
		this.pollUrl = pollUrl_;
		this.responseUrl = responseUrl_;
		this.apiKey = apiKey_;
	}

}