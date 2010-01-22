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
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp> 
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

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

	template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
		//ar & BOOST_SERIALIZATION_NVP(thisSqn_); 
		//ar & (thisSqn_); 
		ip_ & ip.ip;
		mac_ & mac.m;
		start_ & start;
		stop_ & stop;
		ttl_ & ttl;
	}
};

//bool operator<(const MacAdress& mac1, const MacAdress& mac2 );

#endif