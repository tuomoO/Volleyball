
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
#define PORT 8888

#include "Game.h"
#include "Court.h"
#include "Packet.h"

#include "SFML/Graphics.hpp"

using namespace sf;
using std::string;
using std::vector;
using std::stringstream;

void sendStates();
void receiveMoves();
MovePacket checkMove(MovePacket& move);

Game game;
SOCKET mySocket;
struct sockaddr_in server, client;
MovePacket packet;
bool sendData;

int main()
{
	//network
	WSADATA wsa;
	sendData = false;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	if ((mySocket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	if (bind(mySocket, (struct sockaddr *)&server, sizeof(server)) == static_cast<unsigned int>(SOCKET_ERROR))
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	std::thread sendThread(sendStates);
	sendThread.detach();
	std::thread receiveThread(receiveMoves);
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
		//time
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&startTime);

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
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

			window.setTitle("Volleyball server (fps: " + framesStream.str() + ")");
			fpsTimer = 0;
		}
	}
	closesocket(mySocket);
	WSACleanup();
	return 0;
}

void sendStates()
{
	int clientSize = sizeof(client);
	while (true)
	{
		if (sendData)
		{
			packet = checkMove(packet);
			game.player1()->move(packet.x, packet.y);
			printf("sent: ");
			packet.print();

			if (sendto(mySocket, packet.serialize(), packet.size(), 0, (struct sockaddr*) &client, clientSize) == SOCKET_ERROR)
				printf("sendto() failed with error code : %d", WSAGetLastError());
			sendData = false;
		}
	}
}

void receiveMoves()
{
	char* buffer = new char[packet.size()];
	int clientSize = sizeof(client);
	int bytesReceived;
	while (true)
	{
		//fflush(stdout);
		if ((bytesReceived = recvfrom(mySocket, buffer, packet.size(), 0, (struct sockaddr *) &client, &clientSize)) == SOCKET_ERROR)
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
		packet = MovePacket(buffer);
		printf("Received packet from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
		printf("received: ");
		packet.print();
		sendData = true;
	}
	delete[] buffer;
}

MovePacket checkMove(MovePacket& move)
{
	MovePacket result(move.x, move.y);
	int x = game.player1()->getRealPos().x + move.x;
	if (x < 64)
		result.x = game.player1()->getRealPos().x - 64;
	else if (x > Court::w - 64)
		result.x = Court::w - 64 - game.player1()->getRealPos().x;
	return result;
}