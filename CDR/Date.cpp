#include "stdafx.h"
#include <iostream>
#include "CDR.h"
#include "functions.h"
#include "Date.h"
#include <string>
#include <sstream>	// f�r att kunna anv�nda stringstream, istringstream
#include <fstream>	// anv�nds f�r att kunna hantera fil-inl�sning/utl�sning
#include <ctime>	// f�r att anv�nda tm och time_t
#include <iomanip>	// beh�vs f�r att kunna anv�nda "setprecision"


int Date::getDateYear()
{
	return this->year;
}

int Date::getDateMonth()
{
	return this->month;
}
void Date::setDateYear(int y)
{
	this->year = y;
}
void Date::setDateMonth(int m)
{
	this->month = m;
}

void Date::setDuration(long long int dur)
{
	this->duration = dur;
}

void Date::addDuration(long long int dur)
{
	this->duration += (dur + basePduration());	// baseProduction l�gger till baspriset fr�n det andra samtalet i samma m�nad nu n�r de tv� l�ggs ihop
}

Date::Date()
{
	this->duration = 0;
	this->month = 0;
	this->year = 0;
}

long long int Date::getDuration()
{
	return this->duration;
}

