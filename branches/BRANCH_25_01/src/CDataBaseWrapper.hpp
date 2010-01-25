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




///klasa przechowuje aktywne hosty i zapisuje do bazy danych.
class CDataBaseWrapper : public CSingleton<CDataBaseWrapper>
{

	friend class CSingleton<CDataBaseWrapper>;
	friend class CConnectionMgr;


public:

	void handleReceived();

	void handleReceivedInThread();

	void enqueReceived(ActiveHost& host);

	void saveHostToDB(ActiveHost& host);

	//Laduje wszystkie histroryczne hosty
	void loadAllHosts();

	void startHandlingReceived();

private:

	CDataBaseWrapper();
	
	~CDataBaseWrapper();

	//zapisuje wszystkie hosty i ustawia im ttl na -1, powinno byc odpalane prze zamykaniu programu
	void saveAllHosts();

	///kolejka FIFO hostow od ktorych zostaly odebrany ARP, ale jeszcze nie 
	queue <ActiveHost> received_;

	///kolejka aktywnych hostow
	std::map<utils::MacAdress,ActiveHost, utils::lessMAC> activeHosts_;

	///do synchronizacji - zeby na raz kilku nie czytalo/zapisywali
	boost::mutex mutex_;

	static bool stopHandleReceivedThread_;

	///Watek odbierania pakietow
	boost::thread handleReceivedThread_;

	///wskaünik na baze danych z ktora sie komunikujemy
	sqlite3 * database;
};



#endif
