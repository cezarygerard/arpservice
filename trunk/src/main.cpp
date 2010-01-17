#include "utils.hpp"
#include <iostream>
#include "CNetworkAdapter.hpp"
#include "CDataBaseWrapper.hpp"

using namespace std;

int main(int argc, char *argv[])
{
	
	CNetworkAdapter::getInstance()->init(utils::init());
	CNetworkAdapter::getInstance()->open();
	CDataBaseWrapper::getInstance();
	CNetworkAdapter::getInstance()->sendARPs();
	system("pause");
}
