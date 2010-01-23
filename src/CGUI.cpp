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
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

//The frame rate
const int FRAMES_PER_SECOND = 20;


SDL_Surface* CGUI::load_image( std::string filename )
{
    //The image that's loaded
    SDL_Surface* loadedImage = NULL;

    //The optimized surface that will be used
    SDL_Surface* optimizedImage = NULL;

    //Load the image
    loadedImage = IMG_Load( filename.c_str() );

    //If the image loaded
    if( loadedImage != NULL )
    {
        //Create an optimized surface
        optimizedImage = SDL_DisplayFormat( loadedImage );

        //Free the old surface
        SDL_FreeSurface( loadedImage );

        //If the surface was optimized
        if( optimizedImage != NULL )
        {
            //Color key surface
            SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, SDL_MapRGB( optimizedImage->format, 0, 0xFF, 0xFF ) );
        }
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

    //Set the window caption
    SDL_WM_SetCaption( "LAN GUI", NULL );

    //If everything initialized fine
    return true;
}


bool CGUI::load_files()
{
    //Load the back image
    //back = load_image( "fadein.png" );

    ////If there was an error in loading the image
    //if( back == NULL )
    //{
    //    return false;
    //}

    ////Load the front image
    //front = load_image( "fadeout.png" );

    ////If there was an error in loading the image
    //if( front == NULL )
    //{
    //    return false;
    //}

    //If everything loaded fine
    return true;
}

void CGUI::clean_up()
{
    //Free the surfaces
 //   SDL_FreeSurface( back );
  //  SDL_FreeSurface( front );

    //Quit SDL
    SDL_Quit();
}

void CGUI::refreshGUI()
{
	CTimer* timer = CTimer::getInstance();
	CTimer::getInstance()->addObserver(*this, 1000/FRAMES_PER_SECOND);
	int time;
	int time1;

	while(!stopCGUI_)
	{
		//Quit flag
		bool quit = false;

		//The front surface alpha value
		int alpha = SDL_ALPHA_OPAQUE;

		////The frame rate regulator
		//Timer fps;

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
				}
			}

			//Get the keystates
			Uint8 *keystates = SDL_GetKeyState( NULL );

			//If up is pressed
			if( keystates[ SDLK_UP ] )
			{
				//If alpha is not at maximum
				if( alpha < SDL_ALPHA_OPAQUE )
				{
					//Increase alpha
					alpha += 5;
				}
			}

			//If down is pressed
			if( keystates[ SDLK_DOWN ] )
			{
				//If alpha is not at minimum
				if( alpha > SDL_ALPHA_TRANSPARENT )
				{
					//Decrease alpha
					alpha -= 5;
				}
			}

			//Set surface alpha
			//SDL_SetAlpha( front, SDL_SRCALPHA, alpha );

			//Apply the back
			//apply_surface( 0, 0, back, screen );

			//Apply the front
			//apply_surface( 0, 0, front, screen );

			//Update the screen
			if( SDL_Flip( screen ) == -1 )
			{
				stopCGUI_ = true;
			}

			//Cap the frame rate
			
			time1 = CTimer::getInstance()->getTime()-time;
			if(time1<1000/FRAMES_PER_SECOND)
				CTimer::getInstance()->delay((1000/FRAMES_PER_SECOND) - time1);	

			//if( fps.get_ticks() < 1000 / FRAMES_PER_SECOND )
			//{
			//	SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - fps.get_ticks() );
			//}
		}

		//Clean up
		clean_up();
		CTimer::destroyInstance();
	}

}

void CGUI::startCGUI()
{
	stopCGUI_ = false;
	threadCGUI_ = boost::thread(boost::bind(&CGUI::refreshGUI, this));
}