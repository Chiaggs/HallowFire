#include <string>
#include <SFML/Graphics.hpp>
using namespace std;

class Game_Event {
public:
	int priority; // on a scale of 10
	long timeStamp;
	string eventType;
	Game_Event(string eventType) {
		if (eventType == "user_input") {
			priority = 5;
		}
		else if (eventType == "character_collision") {
			priority = 4;
		}
		else if (eventType == "character_death") {
			priority = 3;
		}
		else if (eventType == "character_spawn") {
			priority = 2;
		}
		else {
			priority = 0;
		}
		this->eventType = eventType;
		// add mechanism to initialize time
	}
};

class EventManager {
public:

};


