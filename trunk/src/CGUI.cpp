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

	//If there was an error in loading the font 
	if( font == NULL ) 
	{ 
		return false; 
	} 

    return true;
}

void CGUI::clean_up()
{
	//Close the font that was used 
	TTF_CloseFont( font ); 

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

	CTimer* timer = CTimer::getInstance();
	int time;
	int time1;

	while(!stopCGUI_)
	{
		//Quit flag
		bool quit = false;

		CTimer::getInstance()->addObserver(*this, 1000/FRAMES_PER_SECOND);

		//Initialize
		if( init() == false )
		{
			stopCGUI_ = true;
			quit = true;
		}

		//Load the files
		if( load_files() == false )
		{
			stopCGUI_ = true;
			quit = true;
		}

		//While the user hasn't quit
		while( quit == false )
		{
			//Start the frame timer
			time = CTimer::getInstance()->getTime();

			//While there's events to handle
			while( SDL_PollEvent( &event ) )
			{
				//If the user has Xed out the window
				if( event.type == SDL_QUIT )
				{
					//Quit the program
					quit = true;
					stopCGUI_ = true;
				}
			}

			//Get the keystates
			Uint8 *keystates = SDL_GetKeyState( NULL );

			//Nacisniecie ENTER
			if( keystates[ SDLK_RETURN ] )
			{
				///***///
			}

			createTable();

			//Update the screen
			if( SDL_Flip( screen ) == -1 )
			{
				stopCGUI_ = true;
				quit = true;
			}

			//Cap the frame rate
			
			time1 = CTimer::getInstance()->getTime()-time;
			if(time1<1000/FRAMES_PER_SECOND)
				CTimer::getInstance()->delay((1000/FRAMES_PER_SECOND) - time1);	
		}

		clean_up();
	}
	CTimer::destroyInstance();
}

void CGUI::startCGUI()
{
	stopCGUI_ = false;
	threadCGUI_ = boost::thread(boost::bind(&CGUI::refreshGUI, this));
}

void CGUI::createTable()
{
	std::vector<boost::tuple<utils::IPAddress, utils::MacAdress, int>> active_hosts = CDataBaseWrapper::getInstance()->cguiQuery();
	int i = 0;
	int num_of_hosts = active_hosts.size();

	int max_x = static_cast<int>(sqrt(static_cast<double>(num_of_hosts))+1);
	int max_y = static_cast<int>(sqrt(static_cast<double>(num_of_hosts)));
	int start_off_x, start_off_y;

	start_off_x = (SCREEN_WIDTH - max_x*120)/2;
	start_off_y = (SCREEN_HEIGHT - max_y*120)/2;

	string s;
	
	//Apply the background
	apply_surface( 0, 0, background, screen );
	
	for(int y = 0; y < max_y; y++)
		for(int x = 0; x < max_x; x++)
		{
			if(i < num_of_hosts)
			{
				//std::cout << s;			
				if(boost::get<2>(active_hosts[i]) > 0)
				{
					apply_surface( start_off_x + (x * 120), start_off_y + (y * 120), active, screen );
				}
				else if(boost::get<2>(active_hosts[i]) <= 0)
				{
					apply_surface( start_off_x + (x * 120), start_off_y + (y * 120), inactive, screen );
				}
				message = TTF_RenderText_Solid( font, utils::iptos(boost::get<0>(active_hosts[i])).c_str(), textColor ); 
				apply_surface( start_off_x + (x * 120), start_off_y + active->h + (y * 120), message, screen );
				message = TTF_RenderText_Solid( font, utils::macToS(boost::get<1>(active_hosts[i])).c_str(), textColor ); 
				apply_surface( start_off_x + (x * 120), start_off_y + active->h + 12 + (y * 120), message, screen );
			i++;
			}
		}
}