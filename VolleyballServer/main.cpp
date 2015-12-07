
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
MovePacket checkMove(MovePacket& move, int player);
int compareClient(sockaddr_in const &other);

Game game;
SOCKET mySocket;
struct sockaddr_in server, client1, client2;
bool sendData;
int clientCount;
bool initializeClient1;
bool initializeClient2;

int main()
{
	//network
	WSADATA wsa;
	sendData = false;
	clientCount = 0;
	initializeClient1 = false;
	initializeClient2 = false;

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
	int clientSize = sizeof(client1);
	while (true)
	{
		if (sendData)
		{
			StatePacket packet = game.getState();
			if (clientCount > 0)
			{
				//player 1
				if (initializeClient1)
					packet.message = Message::PLAYER1;
				if (sendto(mySocket, packet.serialize(), packet.size(), 0, (struct sockaddr*) &client1, clientSize) == SOCKET_ERROR)
					printf("sendto() player 1 failed with error code : %d", WSAGetLastError());
				if (initializeClient1)
				{
					initializeClient1 = false;
					packet.message = Message::NOTHING;
				}
				
				//player2
				if (clientCount > 1)
				{
					if (initializeClient2)
						packet.message = Message::PLAYER2;
					if (sendto(mySocket, packet.serialize(), packet.size(), 0, (struct sockaddr*) &client2, clientSize) == SOCKET_ERROR)
						printf("sendto() player 2 failed with error code : %d", WSAGetLastError());
					if (initializeClient2)
						initializeClient2 = false;
				}
			}
			printf("sent: ");
			packet.print();
			sendData = false;
		}
	}
}

void receiveMoves()
{
	MovePacket packet;
	char* buffer = new char[packet.size()];
	
	struct sockaddr_in tempClient;
	int clientSize = sizeof(tempClient);
	int bytesReceived;
	while (true)
	{
		if ((bytesReceived = recvfrom(mySocket, buffer, packet.size(), 0, (struct sockaddr *) &tempClient, &clientSize)) == SOCKET_ERROR)
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
		packet = MovePacket(buffer);
		printf("received: ");
		packet.print();

		//client check
		int player = 0;
		if (clientCount == 0)
		{
			client1 = tempClient;
			clientCount++;
			player = 1;
			initializeClient1 = true;
		}
		else if (clientCount == 1 && compareClient(tempClient) != 1)
		{
			client2 = tempClient;
			clientCount++;
			player = 2;
			initializeClient2 = true;
		}
		else
			player = compareClient(tempClient);
		
		printf("from ");
		if (player == 0)
			printf("%s:%d\n", inet_ntoa(tempClient.sin_addr), ntohs(tempClient.sin_port));
		else
			printf("player %d\n", player);

		//update
		if (player > 0)
		{
			packet = checkMove(packet, player);
			if (player == 1)
				game.player1()->move(packet.x, packet.y);
			else
				game.player2()->move(packet.x, packet.y);
			sendData = true;
		}
	}
	delete[] buffer;
}

MovePacket checkMove(MovePacket& move, int player)
{
	Vector2i pos;
	if (player == 1)
		pos = game.player1()->getRealPos();
	else
		pos = game.player2()->getRealPos();

	MovePacket result(move.x, move.y);
	int x = pos.x + move.x;
	if (x < 64)
		result.x = 64 - pos.x;
	else if (x > Court::w - 64)
		result.x = Court::w - 64 - pos.x;
	return result;
}

int compareClient(sockaddr_in const &other)
{ 
	int player = 0;
	if (client1.sin_addr.S_un.S_addr == other.sin_addr.S_un.S_addr && client1.sin_port == other.sin_port)
		player = 1;
	else if (client2.sin_addr.S_un.S_addr == other.sin_addr.S_un.S_addr && client2.sin_port == other.sin_port)
		player = 2;
	return player;
}