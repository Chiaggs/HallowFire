#include <SFML/Graphics.hpp>

class MovingPlatform : public sf::RectangleShape {
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
		this->setSize(sf::Vector2f(length, breadth));
		this->setFillColor(sf::Color::Red);
		this->setPosition(700.f, 545.f);
	}

	// Public interface functions
	void processMovement(float time) {
		float speed = 500;
		float distance = speed * time;
		//cout << "Distance: " << distance;
		int leftpos = this->getPosition().x;
		if (leftpos <= 0)
			towardsLeft = false;
		if (leftpos >= 720)
			towardsLeft = true;
		if (towardsLeft)
			this->move(-(distance), 0.f);
		else
			this->move(distance, 0.f);
	}
};