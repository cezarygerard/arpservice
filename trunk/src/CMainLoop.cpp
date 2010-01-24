#include "CMainLoop.hpp"

void CMainLoop::initMainLoop()
{
	CNetworkAdapter::getInstance()->init(utils::init());
	CNetworkAdapter::getInstance()->open();	
	
	CDataBaseWrapper::getInstance();
	
	CConnectionMgr::getInstance()->startListening();

	CGUI::getInstance()->startCGUI();
		
}

void CMainLoop::enterMainLoop()
{

	while(!quit_)	
	{	
		CDataBaseWrapper::getInstance()->handleReceived();
		boost::this_thread::sleep(boost::posix_time::seconds(1));
	}
}

void CMainLoop::endMainLoop()
{
	CGUI::destroyInstance();
	CDataBaseWrapper::destroyInstance();
	CNetworkAdapter::destroyInstance();
	CConnectionMgr::destroyInstance(); 
}

void CMainLoop::quitNow()
{
	quit_ = true;
}
