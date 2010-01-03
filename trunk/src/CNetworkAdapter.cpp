#include "CNetworkAdapter.hpp"
#include <math.h>

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
	unsigned char arp_req[utils::ARP_REQ_SIZE];
	/* Supposing to be on ethernet, set mac destination to FF:FF:FF:FF:FF:FF"*/
	for (int i=0; i< utils::ARP_REQ_SIZE; i++)
	{
		arp_req[i]=0;
	}
    arp_req[0]=255;
    arp_req[1]=255;
    arp_req[2]=255;
    arp_req[3]=255;
    arp_req[4]=255;
    arp_req[5]=255;
	/* set mac source to current adapter's address */
	arp_req[6]=mac_.m[0];
    arp_req[7]=mac_.m[1];
    arp_req[8]=mac_.m[2];
    arp_req[9]=mac_.m[3];
    arp_req[10]=mac_.m[4];
    arp_req[11]=mac_.m[5];
	//type: arp   
	arp_req[12]=0x8;
	arp_req[13]=0x6;
	// hardware type eth:
	arp_req[14]=0x0;
	arp_req[15]=0x6;
	//  protocol type ip:
	arp_req[16]=0x8;
    arp_req[17]=0x0;
	//hw size:
    arp_req[18]=0x6;
	//prot size:
    arp_req[19]=0x4;
	//opcode: req
	arp_req[20]=0x0;
    arp_req[21]=0x1;
	//sender MAC:
	arp_req[22]=mac_.m[0];
    arp_req[23]=mac_.m[1];
    arp_req[24]=mac_.m[2];
    arp_req[25]=mac_.m[3];
    arp_req[26]=mac_.m[4];
    arp_req[27]=mac_.m[5];
	//sender ip:
	arp_req[28]=ip_.ip[0];
    arp_req[29]=ip_.ip[1];
    arp_req[30]=ip_.ip[2];
    arp_req[31]=ip_.ip[3];
	//target mac:
    arp_req[32]=0x00;
    arp_req[33]=0x00;
	arp_req[34]=0x00;
	arp_req[35]=0x00;
	arp_req[36]=0x00;
	arp_req[37]=0x00;

	while(1)
	{	
		//target ip:
		arp_req[38]=ip_.ip[0] & netMask_.ip[0];
		arp_req[39]=ip_.ip[1] & netMask_.ip[1];
		arp_req[40]=ip_.ip[2] & netMask_.ip[2];
		arp_req[41]=ip_.ip[3] & netMask_.ip[3];
		
		unsigned int o1 = (unsigned int)arp_req[38];
		unsigned int o2 = (unsigned int)arp_req[39];
		unsigned int o3 = (unsigned int)arp_req[40];
		unsigned int o4 = (unsigned int)arp_req[41];
		
		for(unsigned int i = 256 - (255 & netMask_.ip[0]); i>0;i--)
		{
			for(unsigned int j = 256 - (255 & netMask_.ip[1]); j>0;j--)
			{
				for(unsigned int k = 256 - (255 & netMask_.ip[2]); k>0;k--)
				{
					for(unsigned int m = 256 - (255 & netMask_.ip[3]); m>0;m--)
					{
						arp_req[38]=o1;
						arp_req[39]=o2;
						arp_req[40]=o3;
						arp_req[41]=o4;
						o4++;
						if (pcap_sendpacket(fp_, arp_req, utils::ARP_REQ_SIZE ) != 0)
						{
							fprintf(stderr,"\nError sending the packet: \n", pcap_geterr(fp_));
							//return -3;
						}
					//	Sleep(2);
					}
					o3++;
				}
				o2++;
			}
			o1++;
		}
		Sleep(60*1000);
	}
}