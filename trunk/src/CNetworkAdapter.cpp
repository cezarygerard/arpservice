/**
* @author Cezary Zawadka 
* @date 2010.01.17
* @version 0.9
*
*	
*/

#include "CNetworkAdapter.hpp"
#include <math.h>

bool CNetworkAdapter::initDone_=false;
bool CNetworkAdapter::stopCaptureARPs_ = false;
bool CNetworkAdapter::stopSendARPs_ = false;
///Konstruktor domyslny
CNetworkAdapter::CNetworkAdapter()
{
	cout<<"CNetworkAdapter::CNetworkAdapter() tworzenie"<<endl;
}

///Destruktor
CNetworkAdapter::~CNetworkAdapter()
{
	cout<<"CNetworkAdapter::~CNetworkAdapter() zamykanie"<<endl;
	stopCaptureARPs_=true;
	captureARPsThread_.join();
	stopSendARPs_ = true;
	sendARPsThread_.join();
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
		mac_=utils::getMACaddress(ip_);
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

	   /* start the capture */
	  startSendingARPs();
	  startCapturingARPs();


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
	
		while(!stopSendARPs_)
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
			
			int arpCount = ( 256 - (255 & netMask_.ip[0])) 
							* (256 - (255 & netMask_.ip[1]))
							* (256 - (255 & netMask_.ip[2]))
							* (256 - (255 & netMask_.ip[3]));
			int sleepPeriod = int(ARP_SEND_PERIOD*1000000/arpCount);
			if(sleepPeriod < 1)
				sleepPeriod =1;

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
							boost::mutex::scoped_lock scoped_lock(mutex_);

							if (pcap_sendpacket(fp_, arp_req, utils::ARP_REQ_SIZE ) != 0)
							{
								fprintf(stderr,"\nError sending the packet: \n", pcap_geterr(fp_));
								//return -3;
							}
							if(stopSendARPs_)
								return;
							
							boost::this_thread::sleep(boost::posix_time::microsec(sleepPeriod));
						//	Sleep(2);
						}
						o3++;
					}
					o2++;
				}
				o1++;
			}
			//boost::this_thread::sleep(boost::posix_time::seconds(ARP_SEND_PERIOD));
		}
	//	Sleep(60*1000);
//	}
}
void CNetworkAdapter::startCapturingARPs()
{

	//najpierw trzeba zatrzymac watek, jezeli dziala
	stopCaptureARPs_ = true;
	captureARPsThread_.join();

	stopCaptureARPs_ = false;
	captureARPsThread_ = boost::thread(boost::bind(&CNetworkAdapter::captureARPs, this));

}

void CNetworkAdapter::startSendingARPs()
{

	stopSendARPs_ = true;
	sendARPsThread_ .join();

	stopSendARPs_ = false;
	sendARPsThread_ = boost::thread(boost::bind(&CNetworkAdapter::sendARPs, this));

}

void CNetworkAdapter::captureARPs()
{
	struct bpf_program fcode;
	u_int netmask;

	string s = "arp dst host " + utils::iptos(ip_);
	char* packet_filter = const_cast<char*>(s.c_str());
	
	cout << packet_filter << endl;

	if (d_->addresses != NULL)
        /* Retrieve the mask of the first address of the interface */
		netmask=((struct sockaddr_in *)(d_->addresses->netmask))->sin_addr.S_un.S_addr;
    else
        /* If the interface is without an address we suppose to be in a C class network */
        netmask=0xffffff; 


	// compile the filter
	 if (pcap_compile(fp_, &fcode, packet_filter, 1, netmask) < 0)
    {
		cerr << "CNetworkAdapter::captureARPs: Nie udalo sie skompilowac filtra, sprawdz skladnie!!" << endl;
        /* Free the device list */
		stopCaptureARPs_ = true;
    }
    
	//set the filter
	if (pcap_setfilter(fp_, &fcode) < 0)
    {
        cerr << "CNetworkAdapter::captureARPs: Blad przy ustawianiu filtra!!" << endl;
		stopCaptureARPs_ = true;
    }

	int res;
	ActiveHost ah;
	struct pcap_pkthdr *header;
	const u_char *pkt_data;
	utils::ARPHeader *arp_h;

	cout << "CNetworkAdapter::captureARPs: rozpoczeto watek captureARPsThread" << endl;
	while(!stopCaptureARPs_)
	{
		
			/* Retrieve the packets */
			while((res = pcap_next_ex( fp_, &header, &pkt_data)) >= 0 && !stopCaptureARPs_){
		        
				if(res == 0)
					/* Timeout elapsed */
					continue;

				arp_h = (utils::ARPHeader *)(pkt_data + 14);
				
				// jezeli dany arp jest odpowiedzia, obsluz
				if(arp_h->oper == 512) 
				{
					cout << endl;
					cout << utils::iptos(arp_h->spa) << endl;
					cout << utils::macToS(arp_h->sha) << endl;

					ah.ip = arp_h->spa;
					ah.mac = arp_h->sha;
					CDataBaseWrapper::getInstance()->enqueReceived(ah);
				}
			}
		    
			if(res == -1){
				cerr << "CNetworkAdapter::captureARPs: Blad przy odbieraniu pakietu!!" << endl;
				stopCaptureARPs_ = true;
			}
		
			boost::this_thread::sleep(boost::posix_time::millisec(1));
	}
}

std::pair<utils::IPAddress, utils::MacAdress> CNetworkAdapter::getIPandMac() const
{
	return std::make_pair(ip_, mac_);
}
