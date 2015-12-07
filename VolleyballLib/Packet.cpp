
#include "Packet.h"

#include <ws2tcpip.h>
#include <iostream>

using namespace std;

static Message messageCheck(int message)
{
	switch (message)
	{
	case NOTHING:
		break;
	case FIRST:
		break;
	case QUIT:
		break;
	case PLAYER1:
		break;
	case PLAYER2:
		break;
	case JUMP:
		break;
	default:
		cout << "Invalid message value: " << message << endl;
		return Message::NOTHING;
	}
	return static_cast<Message>(message);
}

//MovePacket
MovePacket::MovePacket()
: x(0), message(Message::NOTHING)
{
}

MovePacket::MovePacket(MovePacket& other)
{
	x = other.x;
	message = other.message;
}

MovePacket::MovePacket(int _x, Message _message)
: x(_x), message(_message)
{
}

MovePacket::MovePacket(char* data)
{
	u_long temp;
	int i = 0;

	memcpy(&temp, &data[i], sizeof(x));
	x = static_cast<int>(ntohl(temp));
	i += sizeof(x);

	memcpy(&temp, &data[i], sizeof(message));
	message = messageCheck(static_cast<int>(ntohl(temp)));
	sizeof(message);
}

char* MovePacket::serialize()
{
	char* result = (char*)malloc(size());
	int i = 0;

	*((u_long*)(&result[i])) = htonl(x);
	i += sizeof(x);

	*((u_long*)(&result[i])) = htonl(static_cast<int>(message));

	return result;
}

int MovePacket::size()
{
	return sizeof(x) + sizeof(message);
}

void MovePacket::print()
{
	cout << "x " << x << ", message " << message << endl;
}

//StatePacket
StatePacket::StatePacket()
{
	x1 = x2 = y1 = y2 = ballx = bally = 0;
	message = Message::NOTHING;
}

StatePacket::StatePacket(int _x1, int _y1, int _x2, int _y2, int _ballx, int _bally, Message _message)
: x1(_x1), y1(_y1), x2(_x2), y2(_y2), ballx(_ballx), bally(_bally), message(_message)
{
}

StatePacket::StatePacket(char* data)
{
	u_long temp;
	int i = 0;

	memcpy(&temp, &data[i], sizeof(x1));
	x1 = static_cast<int>(ntohl(temp));
	i += sizeof(x1);

	memcpy(&temp, &data[i], sizeof(y1));
	y1 = static_cast<int>(ntohl(temp));
	i += sizeof(y1);

	memcpy(&temp, &data[i], sizeof(x2));
	x2 = static_cast<int>(ntohl(temp));
	i += sizeof(x2);

	memcpy(&temp, &data[i], sizeof(y2));
	y2 = static_cast<int>(ntohl(temp));
	i += sizeof(y2);

	memcpy(&temp, &data[i], sizeof(ballx));
	ballx = static_cast<int>(ntohl(temp));
	i += sizeof(ballx);

	memcpy(&temp, &data[i], sizeof(bally));
	bally = static_cast<int>(ntohl(temp));
	i += sizeof(bally);

	memcpy(&temp, &data[i], sizeof(message));
	message = messageCheck(static_cast<int>(ntohl(temp)));
}

char* StatePacket::serialize()
{
	char* result = (char*)malloc(size());
	int i = 0;

	*((u_long*)(&result[i])) = htonl(x1);
	i += sizeof(x1);

	*((u_long*)(&result[i])) = htonl(y1);
	i += sizeof(y1);

	*((u_long*)(&result[i])) = htonl(x2);
	i += sizeof(x2);

	*((u_long*)(&result[i])) = htonl(y2);
	i += sizeof(y2);

	*((u_long*)(&result[i])) = htonl(ballx);
	i += sizeof(ballx);

	*((u_long*)(&result[i])) = htonl(bally);
	i += sizeof(bally);

	*((u_long*)(&result[i])) = htonl(static_cast<int>(message));

	return result;
}

int StatePacket::size()
{
	return sizeof(x1)+sizeof(y1)+sizeof(x2)+sizeof(y2)+sizeof(ballx)+sizeof(bally)+sizeof(message);
}

void StatePacket::print()
{
	cout << "p1: x " << x1 << ", y " << y1 << " | p2: x " << x2 << ", y " << y2 << " | ball: x " << ballx << ", y " << bally << endl;
}
