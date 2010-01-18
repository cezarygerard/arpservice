/**
* @author Cezary Zawadka
* @date 2010.01.17
* @version 0.9
*
*	
*/

#ifndef	ACTIVE_HOST_H
#define ACTIVE_HOST_H

#include "utils.hpp" 

using namespace utils;

//ActiveHost jaki jest kazdy widzi:
struct ActiveHost
{
	ActiveHost();

	MacAdress mac;

	IPAddress ip;

	//poczatek aktywnosci w sieci format YYYY-MM-DD-HH-MM-SS
	string start;

	//koniec aktywnosci format YYYY-MM-DD-HH-MM-SS
	string stop;

	//'flaga', time-to-live, inicjalizowana na 5, gdy chost jest aktywny pozostaje na 5, jezeli
	//go nie ma, zmiejszana o 1, gdy dojdzie do zeraz host przechodzi do historii(bazy danych)
	int ttl;
};

//bool operator<(const MacAdress& mac1, const MacAdress& mac2 );

#endif