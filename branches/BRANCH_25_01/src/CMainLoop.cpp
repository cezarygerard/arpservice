#include "CMainLoop.hpp"

void CMainLoop::initMainLoop()
{

	CGViewer::getInstance();
	CNetworkAdapter::getInstance();
	CDataBaseWrapper::getInstance();
	CConnectionMgr::getInstance();	
	utils::fout.open("../log/log.txt",ios_base::out);
	
}

void CMainLoop::enterMainLoop()
{
	CNetworkAdapter::getInstance()->init(utils::init());
	CNetworkAdapter::getInstance()->open();	

	CGViewer::getInstance()->startCGViewer();

	CConnectionMgr::getInstance()->startListening();
	CDataBaseWrapper::getInstance()->startHandlingReceived();

	while(!quit_)	
	{	
	//string s;
	//cout<<"$>";
	//cin>>s;
	
		boost::this_thread::sleep(boost::posix_time::seconds(interval_));
	}
}

void CMainLoop::endMainLoop()
{
	CDataBaseWrapper::destroyInstance();
	CNetworkAdapter::destroyInstance();
	CConnectionMgr::destroyInstance(); 
	CGViewer::destroyInstance();
	utils::fout.close();
}

void CMainLoop::quitNow()
{
	quit_ = true;
}
