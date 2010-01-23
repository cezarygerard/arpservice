/**\file
* @author Czarek Zawadka
* @date 2009.12.12
* @version 0.6
* @class CTimer CTimer.hpp
* @brief klasa CTimer pozwala  klasom implementujacym interfejs CTimerObserver na dostep do timera z SDL'a. CTimer to singleton
*	
* 
*	Korzystanie z CTimer polega na wywolaniu klasy addObserver i cieszeniu sie z dokladnego odmierzania czasu, 
*	gdy juz sie nacieszymy mozna wywolac removeObserver. 
*
* 
*	Dzialanie klasy CTimer polega na obudowaniu SDL'a. Wykorzystywana jest metoda SDL_AddTimer(Uint32 interval, SDL_NewTimerCallback callback, void* param),
*	ktora zalatwia wszystko. Parametry tej funkcji to: interval - odmierzany czas, callback - funkcja ktora ma 
*	zostac wywolana po odliczeniu do koñca, param - argument z ktorym ma byc wywolana funkcja callback. 
*	W CTimer zostala zdefiniowana funkcja timerCallback, ktora jest uzywana jako callback. Parametremi sa: interval(wymagane przez SDL) 
*	oraz  wskaznik na *	zainteresowanego CTimerObserver'a.
*
*/

#ifndef CTIMER_H
#define CTIMER_H

#include <map>

// naglowki SDL
#include  "SDL.h"

#include "CSingleton.hpp"
#include "CTimerObserver.hpp"

using namespace std;

class CTimerObserver;

class CTimer : public CSingleton<CTimer>
{
	friend class CSingleton<CTimer>;

public:

	///rejestrowanie obserwatora. Poczatek odmierzania czasu jest natychmiasowy
	///@param o - obserwator ktory bedzie powiadamiany
	///@param interval - odmierzany czas
	///@return SDL_TimerID - id kotre przypisano
	SDL_TimerID addObserver(CTimerObserver& o, int interval);	
	///usuwanie obserwatora
	void removeObserver(const CTimerObserver& o);
	
	//void start(const CTimerObserver& o);
	//void stop(const CTimerObserver& o);

	///pozwala uzyskac aktualny czas(liczony od uruchomienia SDL), lub czas jaki minal od refTime
	///@param refTime - czas od ktorego ma byc liczony aktualny czas
	int getTime(int refTime = 0);

	///Klasa opakowujaca SDL_Delay, sterowanie wraca do miejsca wywolania o delayTime milisekundach
	///@param delayTime w milisekundach, powinna byc wielokrotnosc 40ms
	void delay(int delayTime);

private:
	///wskaznik na ta metode jest argumentem funkcji SDL_AddTimer(), tutaj realizowane jest 
	///powiadamianie obserwatorow 
	///@param interval - odmierzany czas
	///@param param - wskaznik na obserwatora dla ktorego odmierzany jest czas
	static Uint32 timerCallback(Uint32 interval, void* param);

	///Konstruktor domyslny
	CTimer();

	///Destruktor
	~CTimer();

	///klucz do mapy observers_, dodanie obserwatora inkrementuje id_ o jeden dla kolejnych obserwatorow
	static int id_;

	///struktura opakowujaca CTimerObserver. Zawiera SDL_TimerID 
	struct TimerParam
	{
		SDL_TimerID timreId_;
		CTimerObserver * observer_;
	};

	///kontener przechowujacy obserwatorow
	map<int, TimerParam> observers_;
};

#endif

//~~CTimer.hpp
