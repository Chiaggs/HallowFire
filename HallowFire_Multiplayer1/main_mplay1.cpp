#include <SFML/Graphics.hpp>
#include <iostream>
#include "GameObject.h"
#include "TimeLine.h"
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
	Character c1, c2, c3;
	MovingPlatform mp1;
	timeLine t1;
	bool gameOver = false;
	bool scaleToggle = false;
	float score = 0;
	int pauseTicker = 0;
	bool isPaused = false;
	bool takeInput = true;

	// entities for socket programming
	string delimiter = " ";

	// Initializing entities
	if (platformTexture.loadFromFile("Textures/wooden-texture.jpg")) {
		p1.rectangle.setTexture(&platformTexture);
		p2.rectangle.setTexture(&platformTexture);
		p3.rectangle.setTexture(&platformTexture);
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
		view.setCenter(sf::Vector2f(c1.circle.getPosition().x, c1.circle.getPosition().y));
		window.clear(sf::Color::Black);
		window.setView(view);
		processScaleToggle(scaleToggle);

		// Code to draw contents in the frame
		updateScore(score, gameOver, elapsedTime);
		window.draw(p1.getRectangleObject());
		window.draw(p2.rectangle);
		window.draw(p3.rectangle);
		window.draw(c1.circle);
		window.draw(mp1.rectangle);
		text.setPosition(sf::Vector2f(c1.circle.getPosition().x - 400, c1.circle.getPosition().y - 300));
		updateScoreHUD(text, score, gameOver);
		window.draw(text);
		//mp1.processMovement(elapsedTime);
		if(takeInput)
			c1.processKeyboardInput(elapsedTime);
		c1.processGravity(elapsedTime);

		// GameTime management
		if(takeInput)
			adjustTicSize(t1);
		if (pauseTicker == 0) {
			if(takeInput)
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

		// Socket Programming
		//cout << "Character Positions are: " << c1.getPosition().x << " " << c1.getPosition().y << endl;
		string msg = to_string(ClientID) + " " + to_string(c1.circle.getPosition().x) + " " + to_string(c1.circle.getPosition().y) + " ";
		zmq::message_t request(msg.size());
		memcpy(request.data(), msg.data(), msg.size());
		socket.send(request, zmq::send_flags::none);

		//  Get the reply.
		zmq::message_t reply;
		socket.recv(reply, zmq::recv_flags::none);
		string recieved_data = string(static_cast<char*>(reply.data()), reply.size());
		size_t pos = 0;
		int iteration = 0;
		float mp_x = 0;
		float mp_y = 0;
		float other_player_x = 0;
		float other_player_y = 0;
		float other_player2_x = 0;
		float other_player2_y = 0;
		std::string token;
		//cout << recieved_data << endl;
		while ((pos = recieved_data.find(delimiter)) != std::string::npos) {
			token = recieved_data.substr(0, pos);
			if (iteration == 1) {
				stringstream converter(token);
				converter >> mp_x;
			}
			if (iteration == 3) {
				stringstream converter(token);
				converter >> mp_y;
			}
			if (iteration == 5) {
				
				stringstream converter(token);
				converter >> other_player_x;
			}
			if (iteration == 7) {

				stringstream converter(token);
				converter >> other_player_y;
			}
			if (iteration == 9) {

				stringstream converter(token);
				converter >> other_player2_x;
			}
			if (iteration == 11) {

				stringstream converter(token);
				converter >> other_player2_y;
			}
			/*if (iteration == 1) {
				stringstream converter(token);
				converter >> client_leftpos;
			}
			if (iteration == 2) {
				stringstream converter(token);
				converter >> client_toppos;
			}*/
			recieved_data.erase(0, pos + delimiter.length());
			iteration++;
		}
		bool other_player = false;
		bool other_player2 = false;
		if (other_player_x != 0 && other_player_y != 0) {
			other_player = true;
			window.draw(c2.circle);
		}
		if (other_player2_x != 0 && other_player2_y != 0) {
			other_player2 = true;
			window.draw(c3.circle);
		}
		if (!isPaused) {
			mp1.rectangle.setPosition(sf::Vector2f(mp_x, mp_y));
			if(other_player)
				c2.circle.setPosition(sf::Vector2f(other_player_x, other_player_y));
			if(other_player2)
				c3.circle.setPosition(sf::Vector2f(other_player2_x, other_player2_y));
		}
		// end of the current frame
		window.display();
	}
	return 0;
}

// functions to detect collision
bool processCharacterMovingPlatformCollision(Character c, MovingPlatform mp) {
	bool collision = false;
	sf::FloatRect characterBoundingBox = c.circle.getGlobalBounds();
	sf::FloatRect movingPlatformBoundingBox = mp.rectangle.getGlobalBounds();
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