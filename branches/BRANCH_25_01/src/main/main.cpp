#include "utils.hpp"
#include <iostream>
#include "CNetworkAdapter.hpp"
#include "CDataBaseWrapper.hpp"
#include "ActiveHost.hpp"
#include "CConnectionMgr.hpp"
#include <stdio.h>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/xml_iarchive.hpp>
//#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/nvp.hpp>
using namespace std;

int main(int argc, char *argv[])
{	
	CNetworkAdapter::getInstance()->init(utils::init());
	CNetworkAdapter::getInstance()->open();

	struct Buffer
	{
		char buffer_[256];
	};

	bool quit = false;
	ActiveHost ah;
	ah.ip.ip[0] = 192;
	ah.ip.ip[1] = 167;
	ah.ip.ip[2] = 2;
	ah.ip.ip[3] = 3;
	ah.mac.m[0] = 254;
	ah.mac.m[1] = 11;	
	ah.mac.m[2] = 121;
	ah.mac.m[3] = 123;
	ah.mac.m[4] = 0;
	ah.mac.m[5] = 0;
	ah.start = "2010-01-17-20-20-20";
	ah.stop = "2010-01-17-23-20-20";
	ah.ttl=5;
/*	IPAddress ip = utils::sToIp("192.17.8.0");
	MacAdress mc = utils::sToMac("88aaFd00bBcc" );
	CDataBaseWrapper::getInstance()->enqueReceived(ah);
//	CDataBaseWrapper::getInstance()->saveHostToDB(ah);
//	CLog::getInstance()->fout(utils::getTime());
*/
	//CConnectionMgr::
/*	std::ostringstream oss;
	//boost::archive::binary_oarchive oa(oss);
	{
		boost::archive::xml_oarchive oa(oss);
		oa<< BOOST_SERIALIZATION_NVP(ah);
	//	oa<< (ah);
	}
	;
	Buffer b;
	//b.buffer_=oss.str().c_str();
	strcpy(b.buffer_, oss.str().c_str());
	ActiveHost * ah1;// = new ActiveHost();;
	char * c = b.buffer_;
	string s (c);
//				if(s == QUIT)
//					quit = true;
	std::istringstream iss(s);
	{
		boost::archive::xml_iarchive ia(iss);
				//	boost::archive::binary_iarchive ia(iss);
					//ia>>BOOST_SERIALIZATION_NVP(cne);
		try{
		ia >> BOOST_SERIALIZATION_NVP(ah1) ;
		//ia >> (ah1) ;
		}
		catch(std::exception e)
		{
		throw(e);
		}
	}
	ah1;
*/

	CDataBaseWrapper::getInstance();
	CConnectionMgr::getInstance()->startListening();
	CConnectionMgr::getInstance()->connect("192.168.1.7");


	while(!quit)	
	{
	//	CNetworkAdapter::getInstance()->sendARPs();	
		CDataBaseWrapper::getInstance()->handleReceived();
	
		boost::this_thread::sleep(boost::posix_time::seconds(1));

	}
	
	CDataBaseWrapper::destroyInstance();	
	system("pause");
	return 0;
}
