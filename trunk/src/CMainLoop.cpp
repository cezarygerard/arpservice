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
		boost::this_thread::sleep(boost::posix_time::millisec(1000));
	}
}

void CMainLoop::endMainLoop()
{

	CDataBaseWrapper::destroyInstance();
	CNetworkAdapter::destroyInstance();
	CGUI::getInstance()->stopCGUI();
	CGUI::destroyInstance();
}

void CMainLoop::quitNow()
{
	quit_ = true;
}