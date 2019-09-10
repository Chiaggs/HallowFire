#include <SFML/Graphics.hpp>
#include <iostream>
#include "Platform.h"
#include "Character.h"
#include "MovingPlatform.h"

using namespace std;
int Platform::objectCount = 0;
bool processCharacterMovingPlatformCollision(Character, MovingPlatform);
void resizeView(const sf::RenderWindow&, sf::View&);
void processScaleToggle(bool&);
void updateScore(int& score, bool& gameOver);
void updateScoreHUD(sf::Text&, int&, bool&);

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
	sf::Text text;

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
	else {
		text.setFont(font);
	}
	text.setCharacterSize(24);
	text.setFillColor(sf::Color::White);

	//Frame Processing
	while (window.isOpen())
	{
		// Handling events
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
		updateScore(score, gameOver);
		window.draw(p1);
		window.draw(p2);
		window.draw(p3);
		window.draw(c1);
		window.draw(mp1);
		text.setPosition(sf::Vector2f(c1.getPosition().x - 400, c1.getPosition().y - 300));
		updateScoreHUD(text, score, gameOver);
		window.draw(text);
		mp1.processMovement();
		c1.processKeyboardInput();
		c1.processGravity();
		bool collision = processCharacterMovingPlatformCollision(c1, mp1);
		if (collision)
			gameOver = true;

		// end of the current frame
		window.display();
	}
	return 0;
}

// functions to detect collision
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

// functions to perform scaling
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

// functions to implement game logic
void updateScore(int& score, bool& gameOver) {
	if (!gameOver) {
		score++;
	}
}

// functions related to HUD display
void updateScoreHUD(sf::Text& text, int& score, bool& gameOver) {
	string HUDtext = "Score: " + std::to_string(score);
	if (gameOver) {
		HUDtext += " Game Over!";
	}
	text.setString(HUDtext);
}