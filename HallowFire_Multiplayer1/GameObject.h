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
			this->move(0, -(1.3 * distance));
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