#include "CMainLoop.hpp"

void CMainLoop::initMainLoop()
{
	CNetworkAdapter::getInstance()->init(utils::init());
	CNetworkAdapter::getInstance()->open();	

	CGViewer::getInstance()->startCGViewer();
	
	CDataBaseWrapper::getInstance()->loadAllHosts();
	CConnectionMgr::getInstance()->startListening();
	CDataBaseWrapper::getInstance()->startHandlingReceived();


		
}

void CMainLoop::enterMainLoop()
{

	while(!quit_)	
	{	

		//CDataBaseWrapper::getInstance()->handleReceived();
		boost::this_thread::sleep(boost::posix_time::seconds(interval_));
	}
}

void CMainLoop::endMainLoop()
{
	CDataBaseWrapper::destroyInstance();
	CNetworkAdapter::destroyInstance();
	CConnectionMgr::destroyInstance(); 
	CGViewer::destroyInstance();
}

void CMainLoop::quitNow()
{
	quit_ = true;
}
