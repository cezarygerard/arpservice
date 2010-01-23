/**\file
* @author Czarek Zawadka
* @date 2009.12.12
* @version 0.6
* @class CTimerObserver CTimerObserver.hpp
* @brief klasa CTimerObserver to interfejs dla klas korzystajacych z CTimer
*
* 
*/

#ifndef CTIMER_OBSERVER_H
#define CTIMER_OBSERVER_H

#include <set>
#include "CTimer.hpp"
#include "SDL.h"
#include <iostream>

class CTimerObserver{
	///potrzebme aby CTimer mogl sie dobierac do prywatnych skladowych tej klasy
	friend class CTimer;				

//protected:
protected:
	
	///@deprecated metoda ktora moze implementowac klasa dziedziczaca, lepiej uzyc  refresh(int interval, SDL_TimerID timerIds){};
	///wywolywana przez CTimer na instancji obserwatora
	virtual void refresh(){};
	
	///metoda jaka powinna implementowac klasa dziedziczaca po interfejsie CTimerObserver
	///umozliwia obserwowanie wielu zegarow jednoczesnie
	///@param interval - odmierzany czas
	///@param timerIds - id identifikujace obserwowany czas
	virtual void refresh(int interval, SDL_TimerID timerIds){};
	
	/// potrzebne, aby klasy pochodne mogly po sobie elegancko posprzatac
	virtual ~CTimerObserver();

private:
	/// id obserwowanego zegara
	mutable std::set <SDL_TimerID> timerIds_;

};
#endif

//~~CTimerObserver.hpp
