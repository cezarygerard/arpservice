/**
* @author Cezary Zawadka, Sebastian £uczak 
* @date 2010.01.17
* @version 0.9
*
*	
*/

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

	
bool lessMAC::operator()(const MacAdress& m1, const MacAdress& m2)const
{
	for(int i = 0; i<6; i++)
	{
		if(m1.m[i] < m2.m[i])
			return true;
		if(m1.m[i] > m2.m[i])
			return false;
	}	
	return false;
}

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

	void printBytes(unsigned char Data[], int size)
	{
		for(int i=0; i<size; i++)
			printf("%02X",Data[i]);
	}

	MacAdress getMACaddress(IPAddress ip)
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

	string macToS(MacAdress in)
	{
		string s;
		stringstream out;

		for (int i = 0;i<6; i++)
		{
			out << setw(2)<< setfill('0')<<hex <<(int)in.m[i];
		}
		s = out.str();
		return s;
	}

	bool operator<(const MacAdress& mac1, const MacAdress& mac2 )
	{
		return true;
	}

	string getTime()
	{
		//funkcje z winapi
		SYSTEMTIME lpSystemTime = SYSTEMTIME();// = NULL;
		GetLocalTime(&lpSystemTime);
		

//		time_t timeInSeconds = time(NULL);
//		tm* now = localtime( &timeInSeconds );//prawdopodobnie dla linucha lepsza jest localtime_s
		/*	printf(
		  "tm_sec:   %d\n"
		  "tm_min:   %d\n"
		  "tm_hour:  %d\n"
		  "tm_mday:  %d\n"
		  "tm_mon:   %d\n"
		  "tm_year:  %d\n"
		  "tm_wday:  %d\n"
		  "tm_yday:  %d\n"
		  "tm_isdst: %d\n",
		   now->tm_sec,
		   now->tm_min,
		   now->tm_hour,
		   now->tm_mday,
		   now->tm_mon,
		   now->tm_year,
		   now->tm_wday,
		   now->tm_yday,
		   now->tm_isdst
*/

		stringstream out;
		out<<lpSystemTime.wYear<<"-";
		if(lpSystemTime.wMonth<10)
			out<<"0"; //mozna bylo uzyc setw(n)
		out<<lpSystemTime.wMonth<<"-";
		if(lpSystemTime.wDay<10)
			out<<"0";
		out<<lpSystemTime.wDay<<"-";
		if(lpSystemTime.wHour<10)
			out<<"0";
		out<<lpSystemTime.wHour<<"-";
		if(lpSystemTime.wMinute<10)
			out<<"0";
		out<<lpSystemTime.wMinute<<"-";
		if(lpSystemTime.wSecond<10)
			out<<"0";
		out<<lpSystemTime.wSecond;
		
		
		//		out<<  now->tm_year<<"-"<<now->tm_mon<<"-"<< now->tm_mday<<"-"<< now->tm_hour<<"-"<< now->tm_min<<"-"<< now->tm_sec<<endl;
	//	string s = out.str();
		return out.str();
	}

	MacAdress sToMac(string s)
	{
		MacAdress m;
		for (int i = 0; i<6; i++)
		{
			m.m[i] = charToHex(s[2*i]) * 16 + charToHex(s[2*i+1]);
		}

		return m;
	}

	int charToHex(char c)
	{
		//int i = c;
		switch (c)
		{
			case '0': return 0;
			case '1': return 1;
			case '2': return 2;
			case '3': return 3;
			case '4': return 4;
			case '5': return 5;
			case '6': return 6;
			case '7': return 7;
			case '8': return 8;
			case '9': return 9;
			case 'a': return 10;
			case 'A': return 10;
			case 'b': return 11;
			case 'B': return 11;
			case 'c': return 12;
			case 'C': return 12;
			case 'd': return 13;
			case 'D': return 13;
			case 'e': return 14;
			case 'E': return 14;
			case 'f': return 15;
			case 'F': return 15;
			default: return 0;
			}
	}

	IPAddress sToIp(string s)
	{
		IPAddress ip;
		unsigned int j = 0;
		unsigned int v = 0;
		//unsigned int p =0;
		for(unsigned int i =0; i < s.length();i++)
		{
			if(s[i] == '.')
			{
				ip.ip[j++] = v;
			//	p = 0;
				v=0;
			}

			v = v * 10 + charToHex(s[i]);
		}
		return ip;
	}

}