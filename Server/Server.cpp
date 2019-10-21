#include <SFML/Graphics.hpp>
#include <zmq.hpp>
#include <string>
#include <iostream>
#include <map>
#include <list>
#include <sstream>
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
#define sleep(n)    Sleep(n)
#endif

using namespace std;

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
	void processMovement(float time) {
		float speed = 500;
		float distance = speed * time;
		//cout << "Distance: " << distance;
		int leftpos = this->getPosition().x;
		if (leftpos <= 0)
			towardsLeft = false;
		if (leftpos >= 720)
			towardsLeft = true;
		if (towardsLeft)
			this->move(-(distance), 0.f);
		else
			this->move(distance, 0.f);
	}
};

class clientStats {
public:
	string clientID;
	float client_leftpos;
	float client_toppos;
	clientStats(string clientID, float client_leftpos, float client_toppos) {
		this->clientID = clientID;
		this->client_leftpos = client_leftpos;
		this->client_toppos = client_toppos;
	}
};
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


int main() {
	//  Prepare our context and socket
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REP);
	socket.bind("tcp://*:5555");
	int uniqueClients = 0;
	list<clientStats> clientMapping;
	string delimiter = " ";

	// game logic 
	float elapsedTime = 0;
	timeLine t1;
	MovingPlatform m1;

	// Client moving platform processing


	while (true) {
		zmq::message_t request;

		//  Dont wait for client requests, do thing a synchonously
		socket.recv(request, zmq::recv_flags::dontwait);
		if (!request.empty()) {
			string clientData = string(static_cast<char*>(request.data()), request.size());
			size_t pos = 0;
			std::string token;
			int iteration = 0;
			string ClientID;
			float client_leftpos = 0;
			float client_toppos = 0;
			while ((pos = clientData.find(delimiter)) != std::string::npos) {
				token = clientData.substr(0, pos);
				if (iteration == 0) {
					ClientID = token;
				}
				if (iteration == 1) {
					stringstream converter(token);
					converter >> client_leftpos;
				}
				if (iteration == 2) {
					stringstream converter(token);
					converter >> client_toppos;
				}
				clientData.erase(0, pos + delimiter.length());
				iteration++;
			}
			//cout << "CLient ID is : " << ClientID << " Iteration: " << clientIteration;
			bool matchFound = false;
			std::list<clientStats>::iterator it;
			for (it = clientMapping.begin(); it != clientMapping.end(); ++it) {
				if (it->clientID == ClientID) {
					matchFound = true;
					//cout << " Match found" << endl;
					it->client_leftpos = client_leftpos;
					it->client_toppos = client_toppos;
				}
			}
			if (!matchFound) {
				//cout << " No match found, adding to list" << endl;
				clientMapping.push_back(clientStats(ClientID, client_leftpos, client_toppos));
			}

			iteration = 0;
			string replyString = "";
			replyString += "mp_x: " + to_string(m1.getPosition().x) + " ";
			replyString += "mp_y: " + to_string(m1.getPosition().y) + " ";
			for (it = clientMapping.begin(); it != clientMapping.end(); ++it) {
				iteration++;
				//cout << "Client: " << iteration << " x pos: " << it->client_leftpos << " y pos: "<< it->client_toppos<< endl;
				//replyString += "Client: " + to_string(iteration);
				if (it->clientID.compare(ClientID) == 0)
					continue;
				replyString += "x_pos: " + to_string(it->client_leftpos) + " ";
				replyString += "y_pos: " + to_string(it->client_toppos) + " ";
				replyString += "\n";
			}


			//  Send reply back to client
			zmq::message_t reply(replyString.size());
			std::memcpy(reply.data(), replyString.data(), replyString.size());
			socket.send(reply, zmq::send_flags::none);
		}

		// Game Logic
		// timeline proceeds even when clients dont responds, server works asynchronously
		m1.processMovement(elapsedTime);
		elapsedTime = t1.restart();
	}
	return 0;
}