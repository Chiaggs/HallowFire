int GID = 0;
class GameObject {
public:
	int ID = 0;
	GameObject() {
		ID = GID;
		GID++;
	}
	int getObjectID() {
		return ID;
	}
};