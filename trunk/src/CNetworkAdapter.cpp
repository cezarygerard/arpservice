#include "CNetworkAdapter.hpp"

//#include "utils.hpp"

bool CNetworkAdapter::initDone_=false;

void CNetworkAdapter::init(pcap_if_t *d)
{
	if(!initDone_)
	{
	initDone_=true;
	d_=d;
	}


	
}

///Konstruktor domyslny
CNetworkAdapter::CNetworkAdapter()
{

}

///Destruktor
CNetworkAdapter::~CNetworkAdapter()
{

}