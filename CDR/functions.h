#pragma once
#include "stdafx.h"
#include <iostream>
#include "CDR.h"
#include "functions.h"
#include <string>
#include <sstream>	// för att kunna använda stringstream, istringstream
#include <fstream>	// används för att kunna hantera fil-inläsning/utläsning
#include <ctime>	// för att använda tm och time_t
#include <iomanip>	// behövs för att kunna använda "setprecision"


using namespace std;



long long int checkNumber(string line);		// gör om ett string nummer till en long long int.
bool inRange(long long int value);