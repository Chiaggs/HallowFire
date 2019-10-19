#include <SFML/Graphics.hpp>
#include "GameObject.h"

class Renderable : public GameObject {
public:
	sf::RectangleShape rectangle;
	sf::RectangleShape getRectangleObject() {
		return rectangle;
	}
};