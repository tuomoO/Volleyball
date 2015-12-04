
#pragma once

class Packet
{
public:
	Packet() = default;
	~Packet() = default;
	virtual char* serialize() = 0;

	int getSize();

protected:
	int mSize;
};

class MovePacket : public Packet
{
public:
	MovePacket();
	MovePacket(float _x, float _y);
	MovePacket(char* data);

	~MovePacket();

	char* serialize();

	float x;
	float y;
private:

};