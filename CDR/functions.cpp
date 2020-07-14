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




long long int checkNumber(string line)		// denna funktion f�r fram en long long int som repsenterar nummret p� den "linje" som l�ses in fr�n filen.
{											// denna int j�mf�rs sen f�r att se om nummret redan finns som ett objekt.

	long long int value = 0;
	string checkLine;
	stringstream iss(line);

	getline(iss, checkLine, ',');
	istringstream ss(checkLine);		//i:et st�r f�r input och inneb�r att "ss" tar emot ett v�rde som det sen ger ut till en annan datatyp.
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