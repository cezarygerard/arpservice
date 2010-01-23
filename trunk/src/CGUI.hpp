/**
* @author Sebastian £uczak
* @date 2010.01.23
* @version 0.5
*
*	
*/

#ifndef	CGUI_H
#define CGUI_H

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include <math.h>
#include <vector>

#include <boost/thread.hpp>
#include <boost/tuple/tuple.hpp>

#include "CDataBaseWrapper.hpp"
#include "CTimerObserver.hpp"
#include "CTimer.hpp"
#include "utils.hpp"

class CGUI : public CSingleton<CGUI>, public CTimerObserver
{
	friend CSingleton<CGUI>;

public:

	//Metoda uruchamia watek odbierania pakietow
	void startCGUI();
	

private:

	SDL_Surface *load_image( std::string filename );
	void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL );
	bool init();
	bool load_files();
	void clean_up();
	void refreshGUI();
	void createTable();

	CGUI() {};
	~CGUI() {};

	static bool stopCGUI_;
	///Watek odbierania pakietow
	boost::thread threadCGUI_;

	SDL_Event event;




};


#endif


