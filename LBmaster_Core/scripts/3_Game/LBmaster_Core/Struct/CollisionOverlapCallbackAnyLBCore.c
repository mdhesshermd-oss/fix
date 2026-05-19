class CollisionOverlapCallbackAnyLBCore : CollisionOverlapCallback {

	bool isBlocked = false;
	ref TStringArray blockingObjects = new TStringArray();

	override bool OnContact(IEntity other, Contact contact) {
		Object obj = Object.Cast(other);
		if (obj && obj.GetShapeName() != "") {
			blockingObjects.Insert(obj.GetType() + " (" + obj + ") " + obj.GetShapeName() + " at " + obj.GetPosition());
			isBlocked = true;
		}
		return true;
	}

}