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

SDL_Surface* screen = NULL;

SDL_Surface* background = NULL;
SDL_Surface* active = NULL;
SDL_Surface* inactive = NULL;
SDL_Surface *message = NULL; 

//The font that's going to be used 
TTF_Font *font = NULL; 
TTF_Font *font_large = NULL;

//The color of the font 
SDL_Color textColor = { 255, 255, 255 }; 




SDL_Surface* CGUI::load_image( std::string filename )
{
    SDL_Surface* loadedImage = NULL;
    SDL_Surface* optimizedImage = NULL;

    //Ladowanie grafiki
    loadedImage = IMG_Load( filename.c_str() );

    if( loadedImage != NULL )
    {
        //dostosowanie grafiki do wyswietlenia z kanalem przezroczystosci
        optimizedImage = SDL_DisplayFormatAlpha( loadedImage );

        //Usuwanie zbednej powierzchni roboczej
        SDL_FreeSurface( loadedImage );

    }

    //Return the optimized surface
    return optimizedImage;
}


void CGUI::apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip )
{
    //Holds offsets
    SDL_Rect offset;

    //Get offsets
    offset.x = x;
    offset.y = y;

    //Blit
    SDL_BlitSurface( source, clip, destination, &offset );
}

bool CGUI::init()
{
    //Initialize all SDL subsystems
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return false;
    }

    //Set up the screen
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );

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


bool CGUI::load_files()
{
    background = load_image( "../res/background.png" );
    if( background == NULL )
    {
        return false;
    }

	active= load_image( "../res/active.png" );
    if( active == NULL )
    {
        return false;
    }

	inactive = load_image( "../res/inactive.png" );
    if( inactive == NULL )
    {
        return false;
    }

	//Open the font 
	font = TTF_OpenFont( "../res/verdana.ttf", 12 ); 

	font_large = TTF_OpenFont("../res/verdana.ttf", 24 );
	//If there was an error in loading the font 
	if( font == NULL || font_large == NULL ) 
	{ 
		return false; 
	} 

    return true;
}

void CGUI::clean_up()
{
	//Close the font that was used 
	TTF_CloseFont( font ); 
	TTF_CloseFont( font_large);
    //Free the surfaces
    SDL_FreeSurface( background );
	SDL_FreeSurface( inactive );
	SDL_FreeSurface( active );

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
		if( load_files() == false )
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

		clean_up();
	}

}


void CGUI::createTable()
{
	int i = 0;
	int num_of_hosts = activeHosts_.size();

	int max_x = static_cast<int>(sqrt(static_cast<double>(num_of_hosts))+1)%9;
	int max_y = static_cast<int>(sqrt(static_cast<double>(num_of_hosts)))%7;
	
	int start_off_x, start_off_y;

	start_off_x = (SCREEN_WIDTH - max_x*120)/2;
	start_off_y = (SCREEN_HEIGHT + 100 - max_y*120)/2;

	apply_surface( 0, 0, background, screen );

	message = TTF_RenderText_Solid( font_large, "Monitor sieci LAN", textColor ); 
	apply_surface( SCREEN_WIDTH/2-100, 60, message, screen );

	//Uspokoj uzytkownika tekstem o oczekiwaniu
	if(num_of_hosts == 0)
	{
		message = TTF_RenderText_Solid( font, "Kompletowanie danych...", textColor ); 
		apply_surface( start_off_x, start_off_y, message, screen );
	}
	for(int y = 0; y < max_y; y++)
		for(int x = 0; x < max_x; x++)
		{
			if(i < num_of_hosts)
			{
				//Zaleznie od TTL pokaz odpowiednia ikone
				if(boost::get<2>(activeHosts_[i]) > 0)
				{
					apply_surface( start_off_x + (x * 120), start_off_y + (y * 120), active, screen );
				}
				else if(boost::get<2>(activeHosts_[i]) <= 0)
				{
					apply_surface( start_off_x + (x * 120), start_off_y + (y * 120), inactive, screen );
				}
				// Wypisz IP
				message = TTF_RenderText_Solid( font, utils::iptos(boost::get<0>(activeHosts_[i])).c_str(), textColor ); 
				apply_surface( start_off_x + (x * 120), start_off_y + active->h + (y * 120), message, screen );
				// Wypisz MAC
				message = TTF_RenderText_Solid( font, utils::macToS(boost::get<1>(activeHosts_[i])).c_str(), textColor ); 
				apply_surface( start_off_x + (x * 120), start_off_y + active->h + 12 + (y * 120), message, screen );
			}
			if(i == num_of_hosts)
			{
				apply_surface( start_off_x + (x * 120), start_off_y + (y * 120), active, screen );
				// Wypisz IP
				message = TTF_RenderText_Solid( font, utils::iptos(CNetworkAdapter::getInstance()->getIPandMac().first).c_str(), textColor ); 
				apply_surface( start_off_x + (x * 120), start_off_y + active->h + (y * 120), message, screen );
				// Wypisz MAC
				message = TTF_RenderText_Solid( font, utils::macToS(CNetworkAdapter::getInstance()->getIPandMac().second).c_str(), textColor ); 
				apply_surface( start_off_x + (x * 120), start_off_y + active->h + 12 + (y * 120), message, screen );
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


void CGUI::refreshCGUIActiveHosts()
{
	activeHosts_ = CDataBaseWrapper::getInstance()->cguiQuery();
}
