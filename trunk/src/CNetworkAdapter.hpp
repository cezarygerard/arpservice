<<<<<<< .mine
#ifndef	CNETWORK_ADAPTER_H
#define CNETWORK_ADAPTER_H

#define HAVE_REMOTE 
#include "pcap.h"
#include "utils.hpp" //tu jest jakis duzy problem
#include "CSingleton.hpp"


//

class CNetworkAdapter : public CSingleton<CNetworkAdapter>
{
	friend CSingleton<CNetworkAdapter>;

public:
	void init(pcap_if_t *d);

private:

	static bool initDone_;
	pcap_if_t *d_;
	///Konstruktor domyslny
	CNetworkAdapter();
	///Destruktor
	~CNetworkAdapter();
};

#endif=======
#ifndef	CNETWORK_ADAPTER_H
#define CNETWORK_ADAPTER_H

#define HAVE_REMOTE 
#include "pcap.h"
//#include "utils.hpp" //tu jest jakis duzy problem
#include "CSingleton.hpp"



class CNetworkAdapter : public CSingleton<CNetworkAdapter>
{
	friend CSingleton<CNetworkAdapter>;

public:
	void init(pcap_if_t *d);

private:

	static bool initDone_;
	pcap_if_t *d_;
	///Konstruktor domyslny
	CNetworkAdapter();
	///Destruktor
	~CNetworkAdapter();
};

#endif>>>>>>> .r5
