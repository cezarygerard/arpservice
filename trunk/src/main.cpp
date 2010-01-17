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
	ah.mac.m[0] = 1;
	ah.mac.m[1] = 1;	
	ah.mac.m[2] = 1;
	ah.mac.m[3] = 1;
	ah.mac.m[4] = 1;
	ah.mac.m[5] = 1;
	CDataBaseWrapper::getInstance()->enqueReceived(ah);
	cout<<utils::getTime()<<endl;
	while(!quit)	
	{
		CNetworkAdapter::getInstance()->sendARPs();	
		CDataBaseWrapper::getInstance()->handleReceived();
		Sleep(100000);
	}
	system("pause");
}
