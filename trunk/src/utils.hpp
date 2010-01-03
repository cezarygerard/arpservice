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


#define HAVE_REMOTE //bez tego kilka rzeczy nie dziala
//#include <RpcDce.h>
#include <stdio.h>
#include "pcap.h"
#include <winsock.h>
#include <Windows.h>
#include <Iphlpapi.h>
#include <Assert.h>
#include <string>
#include <sstream>
#pragma comment(lib, "iphlpapi.lib")

using namespace std;

/// @namespace utils Przechowuje funkcje i struktury uzywane w kodzie wielu klas

namespace utils
{

	struct IPAddress
	{
		unsigned char ip[4];
	};

	struct MacAdress
	{
		unsigned char m[6];
	};

	string iptos(u_long in);
	string iptos(IPAddress in);
	void ifprint(pcap_if_t *d);
	IPAddress iptoIP(u_long in);

	pcap_if_t * init()
	{
	  pcap_if_t *alldevs;
	  pcap_if_t *d;
	  char errbuf[PCAP_ERRBUF_SIZE+1];
	  char source[PCAP_ERRBUF_SIZE+1];
	
	  strcpy_s( source, "rpcap://");
	  source[PCAP_ERRBUF_SIZE] = '\0';

	  /* Retrieve the interfaces list */
	  if (pcap_findalldevs_ex(source, NULL, &alldevs, errbuf) == -1)
	  {
		fprintf(stderr,"Error in pcap_findalldevs: %s\n",errbuf);
		exit(1);
	  }

	  /* Scan the list printing every entry */
	  for(d=alldevs;d;d=d->next)
	  {
		ifprint(d);
	  }
	
		int inter;
		cout<<"podaj numwer interfejsu ktory chcesz uzyc\n>";
		cin>>inter;
		d=alldevs;
		for (int i=1; i<inter; i++)
			d=d->next;

//	  pcap_freealldevs(alldevs);
	  
	  return d;
	}

	static void PrintBytes(unsigned char Data[], int size)
	{
		for(int i=0; i<size; i++)
			printf("%02X",Data[i]);
	}

	MacAdress GetMACaddress(IPAddress ip)
	{
		IP_ADAPTER_INFO AdapterInfo[16];			// Allocate information for up to 16 NICs
		DWORD dwBufLen = sizeof(AdapterInfo);		// Save the memory size of buffer
		DWORD dwStatus = GetAdaptersInfo(			// Call GetAdapterInfo
			AdapterInfo,							// [out] buffer to receive data
			&dwBufLen);								// [in] size of receive data buffer
		assert(dwStatus == ERROR_SUCCESS);			// Verify return value is valid, no buffer overflow
		PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;// Contains pointer to current adapter info
		MacAdress m;

		do {
			
			if((iptos(ip)).compare((pAdapterInfo->IpAddressList.IpAddress.String))==0)
			{
				for(int i=0; i<6; i++)
				{
					m.m[i] = pAdapterInfo->Address[i];
				}				
				return m;
			}
			pAdapterInfo = pAdapterInfo->Next;		// Progress through linked list
		}

		while(pAdapterInfo);						// Terminate if last adapter
		for(int i=0; i<6; i++)
			m.m[i] = 0;

		return m;
	}

/* Print all the available information on the given interface */
	void ifprint(pcap_if_t *d)
	{
	  pcap_addr_t *a;

	  /* Name */
	  printf("%s\n",d->name);

	  /* Description */
	  if (d->description)
		printf("\tDescription: %s\n",d->description);

	  /* IP addresses */
	  for(a=d->addresses;a;a=a->next) {
	//	printf("\tAddress Family: #%d\n",a->addr->sa_family);
	  
		switch(a->addr->sa_family)
		{
		  case AF_INET:
			if (a->addr)
				cout<<"\tAddress:"<<iptos(((struct sockaddr_in *)a->addr)->sin_addr.s_addr)<<endl;
			if (a->netmask)
				cout<<"\tNetmask:"<<iptos(((struct sockaddr_in *)a->netmask)->sin_addr.s_addr)<<endl;
			if (a->broadaddr)
				cout<<"\tBroadcast Address:"<<iptos(((struct sockaddr_in *)a->broadaddr)->sin_addr.s_addr)<<endl;
			if (a->dstaddr)
				 cout<<"\tDestination Address:"<<iptos(((struct sockaddr_in *)a->dstaddr)->sin_addr.s_addr)<<endl;
			break;

		  default:
			break; 
		}
	  }
	  printf("\n");
	}

	/* From tcptraceroute, convert a numeric IP address to a string */
	#define IPTOSBUFFERS    12
	string iptos(u_long in)
	{
		static char output[IPTOSBUFFERS][3*4+3+1];
		u_char *p;
		p = (u_char *)&in;
		string s;
		stringstream out;
		for (int i = 0;i<4; i++)
		{	out <<(int)p[i];
			if(i<3)
				out<<'.';
		}
		s = out.str();
		return s;
	}

	IPAddress iptoIP(u_long in)
	{
		static char output[IPTOSBUFFERS][3*4+3+1];
		u_char *p;
		p = (u_char *)&in;
		IPAddress ip;
		for (int i = 0;i<4; i++)
		{	
			ip.ip[i] = p[i];
		}	
		return ip;
	}

	string iptos(IPAddress in)
	{
		string s;
		stringstream out;
		for (int i = 0;i<4; i++)
		{	out <<(int)in.ip[i];
			if(i<3)
				out<<'.';
		}
		s = out.str();
		return s;
	}
}

#endif 