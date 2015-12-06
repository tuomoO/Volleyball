
#pragma once

enum Message
{
	NOTHING,
	FIRST,
	QUIT,
	PLAYER1,
	PLAYER2
};

static Message messageCheck(int message);

class Packet
{
public:
	Packet() = default;
	~Packet() = default;
	virtual char* serialize() = 0;
	virtual int size() = 0;
	virtual void print() = 0;
};

class MovePacket : public Packet
{
public:
	MovePacket();
	MovePacket(MovePacket& other);
	MovePacket(int _x, int _y, Message _message = Message::NOTHING);
	MovePacket(char* data);
	~MovePacket() = default;

	char* serialize();
	int size();
	void print();

	int x, y;
	Message message;
};

class StatePacket : public Packet
{
	StatePacket();
	StatePacket(int _player, int _x1, int _y1, int _x2, int _y2, int _ballx, int _bally);
	StatePacket(char* data);
	~StatePacket() = default;

	char* serialize();
	int size();
	void print();

	int player, x1, y1, x2, y2, ballx, bally;
};
