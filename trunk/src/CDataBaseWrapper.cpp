/**
* @author Cezary Zawadka
* @date 2010.01.17
* @version 0.9
*
*	
*/

#include "CDataBaseWrapper.hpp"


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

CDataBaseWrapper::CDataBaseWrapper()
{
	
	if (sqlite3_open("../sqlite/ARPrecord.sqlite", &database) == SQLITE_OK) 
	{
		cout<<"CDataBaseWrapper::CDataBaseWrapper() udalo sie poloczyc z baza"<<endl;
	}
	else
		cout<<"CDataBaseWrapper::CDataBaseWrapper() nie!!!! udalo sie poloczyc z baza"<<endl;
		
}
CDataBaseWrapper::~CDataBaseWrapper()
{
	sqlite3_close(database);
}
void CDataBaseWrapper::handleReceived()
{
	boost::mutex::scoped_lock scoped_lock(mutex_);

	map<utils::MacAdress,ActiveHost, lessMAC>::iterator it;
	for(it = activeHosts_.begin(); it != activeHosts_.end(); it++ )
	{
		(*it).second.ttl -=1;
		if((*it).second.ttl<=0)
		{
			//zapisz info do bazy//
			ah.stop = utils::getTime();
			saveHostToDB(ah);
			activeHosts_.erase(it);
		}
	}

	while (!received_.empty())
	{
		ActiveHost ah = received_.front();
		map<utils::MacAdress,ActiveHost, lessMAC>::iterator it;
		it = activeHosts_.find(ah.mac);
		
		if(it == activeHosts_.end())
		{//hosta nie ma na liscie aktywnych
			ah.start = utils::getTime();
			ah.stop = utils::getTime();
			activeHosts_.insert ( pair<utils::MacAdress,ActiveHost>(ah.mac,ah) );

		}
		else
		{//host jest na liscie aktywnych
			(*it).second.ttl = 5;
			(*it).second.stop = utils::getTime();
			(*it).second.ip = ah.ip;
		}
		
		received_.pop();
		//	utils::MacAdress currentMac = received_.front().mac;
	//	activeHosts_[currentMac].
	}
}

void CDataBaseWrapper::enqueReceived(ActiveHost& host)
{
	boost::mutex::scoped_lock scoped_lock(mutex_);
	received_.push(host);
}

void CDataBaseWrapper::saveHostToDB(ActiveHost& host)
{
	boost::mutex::scoped_lock scoped_lock(mutex_);
	stringstream query;
	sqlite3_stmt *statement;
	query<<"insert into arprecord (mac,ip,start,stop) values ('"<<utils::macToS(host.mac)<<"','"
			<<utils::iptos(host.ip)<<"','"<<host.start<<"','"<<host.stop<<"');";

	int result = sqlite3_prepare_v2(database,query.str().c_str(),-1,&statement, NULL);
	if(result == SQLITE_OK)
		cout<<"CDataBaseWrapper::saveHostToDB udalo sie"<<endl;
	else	
		cout<<"CDataBaseWrapper::saveHostToDB fail"<<result<<endl;
}