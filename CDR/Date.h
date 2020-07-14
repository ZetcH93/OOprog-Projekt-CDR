#include "stdafx.h"
#include <iostream>
#include "CDR.h"
#include "functions.h"
#include <string>
#include <sstream>	// för att kunna använda stringstream, istringstream
#include <fstream>	// används för att kunna hantera fil-inläsning/utläsning
#include <ctime>	// för att använda tm och time_t
#include <iomanip>	// behövs för att kunna använda "setprecision"




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