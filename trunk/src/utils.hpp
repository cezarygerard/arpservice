/*
 * Copyright (c) 1999 - 2005 NetGroup, Politecnico di Torino (Italy)
 * Copyright (c) 2005 - 2006 CACE Technologies, Davis (California)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Politecnico di Torino, CACE Technologies 
 * nor the names of its contributors may be used to endorse or promote 
 * products derived from this software without specific prior written 
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
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

#pragma comment(lib, "iphlpapi.lib")

using namespace std;

/// @namespace utils Przechowuje funkcje i struktury uzywane w kodzie wielu klas

namespace utils
{
	//rozmiar pakietu ARP request
	const int ARP_REQ_SIZE = 60;
	
	struct IPAddress
	{
		unsigned char ip[4];
	};

	struct MacAdress
	{
		unsigned char m[6];
	};

//	struct IPAddress;
//	struct MacAdress;
	//zmiania IP z long int na string
	string iptos(u_long in);

	//zmiania IP ze struktury utils::IPAddress na string
	string iptos(IPAddress in);
	
	string macToS(MacAdress in);

	//wypisanie informaqcji o danym interfejsie, uzywane w utils::init()
	void ifprint(pcap_if_t *d);
	
	//zmiania IP z long int na utils::IPAddress 
	IPAddress iptoIP(u_long in);

	///inicjalizacja programu - wypisanie listy interfejsow, zaczytanie ktory interfejs ma byc uzyty
	pcap_if_t * init();
	
	//wypisuje na standardowe wyjscie pierwsze size bajtow z tablicy data
	void printBytes(unsigned char data[], int size);
	
	//zwraca MAC dla interfejsu o podanym IP
	//@return utils::MacAdress
	//@param utils::IPAddress 
	MacAdress getMACaddress(IPAddress ip);

	string getTime();
//	bool operator<(const MacAdress& mac1, const MacAdress& mac2 );

}

#endif 