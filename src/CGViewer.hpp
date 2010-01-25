/**
* @author Sebastian £uczak
* @date 2010.01.23
* @version 0.5
*
*	
*/

#ifndef	CGViewer_H
#define CGViewer_H

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include <math.h>
#include <vector>
#include <sstream>


#include <boost/smart_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/tuple/tuple.hpp>

#include "CMainLoop.hpp"
#include "CSingleton.hpp"	
#include "CDataBaseWrapper.hpp"
#include "CNetworkAdapter.hpp"

#include "utils.hpp"

using namespace utils;

class CGViewer : public CSingleton<CGViewer>
{
	friend CSingleton<CGViewer>;

public:

	//Metoda uruchamia watek odbierania pakietow
	void startCGViewer();
	void stopCGViewer();
	void refreshCGViewerActiveHosts(std::map<utils::MacAdress,ActiveHost, utils::lessMAC>& m);
	

private:

	boost::shared_ptr<SDL_Surface> loadImage(const std::string& filename);
	boost::shared_ptr<TTF_Font> openFont(const std::string& fileName, int size);

	void applySurface( int x, int y,  boost::shared_ptr<SDL_Surface> source,  SDL_Surface* destination, SDL_Rect* clip = NULL );
	bool init();
	bool loadFiles();
	void cleanUp();
	void refreshGUI();
	void createTable();

	CGViewer();
	~CGViewer();

	std::vector<boost::tuple<utils::IPAddress, utils::MacAdress, int>> activeHosts_;

	///do synchronizacji - zeby na raz kilku nie czytalo/zapisywali
	boost::shared_mutex mutex_;

	static bool stopCGViewer_;
	///Watek odbierania pakietow
	boost::thread threadCGViewer_;

	SDL_Event event;




};


#endif


