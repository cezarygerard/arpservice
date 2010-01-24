/**
* @author Cezary Zawadka
* @date 2010.01.17
* @version 0.9
*
*	
*/

#include "CDataBaseWrapper.hpp"

bool CDataBaseWrapper::stopHandleReceivedThread_=false;


CDataBaseWrapper::CDataBaseWrapper()
{
	database = NULL;
	if (sqlite3_open("../sqlite/ARPrecord.sqlite", &database) == SQLITE_OK) 
	{
		cout<<"CDataBaseWrapper::CDataBaseWrapper() udalo sie poloczyc z baza"<<endl;
		loadAllHosts();
	}
	else
		cout<<"CDataBaseWrapper::CDataBaseWrapper() nie!!!! udalo sie poloczyc z baza"<<endl;
}

CDataBaseWrapper::~CDataBaseWrapper()
{
	stopHandleReceivedThread_ = true;
	saveAllHosts();
	sqlite3_close(database);
	cout<<"CDataBaseWrapper::~CDataBaseWrapper() zamykanie"<<endl;
}

void CDataBaseWrapper::handleReceived()
{
	
	boost::mutex::scoped_lock scoped_lock(mutex_);

	map<utils::MacAdress,ActiveHost, utils::lessMAC>::iterator it;
	for(it = activeHosts_.begin(); it != activeHosts_.end(); it++ )
	{
		if((*it).second.ttl>0)
		{
			(*it).second.ttl -=1;
		}
		if((*it).second.ttl==0)
		{
			(*it).second.ttl -=1;
			//zapisz info do bazy//
			(*it).second.stop = utils::getTime();
			saveHostToDB((*it).second);
			//activeHosts_.erase(it);
		}
	}

	while (!received_.empty())
	{
		ActiveHost ah = received_.front();
		map<utils::MacAdress,ActiveHost, utils::lessMAC>::iterator it;
		it = activeHosts_.find(ah.mac);
		
		if(it == activeHosts_.end())
		{//hosta nie ma na liscie 
			ah.start = utils::getTime();
			ah.stop = utils::getTime();
			activeHosts_.insert ( pair<utils::MacAdress,ActiveHost>(ah.mac,ah) );

		}
		else
		{//host jest na liscie 
			if((*it).second.ttl <=0)
			{
				(*it).second.start = utils::getTime();
			}
			(*it).second.ttl = utils::MAX_TTL;
			(*it).second.stop = utils::getTime();
			(*it).second.ip = ah.ip;
		}
		
		received_.pop();
		//	utils::MacAdress currentMac = received_.front().mac;
	//	activeHosts_[currentMac].		
	}
	CGUI::getInstance()->refreshCGUIActiveHosts(activeHosts_);
}

void CDataBaseWrapper::handleReceivedInThread()
{
	while(!stopHandleReceivedThread_)
	{
		handleReceived();
		boost::this_thread::sleep(boost::posix_time::seconds(1));
	}
}

//const std::vector<boost::tuple<IPAddress, MacAdress, int>> CDataBaseWrapper::cguiQuery() 
//{
//
//	
//	return v;
//}

void CDataBaseWrapper::enqueReceived(ActiveHost& host)
{
	boost::mutex::scoped_lock scoped_lock(mutex_);
	received_.push(host);
}

void CDataBaseWrapper::saveHostToDB(ActiveHost& host)
{
//	boost::mutex::scoped_lock scoped_lock(mutex_);
	stringstream query;
	sqlite3_stmt *statement;
	query<<"insert into arprecord (mac,ip,start,stop) values ('"<<utils::macToS(host.mac)<<"','"
			<<utils::iptos(host.ip)<<"','"<<host.start<<"','"<<host.stop<<"');";
//	cout<<"CDataBaseWrapper::saveHostToDB:"<<query.str()<<endl;
	sqlite3_prepare_v2(database,query.str().c_str(),-1,&statement, NULL);
	int result = sqlite3_step(statement);
	if(result == SQLITE_DONE)
		cout<<"CDataBaseWrapper::saveHostToDB udalo sie"<<endl;
	else	
		cout<<"CDataBaseWrapper::saveHostToDB fail"<<result<<endl;

	sqlite3_finalize(statement);
}

void CDataBaseWrapper::loadAllHosts()
{
	boost::mutex::scoped_lock scoped_lock(mutex_);
	
	string selectSql = "select * from arprecord group by mac;";
	sqlite3_stmt *statement;
	if (sqlite3_prepare_v2(database, selectSql.c_str(), -1, &statement, NULL) == SQLITE_OK) {
		while (sqlite3_step(statement) == SQLITE_ROW) {
			ActiveHost ah;
			int id = sqlite3_column_int(statement, 0);
			
			char *str = (char *)sqlite3_column_text(statement, 1);
			ah.mac = utils::sToMac(string(str));

			str = (char *)sqlite3_column_text(statement, 2);
			ah.ip = utils::sToIp(string(str));
			
			str = (char *)sqlite3_column_text(statement, 3);
			ah.start = string (str);
			
			str = (char *)sqlite3_column_text(statement, 4);
			ah.stop = string (str);
			ah.ttl = -1;
			activeHosts_.insert( pair<utils::MacAdress,ActiveHost>(ah.mac,ah) );
		}	
	}
}

void CDataBaseWrapper::saveAllHosts()
{
	boost::mutex::scoped_lock scoped_lock(mutex_);

	map<utils::MacAdress,ActiveHost, utils::lessMAC>::iterator it;
	
	for(it = activeHosts_.begin(); it != activeHosts_.end(); it++ )
	{
		if((*it).second.ttl>0)
		{
			(*it).second.ttl = - 1;
			//zapisz info do bazy//
			(*it).second.stop = utils::getTime();
			saveHostToDB((*it).second);
		}	
	}
}

void CDataBaseWrapper::startHandlingReceived()
{

//najpierw trzeba zatrzymac watek, jezeli dziala
stopHandleReceivedThread_ = true;
handleReceivedThread_.join();

stopHandleReceivedThread_ = false;
handleReceivedThread_ = boost::thread(boost::bind(&CDataBaseWrapper::handleReceivedInThread, this));

}
