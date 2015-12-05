
#include "Packet.h"

#include <ws2tcpip.h>

//MovePacket
MovePacket::MovePacket()
: x(0), y(0)
{
}

MovePacket::MovePacket(MovePacket& other)
{
	x = other.x;
	y = other.y;
}

MovePacket::MovePacket(int _x, int _y)
: x(_x), y(_y)
{
}

MovePacket::MovePacket(char* data)
{
	u_long temp;

	memcpy(&temp, &data[0], 4);
	x = static_cast<int>(ntohl(temp));

	memcpy(&temp, &data[4], 4);
	y = static_cast<int>(ntohl(temp));
}

char* MovePacket::serialize()
{
	char* result = (char*)malloc(size());
	int i = 0;

	*((u_long*)(&result[i])) = htonl(x);
	i += sizeof(x);

	*((u_long*)(&result[i])) = htonl(y);

	return result;
}

int MovePacket::size()
{
	return 8;
}


//StatePacket
StatePacket::StatePacket()
{
	player = x1 = x2 = y1 = y2 = ballx = bally = 0;
}

StatePacket::StatePacket(int _player, int _x1, int _y1, int _x2, int _y2, int _ballx, int _bally)
: player(_player), x1(_x1), y1(_y1), x2(_x2), y2(_y2), ballx(_ballx), bally(_bally)
{
}

StatePacket::StatePacket(char* data)
{
	u_long temp;
	int i = 0;

	memcpy(&temp, &data[i], sizeof(player));
	player = static_cast<int>(ntohl(temp));
	i += sizeof(player);

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
}

char* StatePacket::serialize()
{
	char* result = (char*)malloc(size());

	return result;
}

int StatePacket::size()
{
	return 28;
}
