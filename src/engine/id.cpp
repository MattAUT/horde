#include "id.h"

#include <utils.h>
#include <limits>
#include <cstdio>
#include "logmanager.h"

ID::ID()
{	
	x = rand();
	y = rand();
}

ID::~ID()
{

}

bool ID::operator==(const ID& other)
{
	return x == other.x && y == other.y;
}

std::string ID::AsString()
{
	char str[2 + sizeof(uint16_t) * 4 + 1];
	sprintf_s(str, "0x%04X%04X", x, y);
	return str;
}