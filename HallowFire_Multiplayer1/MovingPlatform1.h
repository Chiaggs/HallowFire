#include <SFML/Graphics.hpp>
#include "Renderable.h"

class MovingPlatform : public Renderable {
public:
	// Variables
	int length;
	int breadth;
	bool towardsLeft;

	// Constrcutor
	MovingPlatform() {
		length = 80;
		breadth = 10;
		towardsLeft = true;
		this->rectangle.setSize(sf::Vector2f(length, breadth));
		this->rectangle.setFillColor(sf::Color::Red);
		this->rectangle.setPosition(700.f, 545.f);
	}

	// Public interface functions
	void processMovement(float time) {
		float speed = 500;
		float distance = speed * time;
		//cout << "Distance: " << distance;
		int leftpos = this->rectangle.getPosition().x;
		if (leftpos <= 0)
			towardsLeft = false;
		if (leftpos >= 720)
			towardsLeft = true;
		if (towardsLeft)
			this->rectangle.move(-(distance), 0.f);
		else
			this->rectangle.move(distance, 0.f);
	}
};