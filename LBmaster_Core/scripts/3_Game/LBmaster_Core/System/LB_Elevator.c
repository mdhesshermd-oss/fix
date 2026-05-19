class LB_Elevator {

	private static ref LB_Elevator g_LB_Elevator;

	static LB_Elevator Get() {
		return g_LB_Elevator;
	}

	static void Set(LB_Elevator instance) {
		g_LB_Elevator = instance;
	}

}
