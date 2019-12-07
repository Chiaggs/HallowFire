#include <SFML/Graphics.hpp>
#include <iostream>
#include "GameObject.h"
#include "EventManager.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <zmq.hpp>
#include <thread>
#include <future>
#include <vector>
#include <fstream>
#include <streambuf>
#include <dukglue/dukglue.h>
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
#define sleep(n)    Sleep(n)
#endif

using namespace std;
int Platform::objectCount = 0;
bool processCharacterMovingPlatformCollision(Character, MovingPlatform);
bool processCharacterDeathZoneCollision(Character, DeathZone);
bool processCharacterSideBoundary(Character, SideBoudary);
void resizeView(const sf::RenderWindow&, sf::View&);
void processScaleToggle(bool&);
void updateScore(float& score, bool& gameOver, float);
void updateScoreHUD(sf::Text&, float&, bool&);
void pause_unpause(int&, bool&);

void adjustTicSize(timeLine&);

class Replay_Instance {
public:
	float player_position_x;
	float player_position_y;
	long timestamp;
	float mp1_x = 0;
	float mp1_y = 0;
	float mp2_x = 0;
	float mp2_y = 0;
	// other player state
	float other_player_x = 0;
	float other_player_y = 0;
	float other_player2_x = 0;
	float other_player2_y = 0;
	float other_player3_x = 0;
	float other_player3_y = 0;
	Replay_Instance(float player_position_x, float player_position_y, long timestamp) {
		this->player_position_x = player_position_x;
		this->player_position_y = player_position_y;
		this->timestamp = timestamp;
	}
};

// ----------- Code for managin scripting ---------------------

class ScriptManager {
public:
	ScriptManager() {

	}
	static void load_script_from_file(duk_context* ctx, const char* filename)
	{
		std::ifstream t(filename);
		std::stringstream buffer;
		buffer << t.rdbuf();
		duk_push_lstring(ctx, buffer.str().c_str(), (duk_size_t)(buffer.str().length()));
	}

	static duk_ret_t native_print(duk_context* ctx)
	{
		duk_push_string(ctx, " ");
		duk_insert(ctx, 0);
		duk_join(ctx, duk_get_top(ctx) - 1);
		printf("%s\n", duk_safe_to_string(ctx, -1));
		return 0;
	}
};

class Vector2f
{
public:
	Vector2f(double x, double y) : m_fX(x), m_fY(y) {}
	Vector2f() { m_fX = 0; m_fY = 0; }

	float Dot(Vector2f& other)
	{
		return m_fX * other.GetX() + m_fY * other.GetY();
	}

	float Distance(Vector2f& other)
	{
		return sqrt(pow(m_fX - other.GetX(), 2) + pow(m_fY - other.GetY(), 2));
	}

	/* Getters and setters */
	double GetX() { return m_fX; }
	double GetY() { return m_fY; }
	void SetX(float x) { m_fX = x; }
	void SetY(float y) { m_fY = y; }

private:
	double m_fX;
	double m_fY;
};

// ------------------------------------------------------------

// client multithreading arg sender
class arg_wrapper {
public:
	string delimiter;
	int ClientID;
	Character c1, c2, c3, c4;
	bool isPaused;
};

// global variables for shared memory between socket and main thread
float other_player_x;
float other_player_y;
float other_player2_x;
float other_player2_y;
float other_player3_x;
float other_player3_y;
float mp_x;
float mp_y;
float mp2_x;
float mp2_y;
EventManager event_manager;

class socket_initializer {
public:
	zmq::context_t context;
	zmq::socket_t socket;
	socket_initializer() {
		context = zmq::context_t(1);
		socket = zmq::socket_t(context, ZMQ_REQ);
		socket.connect("tcp://localhost:5555");
	}
};

socket_initializer s1;

void client_processor(arg_wrapper args_list)
{
	//  Prepare our context and socket
	/*zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REQ);
	socket.connect("tcp://localhost:5555");*/

	string msg = to_string(args_list.ClientID) + " " + to_string(args_list.c1.circle.getPosition().x) + " " + to_string(args_list.c1.circle.getPosition().y) + " ";
	zmq::message_t request(msg.size());
	memcpy(request.data(), msg.data(), msg.size());
	s1.socket.send(request, zmq::send_flags::none);

	//  Get the reply.
	zmq::message_t reply;
	s1.socket.recv(reply, zmq::recv_flags::none);
	string recieved_data = string(static_cast<char*>(reply.data()), reply.size());
	size_t pos = 0;
	int iteration = 0;
	mp_x = 0;
	mp_y = 0;
	mp2_x = 0;
	mp2_y = 0;
	other_player_x = 0;
	other_player_y = 0;
	other_player2_x = 0;
	other_player2_y = 0;
	other_player3_x = 0;
	other_player3_y = 0;
	std::string token;
	//cout << recieved_data << endl;
	while ((pos = recieved_data.find(args_list.delimiter)) != std::string::npos) {
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
			converter >> mp2_x;
		}
		if (iteration == 7) {
			stringstream converter(token);
			converter >> mp2_y;
		}
		if (iteration == 9) {

			stringstream converter(token);
			converter >> other_player_x;
		}
		if (iteration == 11) {

			stringstream converter(token);
			converter >> other_player_y;
		}
		if (iteration == 13) {

			stringstream converter(token);
			converter >> other_player2_x;
		}
		if (iteration == 15) {

			stringstream converter(token);
			converter >> other_player2_y;
		}
		if (iteration == 17) {

			stringstream converter(token);
			converter >> other_player3_x;
		}
		if (iteration == 19) {

			stringstream converter(token);
			converter >> other_player3_y;
		}
		/*if (iteration == 1) {
			stringstream converter(token);
			converter >> client_leftpos;
		}
		if (iteration == 2) {
			stringstream converter(token);
			converter >> client_toppos;
		}*/
		recieved_data.erase(0, pos + args_list.delimiter.length());
		iteration++;
	}
}

int main() {

	// Set initial configuration
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8; // setting the anti-aliasing level, to remove jagged lines
	sf::RenderWindow window;
	window.create(sf::VideoMode(800, 600), "HallowFire", sf::Style::Close | sf::Style::Resize, settings); // Optional thrid arg can be used to specify Window style
	//window.setVerticalSyncEnabled(true); // Setting VSync to true, to prevent screen tearing
	sf::Texture platformTexture;
	sf::Texture platformTexture2;
	sf::Texture platformTexture3;
	sf::View view(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(800, 600));
	sf::Font font;
	sf::Text text;
	float elapsedTime = 0;

	//Defing Entities
	Platform p1, p2, p3;
	SpawnPoint sp1;
	DeathZone dz1;
	SideBoudary sb1;
	Character c1, c2, c3, c4;
	MovingPlatform mp1 (80, 10);
	MovingPlatform mp2(50, 10);
	timeLine t1;
	ViewManager vm;
	// Custom Event Management
	bool gameOver = false;
	bool scaleToggle = false;
	float score = 0;
	int pauseTicker = 0;
	bool isPaused = false;
	bool takeInput = true;
	bool setInitialView = true;
	bool isRecording = false;
	bool isPlayback = false;

	// playback Variables
	milliseconds ms = duration_cast<milliseconds>(
		system_clock::now().time_since_epoch()
		);
	long playback_duration= ms.count();
	long playback_reference_time = 0;
	float playback_multiplier = 1;
	vector<Replay_Instance> replay_array;
	float client_init_x = 0;
	float client_init_y = 0;

	// entities for socket programming
	string delimiter = " ";
	srand(time(0));
	int ClientID = rand();

	// Initializing entities
	if (platformTexture.loadFromFile("Textures/wooden-texture.jpg")) {
		p1.rectangle.setTexture(&platformTexture);
	}
	else {
		cout << "Failure Loading texture";
	}
	if (platformTexture2.loadFromFile("Textures/wooden-texture2.jpg")) {
		p2.rectangle.setTexture(&platformTexture2);
	}
	else {
		cout << "Failure Loading texture";
	}
	if (platformTexture3.loadFromFile("Textures/wooden-texture3.jpg")) {
		p3.rectangle.setTexture(&platformTexture3);
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


	// --------------- Scripting -----------------------

	duk_context* ctx = NULL;

	ctx = duk_create_heap_default();
	if (!ctx) {
		printf("Failed to create a Duktape heap.\n");
		exit(1);
	}
	// Register objects and member functions inside our context
	dukglue_register_constructor<Vector2f>(ctx, "Vector2f");
	dukglue_register_method(ctx, &Vector2f::Dot, "dot");
	dukglue_register_method(ctx, &Vector2f::Distance, "distance");
	dukglue_register_method(ctx, &Vector2f::GetX, "x");
	dukglue_register_method(ctx, &Vector2f::GetY, "y");

	// Can use the standard duktape API to register c_functions if necessary
	ScriptManager sm;
	duk_push_c_function(ctx, sm.native_print, DUK_VARARGS);
	duk_put_global_string(ctx, "print");

	// Load script from file, evaluate script
	sm.load_script_from_file(ctx, "colorScript.js");
	if (duk_peval(ctx) != 0) {
		printf("Error occured: %s\n", duk_safe_to_string(ctx, -1));
		duk_destroy_heap(ctx);
		return 1;
	}
	duk_pop(ctx); // Ignore return, clear stack

	duk_push_global_object(ctx);			
	duk_get_prop_string(ctx, -1, "myTest"); 
											
	Vector2f a(2, 3); Vector2f b(4, 5);
	dukglue_push(ctx, &a);
	dukglue_push(ctx, &b);

	if (duk_pcall(ctx, 2) != 0)
		printf("Error here: %s\n", duk_safe_to_string(ctx, -1));
	else
		printf("%s\n", duk_safe_to_string(ctx, -1));

	string color_red = "red";
	string color_blue = "blue";
	string color_green = "green";
	if (color_red.compare(duk_safe_to_string(ctx, -1)) == 0 ) {
		cout << "Red Detected";
	}
	else if (color_blue.compare(duk_safe_to_string(ctx, -1)) == 0) {
		cout << "Blue Detected";
	}
	else if (color_green.compare(duk_safe_to_string(ctx, -1)) == 0) {
		cout << "Green Detected";
	}
	else {
		cout << "No color Detected";
	}

	duk_pop(ctx);

	duk_destroy_heap(ctx);

	// -------------------------------------------------


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

		//playback processing
		// trigger recording / playback
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
			if (!isRecording) {
				isRecording = true;
				cout << "Recording started" << endl;
				milliseconds ms = duration_cast<milliseconds>(
					system_clock::now().time_since_epoch()
					);
				playback_duration = ms.count();
				replay_array.clear();
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::V)) {
			if (isRecording) {
				isRecording = false;
				cout << "Recording stopped" << endl;
				/*for (auto x : replay_array) {
					cout << x.player_position_x << " " << x.player_position_y << " " << x.timestamp << endl;
				}*/
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
			if (isRecording == false && !isPlayback) {
				isPlayback = true;
				cout << "Playing Back" << endl;
				client_init_x = c1.circle.getPosition().x;
				client_init_y = c1.circle.getPosition().y;
			}
			milliseconds ms = duration_cast<milliseconds>(
				system_clock::now().time_since_epoch()
				);
			playback_reference_time = ms.count();
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::B)) {
			playback_multiplier = 0.5;
			cout << "Playback speed half";
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::N)) {
			playback_multiplier = 1;
			cout << "Playback speed original";
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::M)) {
			playback_multiplier = 2;
			cout << "Playback speed doubled";
		}


		// manage what happens after triggering recording / playback
		if (isRecording) {
			milliseconds ms = duration_cast<milliseconds>(
				system_clock::now().time_since_epoch()
				);
			long current_time = ms.count();
			Replay_Instance replay_instance = Replay_Instance(c1.circle.getPosition().x, c1.circle.getPosition().y, current_time - playback_duration);
			replay_instance.mp1_x = mp1.rectangle.getPosition().x;
			replay_instance.mp1_y = mp1.rectangle.getPosition().y;
			replay_instance.mp2_x = mp2.rectangle.getPosition().x;
			replay_instance.mp2_y = mp2.rectangle.getPosition().y;
			replay_instance.other_player_x = other_player_x;
			replay_instance.other_player_y = other_player_y;
			replay_instance.other_player2_x = other_player2_x;
			replay_instance.other_player2_y = other_player2_y;
			replay_instance.other_player3_x = other_player3_x;
			replay_instance.other_player3_y = other_player3_y;
			replay_array.push_back(replay_instance);
		}
		if (isPlayback) {
			milliseconds ms = duration_cast<milliseconds>(
				system_clock::now().time_since_epoch()
				);
			long hooked_time =  ms.count() - playback_reference_time;
			hooked_time = hooked_time * playback_multiplier;
			if (replay_array.empty()) {
				cout << "Array is emptied";
				isPlayback = false;
				c1.circle.setPosition(client_init_x, client_init_y);
			}
			else {
				Replay_Instance replay_i = replay_array.front();
				c1.circle.setPosition(replay_i.player_position_x, replay_i.player_position_y);
				mp1.rectangle.setPosition(replay_i.mp1_x, replay_i.mp1_y);
				mp2.rectangle.setPosition(replay_i.mp2_x, replay_i.mp2_y);
				other_player_x = replay_i.other_player_x;
				other_player_y = replay_i.other_player_y;
				other_player2_x = replay_i.other_player2_x;
				other_player2_y = replay_i.other_player2_y;
				other_player3_x = replay_i.other_player3_x;
				other_player3_y = replay_i.other_player3_y;
				for (int i = 0; i < replay_array.size(); i++) {
					if (hooked_time > (replay_array.at(i).timestamp)) {
						replay_array.erase(replay_array.begin(), replay_array.begin() + i+1);
						break;
					}
				}
			}
		}


		// reset the frame
		if (setInitialView) {
			vm.processTranslation(true, c1);
			setInitialView = false;
		}
		window.clear(sf::Color::Black);
		window.setView(vm.getView());
		processScaleToggle(scaleToggle);

		// Code to draw contents in the frame
		arg_wrapper args_list;
		args_list.ClientID = ClientID;
		args_list.isPaused = isPaused;
		args_list.delimiter = delimiter;
		args_list.c1 = c1;
		args_list.c2 = c2;
		args_list.c3 = c3;
		args_list.c4 = c4;
		thread th3(client_processor, args_list);

		if (!isPlayback) {
			updateScore(score, gameOver, elapsedTime);
			window.draw(text);
		}
		window.draw(p1.getRectangleObject());
		window.draw(p2.rectangle);
		window.draw(p3.rectangle);
		window.draw(c1.circle);
		window.draw(mp1.rectangle);
		window.draw(mp2.rectangle);
		text.setPosition(sf::Vector2f(c1.circle.getPosition().x - 400, c1.circle.getPosition().y - 300));
		updateScoreHUD(text, score, gameOver);
		if (!isPlayback) {
			if (takeInput)
				c1.processKeyboardInput(elapsedTime);
			c1.processGravity(elapsedTime);
		}

		// GameTime management
		if (takeInput)
			adjustTicSize(t1);
		if (pauseTicker == 0) {
			if (takeInput)
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
		if (!isPlayback) {
			bool collision = processCharacterMovingPlatformCollision(c1, mp1);
			if (collision)
				gameOver = true;
			bool char_death_zone1 = processCharacterDeathZoneCollision(c1, dz1);
			if (char_death_zone1) {
				cout << "Death Zone Collision Occured" << endl;
				c1.circle.setPosition(sp1.rectangle.getPosition());
				//GameObject* goPtr = new Character();
				//event_manager.raiseEvent(Game_Event("character_death", t1, goPtr));
				//delete goPtr;
			}
			bool char_side_boundary1 = processCharacterSideBoundary(c1, sb1);
			if (char_side_boundary1) {
				vm.processTranslation(char_side_boundary1, c1);
				char_side_boundary1 = false;
			}
		}	
		// Socket Programming
		// cout << "Character Positions are: " << c1.getPosition().x << " " << c1.getPosition().y << endl;
		
		th3.join();
		bool other_player = false;
		bool other_player2 = false;
		bool other_player3 = false;
		if (other_player_x != 0 && other_player_y != 0) {
			other_player = true;
			window.draw(c2.circle);
		}
		if (other_player2_x != 0 && other_player2_y != 0) {
			other_player2 = true;
			window.draw(c3.circle);
		}
		if (other_player3_x != 0 && other_player3_y != 0) {
			other_player3 = true;
			window.draw(c4.circle);
		}
		if (!args_list.isPaused) {
			// cout << "Mp's are" << mp_x << mp_y << endl;
			if (!isPlayback) {
				mp1.rectangle.setPosition(sf::Vector2f(mp_x, mp_y));
				mp2.rectangle.setPosition(sf::Vector2f(mp2_x, mp2_y));
			}
			if (other_player)
				c2.circle.setPosition(sf::Vector2f(other_player_x, other_player_y));
			if (other_player2)
				c3.circle.setPosition(sf::Vector2f(other_player2_x, other_player2_y));
			if(other_player3)
				c4.circle.setPosition(sf::Vector2f(other_player3_x, other_player3_y));
		}
		t1.updateGameEngineTime();
		event_manager.handlePendingEvents();
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

bool processCharacterDeathZoneCollision(Character c, DeathZone dz1) {
	bool collision = false;
	sf::FloatRect characterBoundingBox = c.circle.getGlobalBounds();
	sf::FloatRect DeathZoneBoundingBox = dz1.rectangle.getGlobalBounds();
	if (characterBoundingBox.intersects(DeathZoneBoundingBox)) {
		collision = true;
	}
	return collision;
}

bool processCharacterSideBoundary(Character c, SideBoudary sb1) {
	bool collision = false;
	sf::FloatRect characterBoundingBox = c.circle.getGlobalBounds();
	sf::FloatRect SideBoundaryBoundingBox = sb1.rectangle.getGlobalBounds();
	if (characterBoundingBox.intersects(SideBoundaryBoundingBox)) {
		collision = true;
	}
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
		GameObject* goPtr = new Character();
		event_manager.raiseEvent(Game_Event("user_input", t1, goPtr));
		delete goPtr;
		t1.ticSize = 1;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::J)) {
		GameObject* goPtr = new Character();
		event_manager.raiseEvent(Game_Event("user_input", t1, goPtr));
		delete goPtr;
		t1.ticSize = 0.5;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {
		GameObject* goPtr = new Character();
		event_manager.raiseEvent(Game_Event("user_input", t1, goPtr));
		delete goPtr;
		t1.ticSize = 2;
	}
}