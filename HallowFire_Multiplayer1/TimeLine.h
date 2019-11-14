#include <time.h>
#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <chrono>
// class to maintain global and game time
// Implemented composition in the class by instantiating an object of clock
using namespace std::chrono;
class timeLine {
public:
	sf::Clock clock;
	sf::Time gameTime;
	sf::Time GlobalTime;
	float ticSize;
	sf::Time timeTic;
	long recordedTime;
	timeLine() {
		ticSize = 1;
		gameTime = clock.getElapsedTime();
		timeTic = sf::seconds(ticSize);
		milliseconds ms = duration_cast<milliseconds>(
			system_clock::now().time_since_epoch()
			);
		recordedTime = ms.count();
	}
	float getGameTime() {
		return clock.getElapsedTime().asSeconds();
	}

	void updateGameEngineTime() {
		milliseconds ms = duration_cast<milliseconds>(
			system_clock::now().time_since_epoch()
			);
		recordedTime = ms.count();
		//cout << "Updated Time is: " << recordedTime << endl;
	}

	long getGameEngineTime() {
		return recordedTime;
	}

	// deprecated
	void updateGameTime() {
		if (clock.getElapsedTime().asSeconds() >= ticSize) {
			clock.restart();
			gameTime = gameTime + timeTic;
		}
	}
	float getElapsedTime() {
		return clock.getElapsedTime().asSeconds() * ticSize;
	}
	float restart() {
		return clock.restart().asSeconds() * ticSize;
	}
};
