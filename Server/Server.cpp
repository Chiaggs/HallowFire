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

class clientStats {
public:
	string clientID;
	int clientIterations;
	clientStats(string clientID, int clientIterations) {
		this->clientID = clientID;
		this->clientIterations = clientIterations;
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
	while (true) {
		zmq::message_t request;

		//  Wait for next request from client
		socket.recv(request, zmq::recv_flags::dontwait);
		if (!request.empty()) {
			string clientData = string(static_cast<char*>(request.data()), request.size());
			size_t pos = 0;
			std::string token;
			int iteration = 0;
			string ClientID;
			int clientIteration = 0;
			while ((pos = clientData.find(delimiter)) != std::string::npos) {
				token = clientData.substr(0, pos);
				if (iteration == 0) {
					ClientID = token;
				}
				if (iteration == 1) {
					stringstream converter(token);
					converter >> clientIteration;
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
					it->clientIterations = clientIteration;
				}
			}
			if (!matchFound) {
				//cout << " No match found, adding to list" << endl;
				clientMapping.push_back(clientStats(ClientID, clientIteration));
			}

			iteration = 0;
			string replyString = "";
			for (it = clientMapping.begin(); it != clientMapping.end(); ++it) {
				iteration++;
				cout << "Client: " << iteration << " Itertion: " << it->clientIterations << endl;
				replyString += "Client: " + to_string(iteration);
				replyString += " Iteration: " + to_string(it->clientIterations);
				replyString += "\n";
			}
			//  Do some 'work'
			sleep(100);

			//  Send reply back to client
			zmq::message_t reply(replyString.size());
			memcpy(reply.data(), replyString.data(), replyString.size());
			socket.send(reply, zmq::send_flags::none);
		}
	}
	return 0;
}