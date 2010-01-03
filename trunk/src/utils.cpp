#include "utils.hpp"

using namespace std;

/// @namespace utils Przechowuje funkcje i struktury uzywane w kodzie wielu klas

namespace utils
{
/*
	struct IPAddress
	{
		unsigned char ip[4];
	};

	struct MacAdress
	{
		unsigned char m[6];
	};
*/
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

		return d;
	}

	void PrintBytes(unsigned char Data[], int size)
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