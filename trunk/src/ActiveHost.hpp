#ifndef	ACTIVE_HOST_H
#define ACTIVE_HOST_H

#include "utils.hpp" 

using namespace utils;

struct ActiveHost
{
	ActiveHost();
	MacAdress mac;
	IPAddress ip;
	string start;
	string stop;
	int ttl;
};

//bool operator<(const MacAdress& mac1, const MacAdress& mac2 );

#endif