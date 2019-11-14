#include <string>
#include <SFML/Graphics.hpp>
#include "TimeLine.h"
#include <queue>
#include <iostream>

using namespace std;

class Game_Event {
public:
	int priority; // on a scale of 10
	long timeStamp;
	string eventType;
	GameObject *go;
	Game_Event(string eventType, timeLine t, GameObject *go) {
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
		else if (eventType == "playback_start") {
			priority = 6;
		}
		else if (eventType == "playback_stop") {
			priority = 7;
		}
		else if (eventType == "charcter_teleport") {
			priority = 8;
		}
		else if (eventType == "wildcard") {
			priority = 9;
		}
		else {
			priority = 0;
		}
		this->eventType = eventType;
		this->timeStamp = t.getGameEngineTime();
		this->go = go;
	}

	string serialize_event() {
		string output = "";
		return output;
	}
};

bool operator<(const Game_Event& p1, const Game_Event& p2)
{
	return p1.priority < p2.priority;
}

class EventManager {
public:
	priority_queue<Game_Event> event_queue;
	void handlePendingEvents() {
		while (!event_queue.empty()) {
			Game_Event ge = event_queue.top();
			event_queue.pop();
			if (ge.eventType == "character_death") {
				cout << "Chcracter Died ";
			}
			if (ge.eventType == "wildcard") {
				cout << "wildcard";
				ge.go->handleEvent(ge.eventType);
			}
			if (ge.eventType == "charcter_teleport") {
				cout << "charcter_teleport";
				ge.go->handleEvent(ge.eventType);
			}
			if (ge.eventType == "playback_stop") {
				cout << "playback_stop";
				ge.go->handleEvent(ge.eventType);
			}
			if (ge.eventType == "playback_start") {
				cout << "playback_start";
				ge.go->handleEvent(ge.eventType);
			}
			if (ge.eventType == "character_spawn") {
				cout << "character_spawn";
				ge.go->handleEvent(ge.eventType);
			}
			if (ge.eventType == "character_collision") {
				cout << "character_collision";
			}
			if (ge.eventType == "user_input") {
				cout << "user_input";
			}
			//ge.go->handleEvent();
		}
	}
	void raiseEvent(Game_Event ge) {
		event_queue.push(ge);
	}
};


