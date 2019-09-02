#include <SFML/Graphics.hpp>
#include <iostream>

using namespace std;

class Platform {
public:
	// Variables
	static int objectCount;
	sf::RectangleShape rectangle;
	int length;
	int breadth;

	// Constrcutor
	Platform() {
		objectCount++;
		length = 200;
		breadth = 40;
		rectangle.setSize(sf::Vector2f(length, breadth));
		rectangle.setFillColor(sf::Color::White);
		setPosition();
	}

	// Helper Functions
	void setPosition() {
		rectangle.setPosition(calculateHorizontalPosition(), 560.f);
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

class Character {
public:
};

class MovingPlatform {
public:

};

int main()
{
	// Set initial configuration
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8; // setting the anti-aliasing level, to remove jagged lines
	sf::RenderWindow window;
	window.create(sf::VideoMode(800, 600), "HallowFire", sf::Style::Default, settings); // Optional thrid arg can be used to specify Window style
	window.setVerticalSyncEnabled(true); // Setting VSync to true, to prevent screen tearing

	//Defing Entities
	Platform p1, p2, p3;

	//Frame Processing
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		// reset the frame
		window.clear(sf::Color::Black);

		// Code to draw contents in the frame
		window.draw(p1.rectangle);
		window.draw(p2.rectangle);
		window.draw(p3.rectangle);

		// end of the current frame
		window.display();
	}


	return 0;
}