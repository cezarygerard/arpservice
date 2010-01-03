/**
* @author Sebastian £uczak
* @date 2009.12.08
* @version 0.3_draft
* @brief Klasa singletonu sluzaca do latwej implementacji wzrorca projektowego w dowolnej klasie 
*	dziedziczyc : public CSingleton<NazwaKlasyDziedziczacej>
*   zaprzyjaznic friend CSingleton<NazwaKlasyDziedziczacej>;
*	
*/
#ifndef CSINGLETON_H
#define CSINGLETON_H

#include <iostream>
#include <cassert>

template <typename T> class CSingleton
{
public:
	/// Metoda zwracainstancje singletona, tworzy go jesli nieutworzony
	/// @return instancja singletona
	static T* getInstance()
	{
		if(mInstance_== NULL)
			mInstance_ = new T;
			
		assert(mInstance_ != NULL);
		
		return mInstance_;
	};
	/// Metoda usuwa instacje singletona
	static void destroyInstance()
	{
		delete mInstance_;
		mInstance_ = NULL;
	}

protected:
	/// Zabezpieczony przed dostepem konstruktor domyslny
	CSingleton() {
		std::cout << "Powstaje Singleton" << std::endl;
	};
	/// Zabezpieczony przed dostepem destruktor domyslny
	virtual ~CSingleton() {};

private:
	/// Statyczna instancja
	static T* mInstance_;
	/// Zabezpieczony przed dostepem konstruktor kopiujacy
	CSingleton(const CSingleton& source) {};
	/// Zabezpieczony przed dostepem operator przypisania
	CSingleton& operator=(CSingleton const&){};
	


};

template <typename T> T* CSingleton <T>::mInstance_ = 0;

#endif

//~~CSingleton.hpp
