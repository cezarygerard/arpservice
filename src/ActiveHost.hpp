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
//#include <boost/serialization/nvp.hpp>
//#include <boost/serialization/export.hpp> 
//#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
//#include <boost/serialization/version.hpp>

using namespace utils;

//ActiveHost jaki jest kazdy widzi:
class ActiveHost
{
	friend class boost::serialization::access;

public:

	ActiveHost();
	~ActiveHost();
	MacAdress mac;

	IPAddress ip;

	IPAddress netaddr;

	//poczatek aktywnosci w sieci format YYYY-MM-DD-HH-MM-SS
	string start;

	//koniec aktywnosci format YYYY-MM-DD-HH-MM-SS
	string stop;

	//'flaga', time-to-live, inicjalizowana na 5, gdy chost jest aktywny pozostaje na 5, jezeli
	//go nie ma, zmiejszana o 1, gdy dojdzie do zeraz host przechodzi do historii(bazy danych)
	int ttl;

	///szablon umozliwiajacy serializacje klasy
	///@param &ar archiwum z przestrzeni nazw boost::archive
	///@param version pole umozliwiajace wersjonowanie klasy, poki co niewykorzystane
	template<class Archive>
    void save(Archive & ar, const unsigned int version) const
    {
		ar & BOOST_SERIALIZATION_NVP(ip.ip);
		ar & BOOST_SERIALIZATION_NVP(mac.m);
		ar & BOOST_SERIALIZATION_NVP(netaddr.ip);
		ar & BOOST_SERIALIZATION_NVP(ttl);
		ar & BOOST_SERIALIZATION_NVP(start);
		ar & BOOST_SERIALIZATION_NVP(stop);
	}

	template<class Archive>
    void load(Archive & ar, const unsigned int version)
    {
		ar & BOOST_SERIALIZATION_NVP(ip.ip);
		ar & BOOST_SERIALIZATION_NVP(mac.m);
		ar & BOOST_SERIALIZATION_NVP(netaddr.ip);
		ar & BOOST_SERIALIZATION_NVP(ttl);
		ar & BOOST_SERIALIZATION_NVP(start);
		ar & BOOST_SERIALIZATION_NVP(stop);

	}

		BOOST_SERIALIZATION_SPLIT_MEMBER();

};
//BOOST_CLASS_VERSION(ActiveHost, 0)


//bool operator<(const MacAdress& mac1, const MacAdress& mac2 );

#endif
