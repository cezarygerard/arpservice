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

class CConnectionMgr : public CSingleton<CConnectionMgr>//, public CTimerObserver 
{
	friend class CSingleton<CConnectionMgr>;

public:
	
	void startListening(int port = 20011);

	void connect(std::string ip, int port = 20011);

private:

	CConnectionMgr();
	
	~CConnectionMgr();
	
	void listen(int port);

	void sendInfo(TCPsocket csd_);

	HostsMapPtr receiveInfo(TCPsocket csd_);

	boost::thread listeningThread_;

	bool stopListening_;

	boost::thread sendingThread_;

	boost::thread receivingThread_;

	///obiekt mutex synchronizujacy watki
	boost::mutex mutex;

	std::vector<boost::shared_ptr<boost::thread> > sendingThreads_;

	SDLNet_SocketSet sockSet_;

	const int static MAX_BUFF = 1024;
	const string static QUIT;

	struct Buffer
	{
		char buffer_[MAX_BUFF];
	};

};

#endif