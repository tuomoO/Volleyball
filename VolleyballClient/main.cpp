
#define PORT 8888

#include "Network.h"
#include "Game.h"
#include "Court.h"
#include "Packet.h"

#include "SFML/Graphics.hpp"

#include <time.h>
#include <sstream>
#include <iomanip>

void receiveStates();
void sendMoves();
void useAlternativeControls();

using std::string;
using std::vector;
using std::stringstream;
using sf::Keyboard;

SOCKET mySocket;
struct sockaddr_in server;
bool sendData;
MovePacket movePacket;
std::mutex writeMutex;

Game game;
bool initialised;
Keyboard::Key leftKey;
Keyboard::Key rightKey;
Keyboard::Key jumpKey;

int main(int argc, char **argv)
{
	//network
	WSADATA wsa;
	initialised = false;

	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d\n", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	//create socket
	if ((mySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		printf("socket() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	//setup address structure
	memset((char *)&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	if (argc != 2)
		server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	else
		server.sin_addr.S_un.S_addr = inet_addr(argv[1]);

	//keyboard
	leftKey = Keyboard::A;
	rightKey = Keyboard::D;
	jumpKey = Keyboard::W;

	//thread
	game.start();
	std::thread sendThread(sendMoves);
	sendThread.detach();
	std::thread receiveThread(receiveStates);
	receiveThread.detach();

	//sfml
	string windowTitleBase = "Volleyball client, player ";
	sf::RenderWindow window(sf::VideoMode(Court::w, Court::h), windowTitleBase, sf::Style::Close);
	window.setFramerateLimit(60);	

	//time
	LARGE_INTEGER startTime, endTime, frequency, milliSeconds;
	float fpsTimer = 0;
	float dt = 1.0f / 60;
	float fps = 60;

	while (game.isRunning())
	{
		sendData = false;
		//time
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&startTime);

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				game.stop();
		}
		if (initialised)
		{
			if (Keyboard::isKeyPressed(rightKey))
			{
				movePacket.x = static_cast<int>(0.5f * dt + 0.5f);
				sendData = true;
			}
			else if (Keyboard::isKeyPressed(leftKey))
			{
				movePacket.x = static_cast<int>(-0.5f * dt + 0.5f);
				sendData = true;
			}
			if (Keyboard::isKeyPressed(jumpKey))
			{
				movePacket.message = Message::JUMP;
				sendData = true;
			}
		}

		game.draw(&window);

		//time
		QueryPerformanceCounter(&endTime);
		milliSeconds.QuadPart = endTime.QuadPart - startTime.QuadPart;
		milliSeconds.QuadPart *= 1000;
		dt = milliSeconds.QuadPart / static_cast<float>(frequency.QuadPart);

		fpsTimer += dt;
		if (fpsTimer > 100)
		{
			fps = (0.8f * fps + 0.2f * ((1.0f / dt) * 1000));
			stringstream framesStream;
			framesStream.precision(2);
			framesStream << std::setfill('0') << std::setw(4) << std::fixed << fps;
			stringstream playerNumber;
			playerNumber << game.getLocalPlayerNumber();

			window.setTitle(windowTitleBase + playerNumber.str() + " (fps: " + framesStream.str() + ")");
			fpsTimer = 0;
		}
	}
	window.close();
	//sendThread.join();
	//receiveThread.join();
	closesocket(mySocket);
	WSACleanup();
	return 0;
}

void sendMoves()
{
	char* sendBuff;
	int serverSize = sizeof(server);
	
	writeMutex.lock();
	movePacket = MovePacket(0, Message::FIRST);
	sendBuff = movePacket.serialize();
	if (sendto(mySocket, sendBuff, movePacket.size(), 0, (struct sockaddr *) &server, serverSize) == SOCKET_ERROR)
		printf("sendto() failed with error code : %d", WSAGetLastError());
	writeMutex.unlock();
	while (game.isRunning())
	{
		if (sendData)
		{
			writeMutex.lock();
			sendBuff = movePacket.serialize();
			if (sendto(mySocket, sendBuff, movePacket.size(), 0, (struct sockaddr *) &server, serverSize) == SOCKET_ERROR)
				printf("sendto() failed with error code : %d", WSAGetLastError());
			printf("sent: ");
			movePacket.print();
			movePacket = MovePacket();
			writeMutex.unlock();
			sendData = false;
		}
	}
}

void receiveStates()
{
	StatePacket state;
	char* receiveBuff = new char[state.size()];
	int serverSize = sizeof(server);

	while (game.isRunning())
	{
		if (recvfrom(mySocket, receiveBuff, state.size(), 0, (struct sockaddr *) &server, &serverSize) == SOCKET_ERROR)
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
		state = StatePacket(receiveBuff);
		writeMutex.lock();
		printf("received: ");
		state.print();
		writeMutex.unlock();
		game.updateState(state);
		if (state.message == Message::PLAYER1 || state.message == Message::PLAYER2)
		{
			game.selectLocalPlayer(state.message);
			initialised = true;

			//for localhost demo purposes
			if (state.message == Message::PLAYER2)
				useAlternativeControls();
		}
	}
	delete[] receiveBuff;
}

void useAlternativeControls()
{
	leftKey = Keyboard::Left;
	rightKey = Keyboard::Right;
	jumpKey = Keyboard::Up;
}