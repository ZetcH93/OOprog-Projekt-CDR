#include "stdafx.h"
#include <iostream>
#include "CDR.h"
#include "functions.h"
#include "Date.h"
#include <string>
#include <sstream>	// för att kunna använda stringstream, istringstream
#include <fstream>	// används för att kunna hantera fil-inläsning/utläsning
#include <ctime>	// för att använda tm och time_t
#include <iomanip>	// behövs för att kunna använda "setprecision"


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
	this->duration += (dur + basePduration());	// baseProduction lägger till baspriset från det andra samtalet i samma månad nu när de två läggs ihop
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

