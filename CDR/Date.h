#include "stdafx.h"
#include <iostream>
#include "CDR.h"
#include "functions.h"
#include <string>
#include <sstream>	// f�r att kunna anv�nda stringstream, istringstream
#include <fstream>	// anv�nds f�r att kunna hantera fil-inl�sning/utl�sning
#include <ctime>	// f�r att anv�nda tm och time_t
#include <iomanip>	// beh�vs f�r att kunna anv�nda "setprecision"




class Date : public CDR
{
	int year;
	int month;
	long long int duration;

public:

	Date();
	~Date(){};

	int getDateYear();
	int getDateMonth();
	void setDateYear(int y);
	void setDateMonth(int m);
	void setDuration(long long int d);
	void addDuration(long long int d);
	long long int getDuration();


};