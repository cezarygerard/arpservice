/** @file CTimerObserver.cpp
* @author Czarek Zawadka
* @date 2009.12.12
* @version 0.9
* @brief klasa CTimerObserver to interfejs dla klas korzystajacych z CTimer
*
*/

#include "CTimerObserver.hpp"

///destruktor powoduje wyrejestrowanie sie z obserwatorow CTimer'a. jezli juz to zostalo zrobione 
///to nic zlego nie powinno sie zdarzyc
CTimerObserver::~CTimerObserver()
{
	std::cout<<"CTimerObserver::~CTimerObserver: niszczenie"<<std::endl; 
	CTimer::getInstance()->removeObserver(*this);
		
}

//~~CTimerObserver.cpp
