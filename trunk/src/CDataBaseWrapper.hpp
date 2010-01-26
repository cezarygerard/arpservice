/**
* @author Cezary Zawadka 
* @date 2010.01.17
* @version 0.9
*
*	
*/

#ifndef	C_DATA_BASE_WRAPPER_H
#define C_DATA_BASE_WRAPPER_H
//#define HAVE_REMOTE 

//#include <pcap.h>

#include <string>
#include <map>

#include "boost/shared_container_iterator.hpp"
#include "boost/shared_ptr.hpp"

#include <boost/tuple/tuple.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <queue>
#include "../sqlite/sqlite3.h"

#include "utils.hpp" 
#include "CSingleton.hpp"
#include "ActiveHost.hpp"
#include "CGViewer.hpp"

//#include "../sqlite/sqlite3ext.h"
//#include "sqlite3.h"

using namespace std;
using namespace utils;

typedef boost::shared_ptr< map<utils::MacAdress,ActiveHost,utils::lessMAC> > ExternalHostsMapPtr;
typedef boost::shared_container_iterator< map<utils::MacAdress,ActiveHost,utils::lessMAC> > sh_iterator;



///klasa przechowuje aktywne hosty i zapisuje do bazy danych.
class CDataBaseWrapper : public CSingleton<CDataBaseWrapper>
{

	friend class CSingleton<CDataBaseWrapper>;
	friend class CConnectionMgr;


public:

	void handleReceivedInThread();

	void handleReceivedExternal();

	void enqueReceived(ActiveHost& host);

	void enqueReceivedExternal(ExternalHostsMapPtr externalHosts);

	//Laduje wszystkie histroryczne hosty
	void loadAllHosts();

	void startHandlingReceived();

private:

	CDataBaseWrapper();
	
	~CDataBaseWrapper();

	void handleReceived();

	void saveHostToDB(ActiveHost& host);

	//zapisuje wszystkie hosty i ustawia im ttl na -1, powinno byc odpalane prze zamykaniu programu
	void saveAllHosts();

	///kolejka FIFO hostow od ktorych zostaly odebrany ARP, ale jeszcze nie przetworzonych (TTL, baza itp)
	queue <ActiveHost> received_;
	///kolejka FIFO hostow pobranych od klientów, ale jeszcze nie przetworzonych (TTL, baza itp)
	queue <ActiveHost> receivedExternal_;

	///kolejka aktywnych hostow
	std::map<utils::MacAdress,ActiveHost, utils::lessMAC> activeHosts_;
	///kolejka hostow zewnetrznych
	std::map<utils::MacAdress,ActiveHost, utils::lessMAC> externalHosts_;

	///do synchronizacji - zeby na raz kilku nie czytalo/zapisywali
	boost::mutex mutex_;
	///do synchronizacji - zeby na raz kilku nie czytalo/zapisywali
	boost::mutex mutexDatabase_;

	boost::mutex mutexExternal_;

	///Watek odbierania pakietow
	boost::thread handleReceivedThread_;
	///flaga watku
	static bool stopHandleReceivedThread_;

	///wskaŸnik na baze danych z ktora sie komunikujemy
	sqlite3 * database;
};



#endif
