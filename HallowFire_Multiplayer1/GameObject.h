#include <SFML/Graphics.hpp>
#include <iostream>

using namespace std;
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
	virtual void handleEvent(string eventType) = 0;
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
	// Constrcutor
	MovingPlatform(int length, int breadth) {
		this->length = length; // 80
		this->breadth = breadth; // 10
		this->rectangle.setSize(sf::Vector2f(length, breadth));
		this->rectangle.setFillColor(sf::Color::Red);
	}
	void handleEvent(string eventType) {
		cout << " Character's handleEvent called";
	}
};

class static_objects : public Renderable {
public:
	// Variables
	static int objectCount;

	// public interface functions
	int getObjectCount() {
		return objectCount;
	}
};

class Platform : public static_objects {
public:
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
	void handleEvent(string eventType) {
		cout << " Platform's handleEvent called";
	}
};

class SpawnPoint : public static_objects {
public:
	// constrcutor
	SpawnPoint() {
		length = 50;
		breadth = 50;
		this->rectangle.setSize(sf::Vector2f(length, breadth));
		this->rectangle.setFillColor(sf::Color::Red);
		setInitialPosition();
	}
	void setInitialPosition() {
		this->rectangle.setPosition(50.f, 250.f);
	}
	void handleEvent(string eventType) {
		cout << " SpawnPoints's handleEvent called";
	}
};

class DeathZone : public static_objects {
public:
	// constrcutor
	DeathZone() {
		length = 50;
		breadth = 40;
		this->rectangle.setSize(sf::Vector2f(length, breadth));
		this->rectangle.setFillColor(sf::Color::Red);
		setInitialPosition();
	}
	void setInitialPosition() {
		this->rectangle.setPosition(250.f, 559.f);
	}
	void handleEvent(string eventType) {
		cout << " DeathZone's handleEvent called";
	}
};

class SideBoudary : public static_objects {
public:
	// constrcutor
	SideBoudary() {
		length = 5;
		breadth = 400;
		this->rectangle.setSize(sf::Vector2f(length, breadth));
		this->rectangle.setFillColor(sf::Color::Red);
		setInitialPosition();
	}
	void setInitialPosition() {
		this->rectangle.setPosition(450.f, 160.f);
	}
	void handleEvent(string eventType) {
		cout << " SideBoundary's handleEvent called";
		// bump to chcarcter handle event
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
			if (this->circle.getPosition().y < this->toppos)
				this->circle.move(0, distance);
		}
	}
	void handleEvent(string eventType) {
		if (eventType == "user_input") {
			if (!sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
				if (this->circle.getPosition().y < this->toppos)
					this->circle.move(0, 5);
			}
		}
		if (eventType == "character_death") {
			if (this->circle.getPosition().y < this->toppos)
				this->circle.move(0, 5);
		}
		if (eventType == "character_spawn") {
			if (!sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
				if (this->circle.getPosition().y < this->toppos)
					this->circle.move(0, 5);
			}
		}
	}
};

class ViewManager {
public:
	sf::View view;
	bool isTranslate;
	ViewManager() {
		view = sf::View(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(800, 600));
		isTranslate = false;
	}
	void processTranslation(bool isTranslate, Character c) {
		if (isTranslate) {
			this->view.setCenter(c.circle.getPosition());
		}
	}
	sf::View getView() {
		return this->view;
	}
};