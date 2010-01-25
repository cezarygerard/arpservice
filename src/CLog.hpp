/**
* @author Sebastian £uczak
* @date 2010.01.23
* @version 0.5
*
*	
*/

#ifndef	CLOG_H
#define CLOG_H


#include <iostream>
#include <fstream>
using namespace std;

class CLog : public CSingleton<CLog>
{
	friend CSingleton<CLog>;

public:
  void fout(const char* text);
  
private:
	ofstream file_;
	CLog();
	~CLog();

};

#endif;