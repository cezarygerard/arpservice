#include "CDataBaseWrapper.hpp"

CDataBaseWrapper::CDataBaseWrapper()
{

}

void CDataBaseWrapper::handleReceived(ActiveHost& recv)
{

}

bool lessMAC::operator()(const MacAdress& m1, const MacAdress& m2)const
{
	for(int i = 0; i<6; i++)
	{
		if(m1.m[i] < m2.m[i])
			return true;
		if(m1.m[i] > m2.m[i])
			return false;
	}	
	return false;
}
