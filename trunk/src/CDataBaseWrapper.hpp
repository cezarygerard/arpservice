#ifndef	C_DATA_BASE_WRAPPER_H
#define C_DATA_BASE_WRAPPER_H
//#define HAVE_REMOTE 

//#include <pcap.h>

#include <string>
#include "utils.hpp" 
#include "CSingleton.hpp"
#include <map>
#include "ActiveHost.hpp"

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

	void handleReceived(ActiveHost& recv);

private:
	CDataBaseWrapper();
	std::map<utils::MacAdress,ActiveHost, lessMAC> activeHosts;

};



#endif