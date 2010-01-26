/*
* @author Czarek Zawadka
* @date 2010.01.22
* @version 0.9
* @class CNetwork CNetwork.hpp
* @brief CNetwork to klasa odpowiedzialna za komunikacje sieciowa
*
*@todo nalezy dodac timeouty i rzucac wyjatki przy niepowodzeniach
*/
#include "CConnectionMgr.hpp"


const string CConnectionMgr::QUIT = "quit";

BOOST_CLASS_EXPORT(ActiveHost);

CConnectionMgr::CConnectionMgr()
{
	if (SDLNet_Init() < 0)
	{
		fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
	}
	stopListening_ = false;
	
}

CConnectionMgr::~CConnectionMgr()
{	
	stopListening_ = true;
	listeningThread_.join();
	SDLNet_Quit();
}

void CConnectionMgr::startListening(int port)
{
	stopListening_ = true;
	listeningThread_.join();
	stopListening_ = false;
	listeningThread_ = boost::thread(boost::bind(&CConnectionMgr::listen, this, port));
}

void CConnectionMgr::addIPAddress(std::string ip)
{
	 if(ipSet_.insert(ip).second)
	 {
		cout<<"Adres IP " << ip << " dodano do grupy klientow" << endl;
	 }
	 else
		cout<<"Adres IP " << ip << " jest juz w grupie klientow" << endl;
	
}

void CConnectionMgr::removeIPAddress(std::string ip)
{
	if(ipSet_.erase(ip) == 1)
	 {
		cout<<"Adres IP " << ip << " usunieto z grupy klientow" << endl;
	 }
	 else
		cout<<"Adres IP " << ip << " nie nalezy do grupy klientow" << endl;

}

void CConnectionMgr::startConnections(int port)
{
	stopConnecting_ = true;
	connectingThread_.join();
	stopConnecting_ = false;
	connectingThread_ = boost::thread(boost::bind(&CConnectionMgr::connections, this, port));
}

void CConnectionMgr::showConnections()
{
	cout << "\n****\n* Wykaz klientow:\n*";
	for(set<std::string>::iterator it = ipSet_.begin(); it != ipSet_.end(); ++it)
	{
		cout << "\n* " << *it;
	}
	cout << "\n****" << endl;


}
void CConnectionMgr::stopConnections()
{
	stopConnecting_ = true;
	connectingThread_.join();
}

void CConnectionMgr::connections(int port)
{
	stopListening_ = true;
	listeningThread_.join();

	if(!ipSet_.empty())
	{
		cout << "Nawiazywanie polaczen..." << endl;
		while (!stopConnecting_)
		{
			{
				boost::recursive_mutex::scoped_lock recursive_lock(recursive_mutex);
				for_each(ipSet_.begin(), ipSet_.end(), boost::bind(&CConnectionMgr::connect, this, _1, port));
			}
			CDataBaseWrapper::getInstance()->handleReceivedExternal();
			boost::this_thread::sleep(boost::posix_time::seconds(60));
		}
	}
	else
	{
		cerr << "Brak polaczenia.\nNie wprowadzono zadnego adresu klienta!" << endl;
		stopConnecting_ = true;
		connectingThread_.join();
	}
	startListening();
}

void CConnectionMgr::connect(std::string ip, int port)
{


	boost::recursive_mutex::scoped_lock recursive_lock(recursive_mutex);
	IPaddress ip_;
	TCPsocket csd_ ;
//	SDLNet_SocketSet sockSet_;
	if (SDLNet_ResolveHost(&ip_, ip.c_str(), port) < 0)
	{
		fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
		//exit(EXIT_FAILURE);
	}
  	
	 /* Open a connection with the IP provided (listen on the host's port) */
	if ((csd_ = SDLNet_TCP_Open(&ip_)))
	{
		//jezeli udalo sie otworzyc polaczenie
//		printf("initNetwork() %d   %d", ip_.host, ip_.port);
//		isClient_ = true;
		sockSet_= SDLNet_AllocSocketSet(1);
		SDLNet_TCP_AddSocket(sockSet_, csd_);
		CDataBaseWrapper::getInstance()->enqueReceivedExternal(receiveInfo(csd_));
	}
	else
	{
		fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
	}

}

void CConnectionMgr::listen(int port)
{
	IPaddress ip_;
	IPaddress *remoteIP_;
	TCPsocket sd_ ;
	TCPsocket csd_ ;
//	SDLNet_SocketSet sockSet_;
	
	{
		boost::mutex::scoped_lock scoped_lock(mutex);

		if (SDLNet_ResolveHost(&ip_, NULL, port) < 0)
		{
			fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());

			//exit(EXIT_FAILURE);
		}

		/* Open a connection with the IP provided (listen on the host's port) */
		if (!(sd_ = SDLNet_TCP_Open(&ip_)))
		{
			fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
			
			//exit(EXIT_FAILURE);
		}
	}

	int quit = 0;
	int t = 0;
	utils::fout<<"Nasluchiwanie polaczen przychodzacych na porcie: "<<port<<endl;

	while (!stopListening_)
	{
		boost::mutex::scoped_lock scoped_lock(mutex);
		/* This check the sd if there is a pending connection.
		* If there is one, accept that, and open a new socket for communicating */
		if ((csd_ = SDLNet_TCP_Accept(sd_)))
		{
			/* Get the remote address */
			if ((remoteIP_ = SDLNet_TCP_GetPeerAddress(csd_)))
			{/* Print the address, converting in the host format */
	//			printf("Host connected: %x %d\n", SDLNet_Read32(&remoteIP_->host), SDLNet_Read16(&remoteIP_->port));
				stopListening_ = true;
				SDLNet_TCP_Close(sd_);
				sendingThread_ = boost::thread(boost::bind(&CConnectionMgr::sendInfo, this, csd_));
			}
			else
				fprintf(stderr, "SDLNet_TCP_GetPeerAddress: %s\n", SDLNet_GetError());
		}	

		boost::this_thread::sleep(boost::posix_time::millisec(10));
	}

	SDLNet_TCP_Close(sd_);
	sockSet_=SDLNet_AllocSocketSet(1);
	SDLNet_TCP_AddSocket(sockSet_, csd_);
	
}

void CConnectionMgr::sendInfo(TCPsocket csd_)
{

	boost::mutex::scoped_lock scoped_lock(mutex);
	cout << "\nTransmisja w roli klienta\n";
	utils::fout<<"CNetwork::sendInfo()"<<endl;
	map<utils::MacAdress,ActiveHost, utils::lessMAC>::iterator it;
	map<utils::MacAdress,ActiveHost, utils::lessMAC> * activeHosts = &(CDataBaseWrapper::getInstance()->activeHosts_);
	
	for(it = activeHosts->begin(); it != activeHosts->end(); it++ )
	{
		std::ostringstream oss;
		//boost::archive::binary_oarchive oa(oss);
		ActiveHost * ah = &(it->second);
		{
			try{
			boost::archive::xml_oarchive oa(oss);
			oa<< BOOST_SERIALIZATION_NVP(ah);
			//boost::archive::text_oarchive oa(oss);
			//oa<< (*ah);
			
			}
			catch(std::exception e)
			{
				e;
			}
		}
		Buffer b;

		strcpy_s(b.buffer_, oss.str().c_str());
		
		if (SDLNet_TCP_Send(csd_, b.buffer_, MAX_BUFF) < MAX_BUFF)
		{
			fprintf(stderr, "SDLNet_TCP_Send: %s\n", SDLNet_GetError());
		}
	}


	if (SDLNet_TCP_Send(csd_, QUIT.c_str(), sizeof(QUIT.c_str())) < sizeof(QUIT.c_str()))
	{
		fprintf(stderr, "SDLNet_TCP_Send: %s\n", SDLNet_GetError());
	}
	SDLNet_TCP_Close(csd_);
}

HostsMapPtr CConnectionMgr::receiveInfo(TCPsocket csd_)
{
	HostsMapPtr hostsMap = HostsMapPtr(new std::map<utils::MacAdress, ActiveHost, utils::lessMAC>);
	boost::mutex::scoped_lock scoped_lock(mutex);
	{
		bool quit = false;
		
		while(!quit)
		{
			int numready = SDLNet_CheckSockets(sockSet_,100);
			if (numready == -1) 
			{
				printf("SDLNet_CheckSockets: %s  numready: %d\n", SDLNet_GetError(),numready );
			}
			else if (numready) 
			{
				Buffer b;
				if (SDLNet_TCP_Recv(csd_, &(b.buffer_), MAX_BUFF) > 0)
				{
					ActiveHost * ah;// = new ActiveHost();
					char * c = b.buffer_;
					string s (c);
					
					if(s.substr(0,QUIT.length()) == QUIT)
					{	
						quit = true;
					}
					else
					{
						std::istringstream iss(s);
						{
							try
							{
							boost::archive::xml_iarchive ia(iss);
							ia >> BOOST_SERIALIZATION_NVP(ah) ;

						//	boost::archive::text_iarchive ia(iss);
						//	ia >> (ah) ;
							}
							
							catch (boost::archive::archive_exception e)
							{
								cout<<"CConnectionMgr::receiveInfo nie udalo sie odebrac informacji o hoscie "<<e.code<<endl;
							}
							
							cout<<"CConnectionMgr::receiveInfo "<<utils::iptos(ah->ip)<<endl;
							if(ah->start[0] != '0')
							{
								hostsMap->insert(pair<utils::MacAdress,ActiveHost>(ah->mac,*ah) );
						
							}
						}
					}
				}
			}	
		}

		SDLNet_TCP_Close(csd_);
	}

	return hostsMap;
}