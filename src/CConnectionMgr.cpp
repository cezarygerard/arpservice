/*
* @author Czarek Zawadka
* @date 2010.01.22
* @version 0.9
* @class CNetwork CNetwork.hpp
* @brief CNetwork to klasa odpowiedzialna za komunikacje sieciowa
*
*
*/
#include "CConnectionMgr.hpp"
#include "SDL_net.h"

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
	stopListening_ = false;
	listeningThread_ = boost::thread(boost::bind(&CConnectionMgr::listen, this, port));
}

void CConnectionMgr::connect(std::string ip, int port)
{
	boost::mutex::scoped_lock scoped_lock(mutex);
	IPaddress ip_;
	TCPsocket csd_ ;
	SDLNet_SocketSet sockSet_;
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
		sockSet_=SDLNet_AllocSocketSet(1);
		SDLNet_TCP_AddSocket(sockSet_, csd_);
//		startRec();
//		startSend();
	//	return 1;
	}
	else
		fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());

}

void CConnectionMgr::listen(int port)
{
	boost::mutex::scoped_lock scoped_lock(mutex);
	
	IPaddress ip_;
	IPaddress *remoteIP_;
	TCPsocket sd_ ;
	TCPsocket csd_ ;
	SDLNet_SocketSet sockSet_;
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
	/* Wait for a connection, send data and term */

	int quit = 0;
	int t = 0;
	cout<<"Nasluchiwanie polaczen przychodzacych na porcie: "<<port<<endl;
//	printf("dupa");	
	while (!stopListening_)
	{
		
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

	/*		
		CTimer::getInstance()->delay(10);
		if(((t+=10) > 100*1000))
		{
			quit = true;
			cout<<"Nie podlaczono zadnego klienta, uruchamianie trybu gry pojedynczej... "<<endl;
			CTimer::getInstance()->delay(1000);
			break;
		}
	*/		
	}

//	isClient_ = false;
//~~czesc serwerowa 

	sockSet_=SDLNet_AllocSocketSet(1);
	SDLNet_TCP_AddSocket(sockSet_, csd_);
	
//	return 0;
	
}

void CConnectionMgr::sendInfo(TCPsocket csd_)
{
	boost::mutex::scoped_lock scoped_lock(mutex);
/*	cout<<"CNetwork::send()"<<endl;
	while(!stopSendThread_)
	{
		CTimer::getInstance()->delay(10);
		boost::mutex::scoped_lock scoped_lock(mutex);
		while(!toSend_.empty())
		{
			//int i = toSend_.size();
			cout<<"CNetwork::sendTh()"<<endl;
			Buffer b;
			std::ostringstream oss;
			//boost::archive::xml_oarchive oa(oss);
			boost::archive::text_oarchive oa(oss);
		//	oa<<BOOST_SERIALIZATION_NVP(toSend_.front());
			toSend_;
			oa<<(toSend_.front());
			//b.buffer_=oss.str().c_str();
			strcpy(b.buffer_, oss.str().c_str());
			//const string str =  oss.str();
			int len = sizeof(b.buffer_);
			
			if (SDLNet_TCP_Send(csd_, b.buffer_, len) < len)
			{
				fprintf(stderr, "SDLNet_TCP_Send: %s\n", SDLNet_GetError());
			}
			delete(toSend_.front());
			toSend_.pop();		
		}
*/	
}

void CConnectionMgr::receiveInfo(TCPsocket csd_)
{
	boost::mutex::scoped_lock scoped_lock(mutex);
}