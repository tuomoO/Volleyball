
#pragma once

enum Message
{
	NOTHING,
	FIRST,
	QUIT,
	PLAYER1,
	PLAYER2,
	JUMP
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
	MovePacket(int _x, Message _message = Message::NOTHING);
	MovePacket(char* data);
	~MovePacket() = default;

	char* serialize();
	int size();
	void print();

	int x;
	Message message;
};

class StatePacket : public Packet
{
public:
	StatePacket();
	StatePacket(int _x1, int _y1, int _x2, int _y2, int _ballx, int _bally, Message _message = Message::NOTHING);
	StatePacket(char* data);
	~StatePacket() = default;

	char* serialize();
	int size();
	void print();

	int x1, y1, x2, y2, ballx, bally;
	Message message;
};
