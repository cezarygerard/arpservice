#include "CMainLoop.hpp"

void CMainLoop::initMainLoop()
{

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
	boost::this_thread::sleep(boost::posix_time::millisec(100));
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
		else if(token == "show" || token == "sh")
		{
			string alt;
			data >> alt;
			if(alt == "clients" || alt == "cl")
				CConnectionMgr::getInstance()->showConnections();
			else if(alt == "ttl"){
				cout << "Okres rozsylania ARP SEND_PERIOD : " << utils::ARP_SEND_PERIOD << endl;
				cout << "Okres zycia hosta TTL : " << utils::TTL << endl;
			}
			else
			{
				if(alt == "external" || alt == "ex")
				CGViewer::getInstance()->switchView(false);
				
				if(alt == "local" || alt == "lo")
				CGViewer::getInstance()->switchView(true);

				CGViewer::getInstance()->startCGViewer();
				cout << "Wyswietlanie monitora sieci" << endl;
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

			size_t found_pos = ip.find(".");
			int count = 1;
			// proste sprawdzenie adresu
			while (found_pos != -1) {
				found_pos = ip.find(".", found_pos + 1);
				count++;
			}

			if(count == 4)
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

			size_t found_pos = ip.find(".");
			int count = 1;
			// proste sprawdzenie adresu
			while (found_pos != -1) {
				found_pos = ip.find(".", found_pos + 1);
				count++;
			}

			if(count == 4)
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
	cout << "[at 'IP'], [attach 'IP'] w celu dodania host'a do grupy klientow\n";
	cout << "[dt 'IP'], [detach 'IP'] w celu usuniecia host'a z grupy klientow\n";
	cout << "[co], [connect] w celu otwarcia polaczen z grupa klientow\n";
	cout << "[di], [disconnect] w celu zamkniecia polaczen z grupa klientow\n";
	cout << endl;
	cout << "[sh cl], [show clients] aby wyswietlic liste klientow\n";
	cout << "[sh], [show] aby wyswietlic graficzny monitor sieci w obecnym trybie\n";
	cout << "[sh ttl], [show ttl] aby wyswietlic wartosci parametrow TTL i SEND_PERIOD\n";
	cout << "[sh lc], [show local] aby wyswietlic stan sieci w ktorej pracuje aplikacja\n";
	cout << "[sh ex], [show external] aby wyswietlic stan sieci dolaczonych klientow\n";
	cout << endl;
	cout << "[set TTL 'ttl'] aby ustawic TTL\n";
	cout << "[set SEND_PERIOD 'send_period'] aby ustawic okres rozsylania ARP\n";
	cout << "[hi], [hide] aby ukryc graficzny monitor sieci\n";

	
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