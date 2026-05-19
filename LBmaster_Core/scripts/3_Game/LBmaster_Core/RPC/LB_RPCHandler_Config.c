class LB_RPCHandler_Config {

	ref array<ref ScriptCaller> registeredRpcs = new array<ref ScriptCaller>();

	void AddCaller(ScriptCaller function) {
		registeredRpcs.Insert(function);
	}

	void Invoke() {
		foreach (ScriptCaller caller : registeredRpcs) {
			caller.Invoke();
		}
	}

}