
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <winsock2.h>
#include <Windows.h>
#include <time.h>
#include <sstream>
#include <iomanip>
#include <thread>

#pragma comment(lib,"ws2_32.lib")
#define SERVER "127.0.0.1"
#define BUFLEN 512
#define PORT 8888

#include "Game.h"
#include "Court.h"
#include "Packet.h"

#include "SFML/Graphics.hpp"

void receiveStates();
void sendMoves();

using namespace sf;
using std::string;
using std::vector;
using std::stringstream;

SOCKET mySocket;
struct sockaddr_in server;
bool sendData;
MovePacket movePacket;

Game game;

int main()
{
	//network
	WSADATA wsa;

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
	server.sin_addr.S_un.S_addr = inet_addr(SERVER);

	//thread
	std::thread sendThread(sendMoves);
	sendThread.detach();
	std::thread receiveThread(receiveStates);
	receiveThread.detach();

	//sfml
	RenderWindow window(VideoMode(Court::w, Court::h), "Volleyball client", Style::Close);
	window.setFramerateLimit(60);	

	//time
	LARGE_INTEGER startTime, endTime, frequency, milliSeconds;
	float fpsTimer = 0;
	float dt = 1.0f / 60;
	float fps = 60;

	while (window.isOpen())
	{
		sendData = false;
		//time
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&startTime);
		
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		if (window.hasFocus())
		{
			if (Keyboard::isKeyPressed(Keyboard::Right))
			{
				movePacket = MovePacket(static_cast<int>(0.5f * dt + 0.5f), 0);
				sendData = true;
			}
			else if (Keyboard::isKeyPressed(Keyboard::Left))
			{
				movePacket = MovePacket(static_cast<int>(-0.5f * dt + 0.5f), 0);
				sendData = true;
			}
		}

		// update + draw
		game.update();
		window.clear(Color(0u, 127u, 255u));
		game.draw(&window);
		window.display();

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

			window.setTitle("Volleyball client (fps: " + framesStream.str() + ")");
			fpsTimer = 0;
		}
	}
	closesocket(mySocket);
	WSACleanup();
	return 0;
}

void sendMoves()
{
	char* sendBuff;
	int serverSize = sizeof(server);
	
	movePacket = MovePacket(0, 0, Message::FIRST);
	sendBuff = movePacket.serialize();
	if (sendto(mySocket, sendBuff, movePacket.size(), 0, (struct sockaddr *) &server, serverSize) == SOCKET_ERROR)
		printf("sendto() failed with error code : %d", WSAGetLastError());
	while (true)
	{
		if (sendData)
		{
			sendBuff = movePacket.serialize();
			if (sendto(mySocket, sendBuff, movePacket.size(), 0, (struct sockaddr *) &server, serverSize) == SOCKET_ERROR)
				printf("sendto() failed with error code : %d", WSAGetLastError());

			printf("sent: ");
			movePacket.print();
			sendData = false;
		}
	}
}

void receiveStates()
{
	char* receiveBuff = new char[movePacket.size()];
	int serverSize = sizeof(server);

	while (true)
	{
		if (recvfrom(mySocket, receiveBuff, movePacket.size(), 0, (struct sockaddr *) &server, &serverSize) == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
			break;
		}
		MovePacket received(receiveBuff);
		printf("received: ");
		received.print();
		game.player1()->move(received.x, received.y);
	}
	delete[] receiveBuff;
}