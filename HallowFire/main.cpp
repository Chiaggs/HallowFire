#include <SFML/Graphics.hpp>

class Platform {
	sf::RectangleShape rectangle;
	Platform() {
		rectangle.setSize(sf::Vector2f(100.f, 20.f));
	}
};

class Character {
};

class MovingPlatform {
};

int main()
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8; // setting the anti-aliasing level, to remove jagged lines
	sf::RenderWindow window;
	window.create(sf::VideoMode(800, 600), "HallowFire", sf::Style::Default, settings); // Optional thrid arg can be used to specify Window style
	window.setVerticalSyncEnabled(true); // Setting VSync to true, to prevent screen tearing
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

		// end of the current frame
		window.display();
	}
	return 0;
}