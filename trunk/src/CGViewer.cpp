/**
* @author Sebastian £uczak
* @date 2010.01.23
* @version 0.9
*
*	
*/
#include "CGViewer.hpp"

bool CGViewer::stopCGViewer_ = false;


SDL_Surface* screen;  

//powierzchnie wyrysowywane
boost::shared_ptr<SDL_Surface> background;
boost::shared_ptr<SDL_Surface> active;
boost::shared_ptr<SDL_Surface> inactive;
boost::shared_ptr<SDL_Surface> unknown;
boost::shared_ptr<SDL_Surface> message; 

//Fonty
boost::shared_ptr<TTF_Font> font; 
boost::shared_ptr<TTF_Font> font_large;

//Kolory czcionek
SDL_Color textColor = { 255, 255, 255 }; 
SDL_Color textColorBlack = { 0, 0, 0 }; 
SDL_Color textColorRed = { 255, 0, 0 }; 


CGViewer::CGViewer() {
	localView_ = utils::LOCAL;
	utils::fout<<"CGViewer::CGViewer() tworzenie"<< std::endl;
};
CGViewer::~CGViewer() {
	utils::fout<<"CGViewer::~CGViewer() zamykanie"<< std::endl;
	stopCGViewer_ = true;
	threadCGViewer_.join();
};

void SafeFreeSurface(SDL_Surface* surface)
{
	//dealokator SDL_Surface
	if (surface)
		SDL_FreeSurface(surface);
}

void SafeCloseFont(TTF_Font* font)
{
	//dealokator SDL_TTF
    if (font)
        TTF_CloseFont(font);
}


boost::shared_ptr<SDL_Surface> CGViewer::loadImage(const std::string& filename)
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

boost::shared_ptr<TTF_Font> CGViewer::openFont(const std::string& fileName, int size)
{
    boost::shared_ptr<TTF_Font> font(
            TTF_OpenFont(fileName.c_str(), size),
            boost::bind(&SafeCloseFont, _1));
    if (!font.get())
        throw std::runtime_error(TTF_GetError());
    return font;
}


void CGViewer::applySurface( int x, int y,  boost::shared_ptr<SDL_Surface> source, SDL_Surface* destination, SDL_Rect* clip )
{
    SDL_Rect offset;
    
    offset.x = x;
    offset.y = y;

    //wyrysowanie
    SDL_BlitSurface( source.get(), clip, destination, &offset );
}

bool CGViewer::init()
{
    //Inicjalizacja podsystemow
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return false;
    }

    //Inicjalizacja powierzchni wyswietlania
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );

    if( screen == NULL )
    {
        return false;
    }

	//inicjalizacja podsystemu fontow
	if( TTF_Init() == -1 ) 
	{ 
		return false; 
	} 

	// Etykieta okna
    SDL_WM_SetCaption( "Monitor sieci LAN", NULL );

    return true;
}


bool CGViewer::loadFiles()
{
    background = loadImage( "../res/background_L.png" );
    if( background.get() == NULL )
    {
        return false;
    }

	active = loadImage( "../res/active_s.png" );
    if( active.get() == NULL )
    {
        return false;
    }

	inactive = loadImage( "../res/inactive_s.png" );
    if( inactive.get() == NULL )
    {
        return false;
    }

	unknown = loadImage( "../res/unknown_s.png" );
    if( unknown.get() == NULL )
    {
        return false;
    }

	// ladowanie czcionek
	font = openFont( "../res/verdana.ttf", 10 ); 

	font_large = openFont("../res/verdana.ttf", 24 );
	
	if( font.get() == NULL || font_large.get() == NULL ) 
	{ 
		return false; 
	} 

    return true;
}

void CGViewer::cleanUp()
{
	//Zamykanie SDL
    SDL_Quit();
	stopCGViewer_ = true;
}

void CGViewer::refreshGUI()
{
	if( init() == false )
	{
		stopCGViewer_ = true;
	}

	if( loadFiles() == false )
	{
		stopCGViewer_ = true;
	}

	//petla watku
	while( !stopCGViewer_ )
	{

		//wychwytywanie zdarzen
		while( SDL_PollEvent( &event ) )
		{
			
			if( event.type == SDL_QUIT )
			{
				//Wyjdz z graficznego monitora
				stopCGViewer_ = true;
			}
		}

		Uint8 *keystates = SDL_GetKeyState( NULL );

		//Nacisniecie ENTER
		if( keystates[ SDLK_ESCAPE ] )
		{
			stopCGViewer_ = true;
		}

		createTable();

		// zamiana buforow
		if( SDL_Flip( screen ) == -1 )
		{
			stopCGViewer_ = true;
		}

		boost::this_thread::sleep(boost::posix_time::millisec(1000/FRAMES_PER_SECOND));
	}

	cleanUp();
	

}


void CGViewer::createTable()
{
	boost::shared_lock<boost::shared_mutex> lock(mutex_);

	hostsVector v;

	if(localView_ == utils::LOCAL)
		v = activeHosts_;
	else if(localView_ == utils::EXTERNAL)
		v = externalHosts_;
	else
		v = selectedHosts_;

	int i = 0;
	int num_of_hosts = v.size();
	//int num_of_hosts = 300;
	int limit = X_LIMIT*Y_LIMIT;
	ostringstream ss;
	ss << num_of_hosts;

	string str_num_of_hosts = "Aktywnych hostow: " + ss.str();

	if(num_of_hosts > limit) 
		num_of_hosts = limit;
	

	int max_y = Y_LIMIT;
	int max_x = num_of_hosts/max_y + 1; //static_cast<int>(sqrt(static_cast<double>(num_of_hosts)) + 3);
	
	int start_off_x, start_off_y;

	if(max_x >=  X_LIMIT) max_x = X_LIMIT;
	if(max_y >= Y_LIMIT) max_y =  Y_LIMIT;

	start_off_x = (SCREEN_WIDTH - max_x*PICTURE_OFFSET_X)/2;
	start_off_y = (SCREEN_HEIGHT + 100 - max_y*PICTURE_OFFSET_Y)/2;

	applySurface( 0, 0, background, screen );
	
	// naglowek okna
	if(localView_ == utils::LOCAL)
		message = boost::shared_ptr<SDL_Surface>(TTF_RenderText_Solid( font_large.get(), "Lokalne", textColor ), boost::bind(&SafeFreeSurface, _1) ) ;   
	else if(localView_ == utils::EXTERNAL)
		message = boost::shared_ptr<SDL_Surface>(TTF_RenderText_Solid( font_large.get(), "Zdalne", textColor ), boost::bind(&SafeFreeSurface, _1) ) ;   
	else
		message = boost::shared_ptr<SDL_Surface>(TTF_RenderText_Solid( font_large.get(), "Archiwalne", textColor ), boost::bind(&SafeFreeSurface, _1) ) ;   

	applySurface( SCREEN_WIDTH - 150, 20, message, screen );	
	
	// naglowek okna cz2
	message = boost::shared_ptr<SDL_Surface>(TTF_RenderText_Solid( font_large.get(), "Monitor sieci LAN", textColor ), boost::bind(&SafeFreeSurface, _1) ) ;   
	applySurface( SCREEN_WIDTH/2-100, 60, message, screen );

	//liczba hostow
	message = boost::shared_ptr<SDL_Surface>(TTF_RenderText_Solid( font.get(), str_num_of_hosts.c_str(), textColor ), boost::bind(&SafeFreeSurface, _1) ) ;   
	applySurface( SCREEN_WIDTH/2-50, 30, message, screen );

	//Uspokoj uzytkownika tekstem o oczekiwaniu
	if(num_of_hosts == 1)
	{
		message = boost::shared_ptr<SDL_Surface>(TTF_RenderText_Solid( font.get(), "Kompletowanie danych...", textColor ), boost::bind(&SafeFreeSurface, _1) ) ;   
		applySurface( SCREEN_WIDTH/2-50, 15, message, screen );
	}
	
	//Uspokoj uzytkownika tekstem o nadmiarze hostów  
	if(num_of_hosts >= limit)
	{
		message = boost::shared_ptr<SDL_Surface>(TTF_RenderText_Solid( font.get(), "Osiagnieto limit wyswietlanych hostow!", textColorRed ), boost::bind(&SafeFreeSurface, _1) ) ;   
		applySurface( SCREEN_WIDTH/2-80, 40, message, screen );
	}
	for(int y = 0; y < max_y; y++)
		for(int x = 0; x < max_x; x++)
		{
			if(i < num_of_hosts)
			{
				//Zaleznie od TTL pokaz odpowiednia ikone
				if(boost::get<2>(v[i]) > 0)
				{
					applySurface( start_off_x + (x * PICTURE_OFFSET_X), start_off_y + (y * PICTURE_OFFSET_Y), active, screen );
				}
				else if(boost::get<2>(v[i]) <= 0)
				{
					applySurface( start_off_x + (x * PICTURE_OFFSET_X), start_off_y + (y * PICTURE_OFFSET_Y), inactive, screen );
				}
				if(localView_ == utils::SELECTED)
					applySurface( start_off_x + (x * PICTURE_OFFSET_X), start_off_y + (y * PICTURE_OFFSET_Y), unknown, screen );
				//// Wypisz IP
				message = boost::shared_ptr<SDL_Surface>(TTF_RenderText_Solid( font.get(), utils::iptos(boost::get<0>(v[0])).c_str(), textColorBlack ), boost::bind(&SafeFreeSurface, _1) ) ;   
				applySurface( start_off_x + (x * PICTURE_OFFSET_X) + active->w + 1, start_off_y + (y * PICTURE_OFFSET_Y), message, screen );
				//// Wypisz MAC
				message = boost::shared_ptr<SDL_Surface>(TTF_RenderText_Solid( font.get(), utils::macToS(boost::get<1>(v[i])).c_str(), textColorBlack ), boost::bind(&SafeFreeSurface, _1) ) ;   
				applySurface( start_off_x + (x * PICTURE_OFFSET_X) + active->w + 1, start_off_y + 12 + (y * PICTURE_OFFSET_Y), message, screen );
			}
			i++;
		}
}

void CGViewer::startCGViewer()
{
	stopCGViewer_ = true;
	threadCGViewer_.join();

	stopCGViewer_ = false;
	threadCGViewer_ = boost::thread(boost::bind(&CGViewer::refreshGUI, this));
}

void CGViewer::stopCGViewer()
{
	stopCGViewer_ = true;
	threadCGViewer_.join();
}

void CGViewer::switchView(const int local_view)
{
	localView_ = local_view;
	if(localView_ > 2)
		localView_ = 0;
}

void CGViewer::refreshCGViewerActiveHosts(std::map<utils::MacAdress,ActiveHost, utils::lessMAC>& m, int view)
{
	boost::unique_lock<boost::shared_mutex> write_lock(mutex_);

		hostsVector v;

	for(std::map<utils::MacAdress,ActiveHost, utils::lessMAC>::iterator it = m.begin(); it != m.end(); ++it)
	{
		v.push_back(boost::make_tuple(it->second.ip, it->second.mac, it->second.ttl));
	}
	if(view == utils::LOCAL)
		activeHosts_ = v;
	else if(view == utils::EXTERNAL)
		externalHosts_ = v;
	else
		selectedHosts_ = v;
}


