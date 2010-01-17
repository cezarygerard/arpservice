#ifndef	C_DATA_BASE_WRAPPER_H
#define C_DATA_BASE_WRAPPER_H
//#define HAVE_REMOTE 

//#include <pcap.h>

#include <string>
#include "utils.hpp" 
#include "CSingleton.hpp"
#include <map>
#include "ActiveHost.hpp"
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <queue>
#include "../sqlite/sqlite3.h"
//#include "../sqlite/sqlite3ext.h"
//#include "sqlite3.h"

using namespace std;
using namespace utils;

//struct ActiveHost;

struct lessMAC// : public binary_function<MacAdress, MacAdress, bool>
{	
	///funkcja wywolywana przez set< boost::shared_ptr<CEntity>, lessSharedPtr> dla poronwywania shared_ptr
	bool operator()(const MacAdress& m1, const MacAdress& m2) const;
};

class CDataBaseWrapper : public CSingleton<CDataBaseWrapper>
{
	friend CSingleton<CDataBaseWrapper>;

public:

	void handleReceived();
	void enqueReceived(ActiveHost& recv);

private:

	CDataBaseWrapper();

	queue <ActiveHost> received_;

	std::map<utils::MacAdress,ActiveHost, lessMAC> activeHosts_;

	boost::mutex mutex_;

	sqlite3 * database;
};



#endif