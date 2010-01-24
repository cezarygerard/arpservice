/**
* @author Sebastian £uczak
* @date 2010.01.23
* @version 0.5
*
*	
*/
#include "CGUI.hpp"

bool CGUI::stopCGUI_ = false;

//Screen attributes
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 800;
const int SCREEN_BPP = 32;

//The frame rate
const int FRAMES_PER_SECOND = 20;

SDL_Surface* screen;  //boost::shared_ptr<SDL_Surface>

boost::shared_ptr<SDL_Surface> background;
boost::shared_ptr<SDL_Surface> active;
boost::shared_ptr<SDL_Surface> inactive;
boost::shared_ptr<SDL_Surface> message; 

//The font that's going to be used 
boost::shared_ptr<TTF_Font> font; 
boost::shared_ptr<TTF_Font> font_large;

//The color of the font 
SDL_Color textColor = { 255, 255, 255 }; 

void SafeFreeSurface(SDL_Surface* surface)
{
		// boost::shared_ptr wywoluje podany przez uzytkownika destruktor
		// nawet, gdy przechowywany wskaznik nie jest prawidlowy
	if (surface)
		SDL_FreeSurface(surface);
}

void SafeCloseFont(TTF_Font* font)
{
    if (font)
        TTF_CloseFont(font);
}


boost::shared_ptr<SDL_Surface> CGUI::loadImage(const std::string& filename)
{
    boost::shared_ptr<SDL_Surface> loadedImage(
            IMG_Load(filename.c_str()),
            boost::bind(&SafeFreeSurface, _1));
    if (!loadedImage.get())
        throw std::runtime_error(IMG_GetError());

	//dostosowanie grafiki do wyswietlenia z kanalem przezroczystosci
    boost::shared_ptr<SDL_Surface> optimizedImage( 
			SDL_DisplayFormatAlpha(loadedImage.get()), 
			boost::bind(&SafeFreeSurface, _1) ); 

    return optimizedImage;
}

boost::shared_ptr<TTF_Font> CGUI::openFont(const std::string& fileName, int size)
{
    boost::shared_ptr<TTF_Font> font(
            TTF_OpenFont(fileName.c_str(), size),
            boost::bind(&SafeCloseFont, _1));
    if (!font.get())
        throw std::runtime_error(TTF_GetError());
    return font;
}


void CGUI::applySurface( int x, int y,  boost::shared_ptr<SDL_Surface> source, SDL_Surface* destination, SDL_Rect* clip )
{
    //Holds offsets
    SDL_Rect offset;

    //Get offsets
    offset.x = x;
    offset.y = y;

    //Blit
    SDL_BlitSurface( source.get(), clip, destination, &offset );
}

bool CGUI::init()
{
    //Initialize all SDL subsystems
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return false;
    }

    //Set up the screen
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );//boost::shared_ptr<SDL_Surface>(SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE ), boost::bind(&SafeFreeSurface, _1) ) ;   

	//If there was an error in setting up the screen
    if( screen == NULL )
    {
        return false;
    }

	//Initialize SDL_ttf 
	if( TTF_Init() == -1 ) 
	{ 
		return false; 
	} 

    //Set the window caption
    SDL_WM_SetCaption( "LAN StatusViewer", NULL );

    //If everything initialized fine
    return true;
}


bool CGUI::loadFiles()
{
    background = loadImage( "../res/background.png" );
    if( background.get() == NULL )
    {
        return false;
    }

	active = loadImage( "../res/active.png" );
    if( active.get() == NULL )
    {
        return false;
    }

	inactive = loadImage( "../res/inactive.png" );
    if( inactive.get() == NULL )
    {
        return false;
    }

	//Open the font 
	font = openFont( "../res/verdana.ttf", 12 ); 

	font_large = openFont("../res/verdana.ttf", 24 );
	//If there was an error in loading the font 
	if( font.get() == NULL || font_large.get() == NULL ) 
	{ 
		return false; 
	} 

    return true;
}

void CGUI::cleanUp()
{
    //Quit SDL
    SDL_Quit();
	stopCGUI_ = true;
}

void CGUI::refreshGUI()
{
	while(!stopCGUI_)
	{

		//Initialize
		if( init() == false )
		{
			stopCGUI_ = true;
		}

		//Load the files
		if( loadFiles() == false )
		{
			stopCGUI_ = true;
		}

		//While the user hasn't quit
		while( !stopCGUI_ )
		{

			//While there's events to handle
			while( SDL_PollEvent( &event ) )
			{
				//If the user has Xed out the window
				if( event.type == SDL_QUIT )
				{
					//Quit the program
					stopCGUI_ = true;
					CMainLoop::getInstance()->quitNow();
				}
			}

			//Get the keystates
			Uint8 *keystates = SDL_GetKeyState( NULL );

			//Nacisniecie ENTER
			if( keystates[ SDLK_RETURN ] )
			{
				//CMainLoop::
				
			}

			createTable();

			//Update the screen
			if( SDL_Flip( screen ) == -1 )
			{
				stopCGUI_ = true;
			}

			//Cap the frame rate
			boost::this_thread::sleep(boost::posix_time::millisec(1000/FRAMES_PER_SECOND));
		}

		cleanUp();
	}

}


void CGUI::createTable()
{
	boost::shared_lock<boost::shared_mutex> lock(mutex_);

	int i = 0;
	int num_of_hosts = activeHosts_.size();

	int max_x = static_cast<int>(sqrt(static_cast<double>(num_of_hosts))+1)%9;
	int max_y = static_cast<int>(sqrt(static_cast<double>(num_of_hosts))+1)%7;
	
	int start_off_x, start_off_y;

	start_off_x = (SCREEN_WIDTH - max_x*120)/2;
	start_off_y = (SCREEN_HEIGHT + 100 - max_y*120)/2;

	applySurface( 0, 0, background, screen );

	message = boost::shared_ptr<SDL_Surface>(TTF_RenderText_Solid( font_large.get(), "Monitor sieci LAN", textColor ), boost::bind(&SafeFreeSurface, _1) ) ;   
	applySurface( SCREEN_WIDTH/2-100, 60, message, screen );

	//Uspokoj uzytkownika tekstem o oczekiwaniu
	if(num_of_hosts == 0)
	{
		message = boost::shared_ptr<SDL_Surface>(TTF_RenderText_Solid( font.get(), "Kompletowanie danych...", textColor ), boost::bind(&SafeFreeSurface, _1) ) ;   
		applySurface( start_off_x, start_off_y, message, screen );
	}
	for(int y = 0; y < max_y; y++)
		for(int x = 0; x < max_x; x++)
		{
			if(i < num_of_hosts)
			{
				//Zaleznie od TTL pokaz odpowiednia ikone
				if(boost::get<2>(activeHosts_[i]) > 0)
				{
					applySurface( start_off_x + (x * 120), start_off_y + (y * 120), active, screen );
				}
				else if(boost::get<2>(activeHosts_[i]) <= 0)
				{
					applySurface( start_off_x + (x * 120), start_off_y + (y * 120), inactive, screen );
				}
				// Wypisz IP
				message = boost::shared_ptr<SDL_Surface>(TTF_RenderText_Solid( font.get(), utils::iptos(boost::get<0>(activeHosts_[i])).c_str(), textColor ), boost::bind(&SafeFreeSurface, _1) ) ;   
				applySurface( start_off_x + (x * 120), start_off_y + active->h + (y * 120), message, screen );
				// Wypisz MAC
				message = boost::shared_ptr<SDL_Surface>(TTF_RenderText_Solid( font.get(), utils::macToS(boost::get<1>(activeHosts_[i])).c_str(), textColor ), boost::bind(&SafeFreeSurface, _1) ) ;   
				applySurface( start_off_x + (x * 120), start_off_y + active->h + 12 + (y * 120), message, screen );
			}
			if(i == num_of_hosts)
			{
				applySurface( start_off_x + (x * 120), start_off_y + (y * 120), active, screen );
				// Wypisz IP
				message = boost::shared_ptr<SDL_Surface>(TTF_RenderText_Solid( font.get(), utils::iptos(CNetworkAdapter::getInstance()->getIPandMac().first).c_str(), textColor ), boost::bind(&SafeFreeSurface, _1) ) ;  
				applySurface( start_off_x + (x * 120), start_off_y + active->h + (y * 120), message, screen );
				// Wypisz MAC
				message = boost::shared_ptr<SDL_Surface>(TTF_RenderText_Solid( font.get(), utils::macToS(CNetworkAdapter::getInstance()->getIPandMac().second).c_str(), textColor ), boost::bind(&SafeFreeSurface, _1) ) ; 
				applySurface( start_off_x + (x * 120), start_off_y + active->h + 12 + (y * 120), message, screen );
			}
			i++;
		}
}

void CGUI::startCGUI()
{
	stopCGUI_ = false;
	threadCGUI_ = boost::thread(boost::bind(&CGUI::refreshGUI, this));
}

void CGUI::stopCGUI()
{
	stopCGUI_ = true;
}


void CGUI::refreshCGUIActiveHosts(std::map<utils::MacAdress,ActiveHost, utils::lessMAC>& m)
{
	boost::unique_lock<boost::shared_mutex> write_lock(mutex_);

		std::vector<boost::tuple<IPAddress, MacAdress, int> > v;

	for(std::map<utils::MacAdress,ActiveHost, utils::lessMAC>::iterator it = m.begin(); it != m.end(); ++it)
	{
		v.push_back(boost::make_tuple(it->second.ip, it->second.mac, it->second.ttl));
	}
	activeHosts_ = v;
}
