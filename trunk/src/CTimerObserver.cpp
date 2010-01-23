/** @file CTimer.cpp
* @author Cezary Zawadka
* @date 2009.12.18
* @version 0.9
* @brief klasa CTimer pozwala  klasom implementujacym interfejs CTimerObserver na dostep do timera z SDL'a. CTimer to singleton
*	
*		
*/


#include "CTimer.hpp"

//inicjalizacja statycznego id
int CTimer::id_=0;

///Konstruktor domyslny
CTimer::CTimer()
{
}

//Destruktor
CTimer::~CTimer()
{
	observers_.clear();
}

//metoda wywolywana przez  SDL 
Uint32 CTimer::timerCallback(Uint32 interval, void* param)
{
	TimerParam * tp = reinterpret_cast<TimerParam *>(param);
	tp->observer_->refresh();
	tp->observer_->refresh(interval, tp->timreId_);
	return interval; 
}

//w tej metodzie wywolywane jest SDL_AddTimer(interval, &timerCallback, &o), rozpoczyna sie odmierzanie 
//po dodatniu kolejnego CTimerObserver zwiekszany jest id_;
SDL_TimerID CTimer::addObserver(CTimerObserver& o, int interval)
{
	TimerParam tp;
	observers_.insert(pair<int, TimerParam> (id_,tp));
	SDL_TimerID newTimerId = SDL_AddTimer(interval, &timerCallback, &(observers_[id_]));
	o.timerIds_.insert(newTimerId);
	observers_[id_].observer_ = &o;
	observers_[id_].timreId_ = newTimerId;
	cout<<"CTimer::addObserver: dodano obserwatora CTimerObserver, odmierzany czas: "<<interval<<" newTimerId  "<<newTimerId<<" id "<<id_ <<endl;
	id_++;
	
	return newTimerId;
}

//usuwanie CTimerObserwera - nie bedzie juz dostawac powiadomien
void CTimer::removeObserver(const CTimerObserver& o)
{
	set<SDL_TimerID>::iterator it;
	for(it = o.timerIds_.begin(); it != o.timerIds_.end(); it++ )
	{
		SDL_RemoveTimer(*it);
	}
}

/*
void start(const CTimerObserver& o);
void stop(const CTimerObserver& o);
*/
int CTimer::getTime(int refTime)
{

	return SDL_GetTicks() - refTime;
}

//Klasa opakowujaca SDL_Delay, sterowanie wraca do miejsca wywolania o delayTime milisekundach
void CTimer::delay(int delayTime)
{
	SDL_Delay(delayTime);
}

//~~CTimer.cpp
