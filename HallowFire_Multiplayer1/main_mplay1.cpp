#include <SFML/Graphics.hpp>
#include <iostream>
#include "Platform1.h"
#include "Character1.h"
#include "MovingPlatform1.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <zmq.hpp>
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
#define sleep(n)    Sleep(n)
#endif

using namespace std;
int Platform::objectCount = 0;
bool processCharacterMovingPlatformCollision(Character, MovingPlatform);
void resizeView(const sf::RenderWindow&, sf::View&);
void processScaleToggle(bool&);
void updateScore(float& score, bool& gameOver, float);
void updateScoreHUD(sf::Text&, float&, bool&);
void pause_unpause(int&, bool&);

// class to maintain global and game time
// Implemented composition in the class by instantiating an object of clock
class timeLine {
public:
	sf::Clock clock;
	sf::Time gameTime;
	sf::Time GlobalTime;
	float ticSize;
	sf::Time timeTic;
	timeLine() {
		ticSize = 1;
		gameTime = clock.getElapsedTime();
		timeTic = sf::seconds(ticSize);
	}
	void printGameTime() {
		cout << clock.getElapsedTime().asSeconds() << " \n";
	}
	void updateGameTime() {
		if (clock.getElapsedTime().asSeconds() >= ticSize) {
			clock.restart();
			gameTime = gameTime + timeTic;
		}
	}
	float getElapsedTime() {
		return clock.getElapsedTime().asSeconds() * ticSize;
	}
	float restart() {
		return clock.restart().asSeconds() * ticSize;
	}
};

void adjustTicSize(timeLine&);

int main() {

	// Set initial configuration
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8; // setting the anti-aliasing level, to remove jagged lines
	sf::RenderWindow window;
	window.create(sf::VideoMode(800, 600), "HallowFire", sf::Style::Close | sf::Style::Resize, settings); // Optional thrid arg can be used to specify Window style
	//window.setVerticalSyncEnabled(true); // Setting VSync to true, to prevent screen tearing
	sf::Texture platformTexture;
	sf::View view(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(800, 600));
	sf::Font font;
	sf::Text text;
	float elapsedTime = 0;

	//Defing Entities
	Platform p1, p2, p3;
	Character c1;
	MovingPlatform mp1;
	timeLine t1;
	bool gameOver = false;
	bool scaleToggle = false;
	float score = 0;
	int pauseTicker = 0;
	bool isPaused = false;
	bool takeInput = true;

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

	// Socket Programming
	//  Prepare our context and socket
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REQ);
	srand(time(0));
	int ClientID = rand();
	socket.connect("tcp://localhost:5555");

	//Frame Processing
	while (window.isOpen())
	{
		// Handling events
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::Resized)
				if (scaleToggle)
					resizeView(window, view);
			if (event.type == sf::Event::GainedFocus)
				takeInput = true;
			if (event.type == sf::Event::LostFocus)
				takeInput = false;
		}

		// reset the frame
		view.setCenter(sf::Vector2f(c1.getPosition().x, c1.getPosition().y));
		window.clear(sf::Color::Black);
		window.setView(view);
		processScaleToggle(scaleToggle);

		// Code to draw contents in the frame
		updateScore(score, gameOver, elapsedTime);
		window.draw(p1);
		window.draw(p2);
		window.draw(p3);
		window.draw(c1);
		window.draw(mp1);
		text.setPosition(sf::Vector2f(c1.getPosition().x - 400, c1.getPosition().y - 300));
		updateScoreHUD(text, score, gameOver);
		window.draw(text);
		mp1.processMovement(elapsedTime);
		if(takeInput)
			c1.processKeyboardInput(elapsedTime);
		c1.processGravity(elapsedTime);

		// GameTime management
		adjustTicSize(t1);
		if (pauseTicker == 0) {
			pause_unpause(pauseTicker, isPaused);
		}
		if (pauseTicker > 0)
			pauseTicker--;
		if (isPaused) {
			elapsedTime = 0;
			t1.restart();
		}
		else
			elapsedTime = t1.restart();

		// collision processing
		bool collision = processCharacterMovingPlatformCollision(c1, mp1);
		if (collision)
			gameOver = true;

		//// Socket Programming
		//string msg = to_string(ClientID) + " " + to_string(i) + " ";
		//zmq::message_t request(msg.size());
		//memcpy(request.data(), msg.data(), msg.size());
		//socket.send(request, zmq::send_flags::none);

		////  Get the reply.
		//zmq::message_t reply;
		//socket.recv(reply, zmq::recv_flags::none);
		//string recieved_data = string(static_cast<char*>(reply.data()), reply.size());
		//cout << recieved_data;



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
void updateScore(float& score, bool& gameOver, float time) {
	if (!gameOver) {
		score += time * 100;
	}
}

// functions related to HUD display
void updateScoreHUD(sf::Text& text, float& score, bool& gameOver) {
	string HUDtext = "Score: " + std::to_string((int)score);
	if (gameOver) {
		HUDtext += " Game Over!";
	}
	text.setString(HUDtext);
}

// functions related to manage game speed
void pause_unpause(int& pauseTicker, bool& isPaused) {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::H)) {
		pauseTicker = 100;
		isPaused = !isPaused;
		cout << "Paused / unpaused";
	}
}

void adjustTicSize(timeLine& t1) {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::K)) {
		t1.ticSize = 1;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::J)) {
		t1.ticSize = 0.5;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {
		t1.ticSize = 2;
	}
}