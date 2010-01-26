#include "CMainLoop.hpp"

void CMainLoop::initMainLoop()
{
	//Inicjalizacja podsystemow
    if( SDL_Init( 0 ) == -1 )
    {
       cout << "Inicjalizacja SDL nie powiodla sie" << endl;
    }

	CGViewer::getInstance();
	CNetworkAdapter::getInstance();
	CDataBaseWrapper::getInstance();
	CConnectionMgr::getInstance();	
	loadParams();
	utils::fout.open("../log/log.txt",ios_base::out);
	
}

void CMainLoop::enterMainLoop()
{
	CNetworkAdapter::getInstance()->init(utils::init());
	CNetworkAdapter::getInstance()->open();	

	CConnectionMgr::getInstance()->startListening();
	CDataBaseWrapper::getInstance()->startHandlingReceived();

	string s;
	getline(cin, s);
	s.clear();
	cout << "*****\n*\n* Lan Monitor wita\n* Wprowadz komende po znaku zachety\n* Lista komend po wprowadzeniu ? lub help \n*\n*****" << endl;
	boost::this_thread::sleep(boost::posix_time::millisec(200));
	while(!quit_)	
	{	
		cout << "lanmonitor $ ";
		getline(cin, s);

		istringstream data(s);
		string token;
		// pobierz ze strumienia pierwsza dana, ktora powinna byc token'em
		data >> token;
		//cout << "|" << token << "|" << endl;
		//cout << endl;
		if(token == "")
			;
		///wydruk na ekran
		else if(token == "print" || token == "pr")
		{
			string alt;
			data >> alt;
			///wydruk klientow
			if(alt == "clients" || alt == "cl")
				CConnectionMgr::getInstance()->showConnections();
			///wydruk czasu zycia hosta i odstepu miedzy wysylaniem ARP
			else if(alt == "ttl"){
				cout << "Okres rozsylania ARP SEND_PERIOD : " << utils::ARP_SEND_PERIOD << endl;
				cout << "Okres zycia hosta TTL : " << utils::TTL << endl;
			}
			///wydruk adresow sieci z bazy
			else if(alt == "netaddr" || alt == "net"){
				CDataBaseWrapper::getInstance()->showNetAddresses();
			}
			else
			{
				cerr << "Wprowadzono bledny parametr print, wprowadz ? w celu otrzymania listy komend" << endl;
			}
		}
		///wyswietlenie monitora wizualnego
		else if(token == "show" || token == "sh")
		{
			string alt;
			data >> alt;
			///wyswietlenie sieci z klientow
			if(alt == "external" || alt == "ex")
				CGViewer::getInstance()->switchView(utils::EXTERNAL);
			///wyswietlenie sieci lokalnej
			else if(alt == "local" || alt == "lo")
				CGViewer::getInstance()->switchView(utils::LOCAL);
			///wyswietlenie sieci archiwalnej
			else if(alt == "archival" || alt == "ar")
				CGViewer::getInstance()->switchView(utils::SELECTED);

			CGViewer::getInstance()->startCGViewer();
			cout << "Wyswietlanie monitora sieci" << endl;
		
		}
		/// ladowanie z SQL
		else if(token == "load" || token == "lo")
		{
			string alt;
			data >> alt;
			///wyswietlenie historii klienta
			if(alt == "?" || alt == "help")
			{
				cout << "[lo ho 'mac'], [load host 'mac'] w celu zaladowania historii hosta o danym MAC\n";
				cout << "[lo net 'netaddr'], [load network 'netaddr'] w celu zaladowania stanu danej sieci z bazy\n";
				cout << "[pr net], [print netaddr] aby wyswietlic adresy sieci do ktorych naleza hosty z bazy\n";
			}

			else if(alt == "host" || alt == "ho")
			{
				string mac;
				data >> mac;

				if(mac.length() == 12)
					CDataBaseWrapper::getInstance()->showHostHistory(mac);
				else
				{
					cerr << "Podano mac adres o nieprawidlowej budowie\nprzyklad:\nnload host 000AE63EFDE1" << endl;
				}

			
			}
			///zaladowanie archiwalnej sieci lokalnej
			else if(alt == "net" || alt == "network")
			{
				string netaddr;
				data >> netaddr;

				if(utils::checkIP(netaddr) == 4)
				{
					cout << "Pobieranie historii..." << endl;
					CDataBaseWrapper::getInstance()->loadSpecificHosts(netaddr);
				}
				else
				{
					cerr << "Podano adres sieci o nieprawidlowej budowie\nprzyklad:\nload network 192.168.1.0" << endl;
				}
			}
			else
			{
				cerr << "Podano nieprawidlowy parametr" << endl;
			}
		
		}
		else if(token == "hide" || token == "hi")
		{
			CGViewer::getInstance()->stopCGViewer();
			cout << "Zamykanie monitora sieci..." << endl;
		}
		else if(token == "attach" || token == "at")
		{	
			string ip;
			data >> ip;

			if(utils::checkIP(ip) == 4)
			{
				cout << "Dodawanie adresu..." << endl;
				CConnectionMgr::getInstance()->addIPAddress(ip);
			}
			else
			{
				cerr << "Podano adres ip o nieprawidlowej budowie\nprzyklad:\nattach 192.168.0.1" << endl;
			}
		}
		else if(token == "detach" || token == "de")
		{
			string ip;
			data >> ip;

			if(utils::checkIP(ip) == 4)
			{
				cout << "Usuwanie adresu..." << endl;
				CConnectionMgr::getInstance()->removeIPAddress(ip);
			}
			else
			{
				cerr << "Podano adres ip o nieprawidlowej budowie\nprzyklad:\ndetach 192.168.0.1" << endl;
			}
		}
		else if(token == "connect" || token == "co")
		{
			serviceThread_.join();
			serviceThread_ = boost::thread(boost::bind(&CMainLoop::startCConnections, this));
			
		}
		else if(token == "disconnect" || token == "di")
		{
			serviceThread_.join();
			serviceThread_ = boost::thread(boost::bind(&CMainLoop::stopCConnections, this));
			cout << "Oczekiwanie na zakonczenie polaczenia..." << endl;
		}
		else if( token == "set") {
			string alt;
			data >> alt;
			if(alt == "SEND_PERIOD")
			{
				data >> utils::ARP_SEND_PERIOD;
				utils::MAX_TTL = utils::TTL * utils::ARP_SEND_PERIOD;
				cout << "Okres rozsylania ARP zmieniono na wartosc " << utils::ARP_SEND_PERIOD << endl;
			}
			else if( alt == "TTL" ) 
			{
				data >> utils::TTL;
				utils::MAX_TTL = utils::TTL * utils::ARP_SEND_PERIOD;
				cout << "Czas zycia host'a zmieniono na wartosc " << utils::TTL << endl;
			}
			else if( alt == "?" )
			{
				cout << "[set TTL 'ttl'] aby ustawic TTL\n";
				cout << "[set SEND_PERIOD 'send_period'] aby ustawic okres rozsylania ARP\n";
			}
			else
				cerr << "Wprowadzono bledna komende zmiany wartosci, wprowadz ? w celu otrzymania listy komend" << endl;
		}
		else if(token == "quit" || token == "q" || token == "exit")
		{
			quit_ = true;
			cout << "Oczekiwanie na zakonczenie wszystkich procesow..." << endl;
		}
		else if(token == "?" || token == "help" || token == "man")
		{
			showHelp();
		}
		else
			cerr << "Wprowadzono bledna komende, wprowadz ? w celu otrzymania listy komend" << endl;
		s.clear();
		cout << endl;
	}
}

void CMainLoop::endMainLoop()
{
	serviceThread_.join();

	CDataBaseWrapper::destroyInstance();
	CNetworkAdapter::destroyInstance();
	CConnectionMgr::destroyInstance(); 
	CGViewer::destroyInstance();

	utils::fout.close();
}

void CMainLoop::quitNow()
{
	quit_ = true;
}

void CMainLoop::showHelp()
{
	cout << "Lista komend\n";
	cout << endl;
	cout << "[?], [help] aby wyswietlic ten ekran\n";
	cout << "[q], [quit] aby wyjsc\n";
	cout << endl;
	cout << "[pr ttl], [print ttl] aby wyswietlic wartosci parametrow TTL i SEND_PERIOD\n";
	cout << "[set TTL 'ttl'] aby ustawic TTL\n";
	cout << "[set SEND_PERIOD 'send_period'] aby ustawic okres rozsylania ARP\n";
	cout << endl;
	cout << "[at 'IP'], [attach 'IP'] w celu dodania host'a do grupy klientow\n";
	cout << "[dt 'IP'], [detach 'IP'] w celu usuniecia host'a z grupy klientow\n";
	cout << "[co], [connect] w celu otwarcia polaczen z grupa klientow\n";
	cout << "[di], [disconnect] w celu zamkniecia polaczen z grupa klientow\n";
	cout << "[pr cl], [print clients] aby wyswietlic liste klientow\n";
	cout << endl;
	cout << "[lo ho 'mac'], [load host 'mac'] w celu zaladowania historii hosta o danym MAC\n";
	cout << "[lo net 'netaddr'], [load network 'netaddr'] w celu zaladowania stanu danej sieci z bazy\n";
	cout << "[pr net], [print netaddr] aby wyswietlic adresy sieci do ktorych naleza hosty z bazy\n";
	
	cout << endl;
	cout << "[sh], [show] aby wyswietlic graficzny monitor sieci w obecnym trybie\n";
	cout << "[sh lc], [show local] aby wyswietlic stan sieci w ktorej pracuje aplikacja\n";
	cout << "[sh ex], [show external] aby wyswietlic stan sieci dolaczonych klientow\n";
	cout << "[sh ar], [show archival] aby wyswietlic stan zaladowanej sieci archiwalnej\n";
	cout << "[hi], [hide] aby ukryc graficzny monitor sieci\n";
	cout << endl;
	

	
}
bool CMainLoop::loadParams() 
{
	string s;

	{
		ifstream in("../conf/params.conf");
		
		
		if(!in) 
		{
			return false;
		}

		// proste pobieranie danych ze strumienia oparte na poszukiwaniu znacznikow
		while( getline(in, s) ) {

			istringstream data(s);
			string token;
			// pobierz ze strumienia pierwsza dana, ktora powinna byc token'em
			data >> token;
			//cout << token << endl;
			if( token == "ARP_SEND_PERIOD") {
				//przytnij
				data.ignore(20, '=');
				//pobierz czas w odstepie ktorego wysylane maja byc pakiety
				data >> utils::ARP_SEND_PERIOD;
			//	cout << utils::ARP_SEND_PERIOD << endl;
			}
			else if( token == "MAX_TTL_EXTERNAL") {
				//przytnij
				data.ignore(20, '=');
				//pobierz czas zycia hosta
				data >> utils::MAX_TTL_EXTERNAL;
			//	cout << utils::TTL << endl;
			}
			else if( token == "TTL") {
				//przytnij
				data.ignore(20, '=');
				//pobierz czas zycia hosta
				data >> utils::TTL;
			//	cout << utils::TTL << endl;
			}
		}
	}
	utils::MAX_TTL = utils::TTL * utils::ARP_SEND_PERIOD;
	return true;
}

void CMainLoop::stopCConnections()
{
	CConnectionMgr::getInstance()->stopConnections();
	cout << "Polaczenia zostaly zakonczone" << endl;
}

void CMainLoop::startCConnections()
{
	CConnectionMgr::getInstance()->startConnections();
	boost::this_thread::sleep(boost::posix_time::milliseconds(50));
}