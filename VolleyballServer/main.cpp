
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

#include "SFML/Graphics.hpp"

using namespace sf;
using std::string;
using std::vector;
using std::stringstream;

void receiveThread();

SOCKET s;
int recv_len;
int slen;
struct sockaddr_in server, si_other;

int main()
{
	//network
	WSADATA wsa;
	slen = sizeof(si_other);

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	if (bind(s, (struct sockaddr *)&server, sizeof(server)) == static_cast<unsigned int>(SOCKET_ERROR))
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	std::thread networkThread(receiveThread);
	networkThread.detach();

	//sfml
	RenderWindow window(VideoMode(Court::w, Court::h), "Volleyball client", Style::Close);
	window.setFramerateLimit(60);
	Texture texture;
	texture.loadFromFile("circle.png");

	Slime p1(&texture, Color(0, 255, 0));
	p1.setPosition(64, Court::h);
	Ball ball(&texture);
	ball.setPosition(Court::w / 2.0f, Court::h / 2.0f);
	Slime p2(&texture, Color(255, 0, 0));
	p2.setPosition(Court::w - 64, Court::h);

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

			window.setTitle("Volleyball server (fps: " + framesStream.str() + ")");
			fpsTimer = 0;
		}
	}

	return 0;
}

void receiveThread()
{
	char buf[BUFLEN];
	while (true)
	{
		printf("Waiting for data..."); fflush(stdout);

		memset(buf, '\0', BUFLEN);
		if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
		printf("Data: %s\n", buf);

		if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR)
		{
			printf("sendto() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
	}
}