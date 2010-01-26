/*
* @author Czarek Zawadka
* @date 2010.01.22
* @version 0.9
* @class CNetwork CNetwork.hpp
* @brief CNetwork to klasa odpowiedzialna za komunikacje sieciowa
*
*
*/
#ifndef CONNECTION_MGR_H
#define CONNECTION_MGR_H

#include <vector>
#include <set>
#include <algorithm>
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include "SDL_net.h"
#include <boost/bind.hpp>
#include "CSingleton.hpp"
#include <string>
#include <boost/thread/mutex.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp> 
#include "utils.hpp"
#include "ActiveHost.hpp"
#include "CDataBaseWrapper.hpp"
#include <boost/shared_ptr.hpp>
using namespace std;

class CDataBaseWrapper;
class ActiveHost;
struct lessMAC;

typedef boost::shared_ptr< map<utils::MacAdress,ActiveHost,utils::lessMAC> > HostsMapPtr;

class CConnectionMgr : public CSingleton<CConnectionMgr>
{
	friend class CSingleton<CConnectionMgr>;

public:
	
	void addIPAddress(std::string ip);
	void removeIPAddress(std::string ip);

	void startListening(int port = 20011);

	void startConnections(int port = 20011);
	void stopConnections();
	
	void showConnections();
	

private:

	CConnectionMgr();
	
	~CConnectionMgr();
	// metoda zwiazana z watkiem nasluchiwania
	void listen(int port);
	// metoda zwiazana z watkiem polaczen
	void connections(int port);
	// laczenie
	void connect(std::string ip, int port);
	// wysylanie danych od siebie
	void sendInfo(TCPsocket csd_);
	// odbieranie danych do mapy
	HostsMapPtr receiveInfo(TCPsocket csd_);
	// watek nasluchiwania
	boost::thread listeningThread_;
	// flaga watku
	bool stopListening_;
	// watek cyklicznego laczenia
	boost::thread connectingThread_;
	// flaga watku
	bool stopConnecting_;
	// watek wysylania
	boost::thread sendingThread_;
	// watek odbierania
	//boost::thread receivingThread_;
	///obiekt mutex synchronizujacy watki
	boost::mutex mutex;
	///obiekt mutex synchronizujacy watki z zagnie¿d¿eniem rekursywnym
	boost::recursive_mutex recursive_mutex;

	boost::shared_mutex shared_mutex;

	std::vector<boost::shared_ptr<boost::thread> > sendingThreads_;

	SDLNet_SocketSet sockSet_;

	const int static MAX_BUFF = 1024;
	const string static QUIT;

	std::set<std::string> ipSet_; 

	struct Buffer
	{
		char buffer_[MAX_BUFF];
	};

};

#endif