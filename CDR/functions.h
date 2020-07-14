#pragma once
#include "stdafx.h"
#include <iostream>
#include "CDR.h"
#include "functions.h"
#include <string>
#include <sstream>	// f�r att kunna anv�nda stringstream, istringstream
#include <fstream>	// anv�nds f�r att kunna hantera fil-inl�sning/utl�sning
#include <ctime>	// f�r att anv�nda tm och time_t
#include <iomanip>	// beh�vs f�r att kunna anv�nda "setprecision"


using namespace std;



long long int checkNumber(string line);		// g�r om ett string nummer till en long long int.
bool inRange(long long int value);