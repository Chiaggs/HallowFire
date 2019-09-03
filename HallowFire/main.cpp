#include <SFML/Graphics.hpp>
#include <iostream>

using namespace std;

class Platform : public sf::RectangleShape {
public:
	// Variables
	static int objectCount;
	int length;
	int breadth;

	// Constrcutor
	Platform() {
		objectCount++;
		length = 200;
		breadth = 40;
		this->setSize(sf::Vector2f(length, breadth));
		this->setFillColor(sf::Color::White);
		setInitialPosition();
	}

	// Helper Functions
	void setInitialPosition() {
		this->setPosition(calculateHorizontalPosition(), 560.f);
	}
	int calculateHorizontalPosition() {
		int buffer = 50;
		int pos = buffer;
		for (int i = 1; i < objectCount; i++) {
			pos += length + buffer;
		}
		return pos;
	}

	// public interface functions
	int getObjectCount() {
		return objectCount;
	}
};
int Platform::objectCount = 0;

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
	void processKeyboardInput() {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
			// Code to make the character jump
			this->move(0, -5);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
			this->move(-3, 0);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
			this->move(3, 0);
		}
	}

	void processGravity() {
		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
			if(this->getPosition().y <= this->toppos-5)
				this->move(0, 5);
		}
	}
};

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
	void processMovement() {
		int leftpos = this->getPosition().x;
		if (leftpos <= 0)
			towardsLeft = false;
		if (leftpos >= 720)
			towardsLeft = true;
		if(towardsLeft)
			this->move(-5.f, 0.f);
		else
			this->move(5.f, 0.f);
	}
};

int main(){

	// Set initial configuration
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8; // setting the anti-aliasing level, to remove jagged lines
	sf::RenderWindow window;
	window.create(sf::VideoMode(800, 600), "HallowFire", sf::Style::Default, settings); // Optional thrid arg can be used to specify Window style
	window.setVerticalSyncEnabled(true); // Setting VSync to true, to prevent screen tearing

	//Defing Entities
	Platform p1, p2, p3;
	Character c1;
	MovingPlatform mp1;

	//Frame Processing
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event)){
			if (event.type == sf::Event::Closed)
				window.close();
		}
		// reset the frame
		window.clear(sf::Color::Black);

		// Code to draw contents in the frame
		window.draw(p1);
		window.draw(p2);
		window.draw(p3);
		window.draw(c1);
		window.draw(mp1);
		mp1.processMovement();
		c1.processKeyboardInput();
		c1.processGravity();

		// end of the current frame
		window.display();
	}

	return 0;
}