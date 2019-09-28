#include <SFML/Graphics.hpp>
class Character : public sf::CircleShape {
public:
	// Variables
	int radius;
	int leftpos, toppos;

	// Constructor
	Character() {
		radius = 25;
		this->setRadius(radius);
		this->setFillColor(sf::Color::Cyan);
		this->leftpos = 75;
		this->toppos = 510;
		this->setPosition(leftpos, toppos);
	}

	//Public interface functions
	void processKeyboardInput(float time) {
		float speed = 400;
		float distance = speed * time;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
			// Code to make the character jump
			this->move(0, -(1.3*distance));
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
			this->move(-distance, 0);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
			this->move(distance, 0);
		}
	}

	void processGravity(float time) {
		float speed = 400;
		float distance = speed * time;
		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
			if (this->getPosition().y <= this->toppos - 5)
				this->move(0, distance);
		}
	}
};