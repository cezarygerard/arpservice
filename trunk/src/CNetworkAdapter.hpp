
#ifndef	CNETWORK_ADAPTER_H
#define CNETWORK_ADAPTER_H

#define HAVE_REMOTE 
#include <pcap.h>
#include "utils.hpp" 
#include "CSingleton.hpp"


//

class CNetworkAdapter : public CSingleton<CNetworkAdapter>
{
	friend CSingleton<CNetworkAdapter>;

public:
	void init(pcap_if_t *d);
	void open();
	void sendARPs();
	
private:
	utils::IPAddress ip_;
	utils::IPAddress netMask_;
	utils::IPAddress broadcast_;
	utils::MacAdress mac_;
	static bool initDone_;
	pcap_if_t *d_;
	pcap_t *fp_;
	///Konstruktor domyslny
	CNetworkAdapter();
	///Destruktor
	~CNetworkAdapter();
};

#endif