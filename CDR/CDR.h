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


class CDR
{

	long long int number;
	int callId =1;
	string nameJSON;
	int startPrice;
	int pricePerMinute;

	long long int* date = new long long int[callId];			// för att hålla datum för detta numret
	long long int* duration = new long long int[callId];		// för att hålla tid för detta numret
	long long int* personCalled = new long long int[callId];	// för att hålla anropad telefon för detta numret

public:

	CDR();
	~CDR();

	void defineLine(string line);							//definierar stringlinjen som tas emot av ifstream

	long long int getNumber();
	string getJsonName();
	void setNumber(long long int value);
	void setCalled(long long int value);
	void setDate(long long int value);
	void setDuration(long long int value);
	void setCDRNamePrice(string name, int basePrice, int minPrice);		// sätter json namn, baspris, minutpris

	void CDRinfo(ofstream* jsonFile);									//för varje nummer-objekt så byggs en cdr lista 
	long long int basePduration();										// räknar ut hur många milsek som ska läggas på för varje samtal per månad
	double timeConvert(long long int ptr);								// gör om samtalslängden till minuter och räknar ut priset
	void createDateList(long long int* date, long long int* duration, ofstream* jsonFile); // bygger själva cdr-listan


};