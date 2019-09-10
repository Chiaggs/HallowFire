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

bool processCharacterMovingPlatformCollision(Character, MovingPlatform);
void resizeView(const sf::RenderWindow&, sf::View&);
void processScaleToggle(bool&);

int main(){

	// Set initial configuration
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8; // setting the anti-aliasing level, to remove jagged lines
	sf::RenderWindow window;
	window.create(sf::VideoMode(800, 600), "HallowFire", sf::Style::Close | sf::Style::Resize, settings); // Optional thrid arg can be used to specify Window style
	window.setVerticalSyncEnabled(true); // Setting VSync to true, to prevent screen tearing
	sf::Texture platformTexture;
	sf::View view(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(800, 600));
	sf::Font font;

	//Defing Entities
	Platform p1, p2, p3;
	Character c1;
	MovingPlatform mp1;
	bool gameOver = false;
	bool scaleToggle = false;
	int score = 0;

	// Initializing entities
	if (platformTexture.loadFromFile("Textures/wooden-texture.jpg")) {
		p1.setTexture(&platformTexture);
		p2.setTexture(&platformTexture);
		p3.setTexture(&platformTexture);
	}
	else {
		cout << "Failure Loading texture";
	}
	if (!font.loadFromFile("Fonts/SegoeUI.ttf"))
	{
		cout << "Unable to load font file";
	}

	//Frame Processing
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event)){
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::Resized)
				if(scaleToggle)
					resizeView(window, view);
		}
		// reset the frame
		view.setCenter(sf::Vector2f(c1.getPosition().x, c1.getPosition().y));
		window.clear(sf::Color::Black);
		window.setView(view);
		processScaleToggle(scaleToggle);

		// Code to draw contents in the frame
		if (!gameOver) {
			score++;
		}
		window.draw(p1);
		window.draw(p2);
		window.draw(p3);
		window.draw(c1);
		window.draw(mp1);
		mp1.processMovement();
		c1.processKeyboardInput();
		c1.processGravity();
		bool collision = processCharacterMovingPlatformCollision(c1, mp1);
		if (collision) {
			cout << "Collision Occured!";
			gameOver = true;
		}

		// end of the current frame
		window.display();
	}
	return 0;
}

bool processCharacterMovingPlatformCollision(Character c, MovingPlatform mp) {
	bool collision = false;
	sf::FloatRect characterBoundingBox = c.getGlobalBounds();
	sf::FloatRect movingPlatformBoundingBox = mp.getGlobalBounds();
	if (characterBoundingBox.intersects(movingPlatformBoundingBox)) {
		return true;
	}
	return collision;
}

bool processCharacterPlatformCollision(Character c, Platform p) {
	bool collision = false;
	return collision;
}

void resizeView(const sf::RenderWindow& window, sf::View& view) {
	float aspectRatio = float(window.getSize().x) / float(window.getSize().y);
	view.setSize(800 * aspectRatio * (6.0 / 8.0), 600);
}

void processScaleToggle(bool& scaleToggle) {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::O)) {
		scaleToggle = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
		scaleToggle = false;
	}
}