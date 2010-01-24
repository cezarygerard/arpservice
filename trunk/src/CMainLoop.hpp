/**
* @author Sebastian £uczak
* @date 2010.01.23
* @version 0.5
*
*	
*/

#ifndef	CMAINLOOP_H
#define CMAINLOOP_H

#include <boost/thread.hpp>

#include "utils.hpp"
#include <iostream>
#include "CNetworkAdapter.hpp"
#include "CConnectionMgr.hpp"
#include "CDataBaseWrapper.hpp"
#include "ActiveHost.hpp"
#include "CGViewer.hpp"


class CMainLoop : public CSingleton<CMainLoop>
{
	friend CSingleton<CMainLoop>;

public:

	void initMainLoop();
	void enterMainLoop();
	void quitNow();
	void endMainLoop();
	

private:

	bool quit_;
	int interval_;
	CMainLoop(): quit_(false), interval_(1) {};
	~CMainLoop() {};

};


#endif


