#include <SFML/Graphics.hpp>

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