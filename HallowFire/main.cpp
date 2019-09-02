#include <SFML/Graphics.hpp>

int main()
{
	sf::Window window;
	window.create(sf::VideoMode(800, 600), "SFML works!"); // Optional thrid arg can be used to specify Window style
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		window.display();
	}
	return 0;
}