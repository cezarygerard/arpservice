#include "CMainLoop.hpp"

void CMainLoop::initMainLoop()
{
	CNetworkAdapter::getInstance()->init(utils::init());
	CNetworkAdapter::getInstance()->open();	

	CGUI::getInstance()->startCGUI();
	
	CDataBaseWrapper::getInstance()->loadAllHosts();
	CConnectionMgr::getInstance()->startListening();
	CDataBaseWrapper::getInstance()->startHandlingReceived();


		
}

void CMainLoop::enterMainLoop()
{

	while(!quit_)	
	{	
		//CDataBaseWrapper::getInstance()->handleReceived();
		
	}
}

void CMainLoop::endMainLoop()
{
	CDataBaseWrapper::destroyInstance();
	CNetworkAdapter::destroyInstance();
	CConnectionMgr::destroyInstance(); 
	CGUI::destroyInstance();
}

void CMainLoop::quitNow()
{
	quit_ = true;
}
