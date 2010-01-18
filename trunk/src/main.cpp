#include "utils.hpp"
#include <iostream>
#include "CNetworkAdapter.hpp"
#include "CDataBaseWrapper.hpp"
#include "ActiveHost.hpp"

using namespace std;

int main(int argc, char *argv[])
{
	
	CNetworkAdapter::getInstance()->init(utils::init());
	CNetworkAdapter::getInstance()->open();
	bool quit = false;
	ActiveHost ah;
	ah.mac.m[0] = 254;
	ah.mac.m[1] = 11;	
	ah.mac.m[2] = 121;
	ah.mac.m[3] = 123;
	ah.mac.m[4] = 214;
	ah.mac.m[5] = 5;
	ah.start = "2010-01-17-20-20-20";
	ah.stop = "2010-01-17-23-20-20";
//	CDataBaseWrapper::getInstance()->enqueReceived(ah);
//	CDataBaseWrapper::getInstance()->saveHostToDB(ah);
//	cout<<utils::getTime()<<endl;

	while(!quit)	
	{
		CNetworkAdapter::getInstance()->sendARPs();	
		CDataBaseWrapper::getInstance()->handleReceived();
		Sleep(100000);
	}
	system("pause");
}
