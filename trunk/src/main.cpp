#include "utils.hpp"
#include <iostream>
#include "CNetworkAdapter.hpp"


using namespace std;

int main(int argc, char *argv[])
{
	CNetworkAdapter::getInstance()->init(utils::init());
	CNetworkAdapter::getInstance()->sendARPs();
	system("pause");
}
