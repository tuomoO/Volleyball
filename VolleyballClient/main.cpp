
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

#include "Slime.h"
#include "Ball.h"
#include "Court.h"
#include "Packet.h"

#include "SFML/Graphics.hpp"

void receiveThread();

using namespace sf;
using std::string;
using std::vector;
using std::stringstream;

void receiveThread();

SOCKET mySocket;
struct sockaddr_in server;
bool sendData;
MovePacket packet;
Slime *player1, *player2;
Ball* volleyball;

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

	std::thread networkThread(receiveThread);
	networkThread.detach();

	//sfml
	RenderWindow window(VideoMode(Court::w, Court::h), "Volleyball client", Style::Close);
	window.setFramerateLimit(60);
	Texture texture;
	texture.loadFromFile("circle.png");

	Slime p1(&texture, Color(0, 255, 0));
	p1.setRealPos(64, Court::h);
	player1 = &p1;
	Ball ball(&texture);
	ball.setRealPos(Court::w / 2, Court::h / 2);
	Slime p2(&texture, Color(255, 0, 0));
	p2.setRealPos(Court::w - 64, Court::h);
	player2 = &p2;

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
				packet = MovePacket(static_cast<int>(0.5f * dt + 0.5f), 0);
				sendData = true;
			}
			else if (Keyboard::isKeyPressed(Keyboard::Left))
			{
				packet = MovePacket(static_cast<int>(-0.5f * dt + 0.5f), 0);
				sendData = true;
			}
		}
		p1.update();
		p2.update();
		ball.update();

		//draw
		window.clear(Color(0u, 127u, 255u));
		window.draw(p1.getShape());
		window.draw(p2.getShape());
		window.draw(ball.getShape());
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

void receiveThread()
{
	char* sendBuff;
	char* receiveBuff = new char[packet.size()];
	int serverSize = sizeof(server);

	while (true)
	{
		if (sendData)
		{
			sendBuff = packet.serialize();
			if (sendto(mySocket, sendBuff, packet.size(), 0, (struct sockaddr *) &server, serverSize) == SOCKET_ERROR)
			{
				printf("sendto() failed with error code : %d", WSAGetLastError());
				break;
			}
			printf("sent: x %d, y %d\n", packet.x, packet.y);

			if (recvfrom(mySocket, receiveBuff, packet.size(), 0, (struct sockaddr *) &server, &serverSize) == SOCKET_ERROR)
			{
				printf("recvfrom() failed with error code : %d", WSAGetLastError());
				break;
			}
			MovePacket received(receiveBuff);
			printf("received: x %d, y %d\n", received.x, received.y);
			player1->move(received.x, received.y);
			sendData = false;
		}
	}
	delete[] receiveBuff;
}