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




long long int checkNumber(string line)		// denna funktion får fram en long long int som repsenterar nummret på den "linje" som läses in från filen.
{											// denna int jämförs sen för att se om nummret redan finns som ett objekt.

	long long int value = 0;
	string checkLine;
	stringstream iss(line);

	getline(iss, checkLine, ',');
	istringstream ss(checkLine);		//i:et står för input och innebär att "ss" tar emot ett värde som det sen ger ut till en annan datatyp.
	ss >> value;

	return value;
}

bool inRange(long long int value)
{	
	bool returnValue;
	int counter = 1;
	int trigger = 0;
	while (counter <= 12)
	{
		if (counter == value)
		{
			trigger = 1;
		}
		counter++;
	}
	if (trigger == 1)
		returnValue = false;
	else
		returnValue = true;

	return returnValue;
}