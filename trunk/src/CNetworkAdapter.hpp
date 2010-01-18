/**
* @author Cezary Zawadka, Sebastian £uczak 
* @date 2010.01.17
* @version 0.9
*
*	
*/

#ifndef	CNETWORK_ADAPTER_H
#define CNETWORK_ADAPTER_H

#define HAVE_REMOTE 
#include <pcap.h>
#include "utils.hpp" 
#include "CSingleton.hpp"
#include <boost/thread/mutex.hpp>

//

class CNetworkAdapter : public CSingleton<CNetworkAdapter>
{
	friend CSingleton<CNetworkAdapter>;

public:

	//inicjalizacja klasy na podany interfejs pcap_if_t *d wyciagniety pomoca funkcji utils::init()
	//tutaj pobirane sa najwazniejsze informacje o interfejsie - ip,netmask, broadcast adress i mac
	void init(pcap_if_t *d);

	//otwarcie interfejsu do nadawania i sluchania
	void open();

	//wywolanie tej funkcji powoduje jednokrotne wyslanie ARPow na wszystkie hosty podsieci
	void sendARPs();
	
private:
	//ip uzywanego interfejsu sieciowego
	utils::IPAddress ip_;
	//netmask dla uzywanego interfejsu sieciowego
	utils::IPAddress netMask_;
	//broadcast address uzywanego interfejsu sieciowego
	utils::IPAddress broadcast_;
	//MacAdress uzywanego interfejsu sieciowego
	utils::MacAdress mac_;

	
	static bool initDone_;
	
	//Item in a list of interfaces, w naszym przypadku na tym interfejsie wysylamy/odbieramy
	pcap_if_t *d_;
	
	//Descriptor of an open capture instance. potrzebne w pcap_sendpacket()
	pcap_t *fp_;
	
	//od synchronizacji waktow - nie powinno sie jednoczesnie nadawac i odbierac
	boost::mutex mutex_;
	
	///Konstruktor domyslny
	CNetworkAdapter();
	
	///Destruktor
	~CNetworkAdapter();
};

#endif