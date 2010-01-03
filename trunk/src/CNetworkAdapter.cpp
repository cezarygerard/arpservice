#include "CNetworkAdapter.hpp"


bool CNetworkAdapter::initDone_=false;

///Konstruktor domyslny
CNetworkAdapter::CNetworkAdapter()
{

}

///Destruktor
CNetworkAdapter::~CNetworkAdapter()
{
	initDone_=false;
}

void CNetworkAdapter::init(pcap_if_t *d)
{
	if(!initDone_)
	{
		initDone_=true;
		d_=d;
		pcap_addr_t *a;
		a=d->addresses;
		for(a=d->addresses;a;a=a->next)
		{
			switch(a->addr->sa_family)
			{
				case AF_INET:
					if (a->addr)
						ip_= utils::iptoIP(((struct sockaddr_in *)a->addr)->sin_addr.s_addr);
					if (a->netmask)
						netMask_ = utils::iptoIP(((struct sockaddr_in *)a->netmask)->sin_addr.s_addr);
					if (a->broadaddr)
						broadcast_ = utils::iptoIP(((struct sockaddr_in *)a->broadaddr)->sin_addr.s_addr);
					break;
				default:
					break; 
			}
		}
		mac_=utils::GetMACaddress(ip_);
	}
}

void CNetworkAdapter::open()
{
	char errbuf[PCAP_ERRBUF_SIZE];
	if(initDone_)
	{
	    if ( (fp_= pcap_open(d_->name,            // name of the device
                        1600,                // portion of the packet to capture (only the first 100 bytes)
                        PCAP_OPENFLAG_PROMISCUOUS,  // promiscuous mode
                        1000,               // read timeout
                        NULL,               // authentication on the remote machine
                        errbuf              // error buffer
                        ) ) == NULL)
		{
			fprintf(stderr,"\nUnable to open the adapter. It is not supported by WinPcap\n");
		   //return -2;
		}
	}
}

void CNetworkAdapter::sendARPs()
{
	u_char arp_req[utils::ARP_REQ_SIZE];
	int i=42;
	int j=53;
	cout<<"bionarny and: "<< (unsigned int)(i & j) <<" koniec"<<endl;
	utils::PrintBytes(ip_.ip,4);
	std::cout<<"\n";
	utils::PrintBytes(netMask_.ip,4);
	std::cout<<"\n";
	utils::PrintBytes(broadcast_.ip,4);
	std::cout<<"\n";
	utils::PrintBytes(mac_.m,6);
}