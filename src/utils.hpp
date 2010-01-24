/**
* @author Cezary Zawadka, Sebastian £uczak  
* @date 2010.01.17
* @version 0.9
*
*@todo IPAdress i MacAdress powinny byc klasami z metodami do ich formatowania	
*/

#ifndef UTILS_H
#define UTILS_H

#define HAVE_REMOTE 
#include <stdio.h>
#include "pcap.h"
#include <iostream>
#include <winsock.h>
#include <Windows.h>
#include <Iphlpapi.h>
#include <Assert.h>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include<math.h> 

#pragma comment(lib, "iphlpapi.lib")

using namespace std;

/// @namespace utils Przechowuje funkcje i struktury uzywane w kodzie wielu klas

namespace utils
{
	///rozmiar pakietu ARP request
	const int ARP_REQ_SIZE = 60; //bytes
	const int ARP_SEND_PERIOD = 15; //seconds
	const int MAX_TTL = 7 * ARP_SEND_PERIOD;
	const int RECEIVER_SLEEP_PERIOD = 1; //ms
	const int HANDLE_SLEEP_PERIOD = 1; //seconds
	const int ETH_HEADER_OFFSET = 14; //bytes

	const int SCREEN_WIDTH = 1000; //px
	const int SCREEN_HEIGHT = 800; //px
	const int SCREEN_BPP = 32;

	const int PICTURE_OFFSET = 120; //px

	const int X_LIMIT = 8; 
	const int Y_LIMIT = 6;

	const int FRAMES_PER_SECOND = 20; // dzielnik

	struct IPAddress
	{
		unsigned char ip[4];
	};

	struct MacAdress
	{
		unsigned char m[6];
	};

///klasa potrzebna do porownywania adresow mac, aby mogly byc kluczem w std::map
struct lessMAC// : public binary_function<MacAdress, MacAdress, bool>
{	
	bool operator()(const MacAdress& m1, const MacAdress& m2) const;
};

	// ARP
	struct ARPHeader
	{
		u_short htype;
		u_short ptype;
		u_char hlen;
		u_char plen;
		u_short oper;
		MacAdress sha;
		IPAddress spa;
		MacAdress tha;
		IPAddress tpa;
	};

	int charToHex(char c);


//	struct IPAddress;
//	struct MacAdress;
	///zmiania IP z long int na string
	
	string iptos(u_long in);

	///zmiania IP ze struktury utils::IPAddress na string, format "192.168.1.1"
	string iptos(IPAddress in);
	
	MacAdress sToMac(string s);

	IPAddress sToIp(string s);

	///zamienia mac na string, format: "0044f6e13344"
	string macToS(MacAdress in);

	///wypisanie informaqcji o danym interfejsie, uzywane w utils::init()
	void ifprint(pcap_if_t *d);
	
	///zmiania IP z long int na utils::IPAddress 
	IPAddress iptoIP(u_long in);

	///inicjalizacja programu - wypisanie listy interfejsow, zaczytanie ktory interfejs ma byc uzyty
	pcap_if_t * init();
	
	///wypisuje na standardowe wyjscie pierwsze size bajtow z tablicy data
	void printBytes(unsigned char data[], int size);
	
	///zwraca MAC dla interfejsu o podanym IP
	///@return utils::MacAdress
	///@param utils::IPAddress 
	MacAdress getMACaddress(IPAddress ip);

	///zwraca czas lokalny, format format "YYYY-MM-DD-HH-MM-SS"
	string getTime();

	//	bool operator<(const MacAdress& mac1, const MacAdress& mac2 );

}

#endif 