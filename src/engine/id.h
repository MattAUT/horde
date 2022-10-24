#ifndef __ID_H__
#define __ID_H__

#include <string>

class ID
{
public:
	ID();
	~ID();
	bool operator==(const ID& other);
	std::string AsString();

protected:
	uint16_t x;
	uint16_t y;

};

#endif // __ID_H__