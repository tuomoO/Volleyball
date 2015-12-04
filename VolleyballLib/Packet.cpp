
#include "Packet.h"

#include <ws2tcpip.h>

int Packet::getSize()
{
	return mSize;
}


MovePacket::MovePacket()
: x(0), y(0)
{
	mSize = 8;
}

MovePacket::MovePacket(float _x, float _y)
: x(_x), y(_y)
{
	mSize = 8;
}

MovePacket::MovePacket(char* data)
{
	mSize = 8;
	u_long temp;

	memcpy(&temp, &data[0], 4);
	x = int(ntohl(temp));

	memcpy(&temp, &data[4], 4);
	y = int(ntohl(temp));
}

MovePacket::~MovePacket()
{

}

char* MovePacket::serialize()
{
	char* result = (char*)malloc(mSize);
	int i = 0;

	*((u_long*)(&result[i])) = htonl(x);
	i += sizeof(x);

	*((u_long*)(&result[i])) = htonl(y);

	return result;
}