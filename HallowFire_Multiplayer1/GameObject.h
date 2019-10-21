#include <SFML/Graphics.hpp>
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

class Renderable : public GameObject {
public:
	sf::RectangleShape rectangle;
	sf::RectangleShape getRectangleObject() {
		return rectangle;
	}
	int length;
	int breadth;
};

class Movable : public Renderable {
public:
	int max_x_limit, max_y_limit;
	int min_x_limit, min_y_limit;
	Movable() {
		max_x_limit = 0;
		max_y_limit = 0;
		min_x_limit = 0;
		min_y_limit = 0;
	}
};

class MovingPlatform : public Movable {
public:
	// Variables
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
};

class Platform : public Renderable {
public:
	// Variables
	static int objectCount;

	// Constrcutor
	Platform() {
		objectCount++;
		length = 200;
		breadth = 40;
		this->rectangle.setSize(sf::Vector2f(length, breadth));
		this->rectangle.setFillColor(sf::Color::White);
		setInitialPosition();
	}

	// Helper Functions
	void setInitialPosition() {
		this->rectangle.setPosition(calculateHorizontalPosition(), 560.f);
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

class Character : public Movable {
public:
	// Variables
	int radius;
	int leftpos, toppos;
	sf::CircleShape circle;

	// Constructor
	Character() {
		radius = 25;
		this->circle.setRadius(radius);
		this->circle.setFillColor(sf::Color::Cyan);
		this->leftpos = 75;
		this->toppos = 510;
		this->circle.setPosition(leftpos, toppos);
	}

	//Public interface functions
	void processKeyboardInput(float time) {
		float speed = 400;
		float distance = speed * time;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
			// Code to make the character jump
			this->circle.move(0, -(1.3 * distance));
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
			this->circle.move(-distance, 0);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
			this->circle.move(distance, 0);
		}
	}

	void processGravity(float time) {
		float speed = 400;
		float distance = speed * time;
		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
			if (this->circle.getPosition().y <= this->toppos - 5)
				this->circle.move(0, distance);
		}
	}
};