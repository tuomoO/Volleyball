
#pragma once

class Packet
{
public:
	Packet() = default;
	~Packet() = default;
	virtual char* serialize() = 0;
	virtual int size() = 0;
};

class MovePacket : public Packet
{
public:
	MovePacket();
	MovePacket(MovePacket& other);
	MovePacket(int _x, int _y);
	MovePacket(char* data);
	~MovePacket() = default;

	char* serialize();
	int size();

	int x, y;
};

class StatePacket : public Packet
{
	StatePacket();
	StatePacket(int _player, int _x1, int _y1, int _x2, int _y2, int _ballx, int _bally);
	StatePacket(char* data);
	~StatePacket() = default;

	char* serialize();
	int size();

	int player, x1, y1, x2, y2, ballx, bally;
};
