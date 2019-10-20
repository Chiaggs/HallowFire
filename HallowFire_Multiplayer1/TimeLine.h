#include <time.h>
#include <SFML/Graphics.hpp>
#include <stdlib.h>
// class to maintain global and game time
// Implemented composition in the class by instantiating an object of clock
class timeLine {
public:
	sf::Clock clock;
	sf::Time gameTime;
	sf::Time GlobalTime;
	float ticSize;
	sf::Time timeTic;
	timeLine() {
		ticSize = 1;
		gameTime = clock.getElapsedTime();
		timeTic = sf::seconds(ticSize);
	}
	float getGameTime() {
		return clock.getElapsedTime().asSeconds();
	}
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
