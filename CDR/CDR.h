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


class CDR
{

	long long int number;
	int callId =1;
	string nameJSON;
	int startPrice;
	int pricePerMinute;

	long long int* date = new long long int[callId];			// f�r att h�lla datum f�r detta numret
	long long int* duration = new long long int[callId];		// f�r att h�lla tid f�r detta numret
	long long int* personCalled = new long long int[callId];	// f�r att h�lla anropad telefon f�r detta numret

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
	void setCDRNamePrice(string name, int basePrice, int minPrice);		// s�tter json namn, baspris, minutpris

	void CDRinfo(ofstream* jsonFile);									//f�r varje nummer-objekt s� byggs en cdr lista 
	long long int basePduration();										// r�knar ut hur m�nga milsek som ska l�ggas p� f�r varje samtal per m�nad
	double timeConvert(long long int ptr);								// g�r om samtalsl�ngden till minuter och r�knar ut priset
	void createDateList(long long int* date, long long int* duration, ofstream* jsonFile); // bygger sj�lva cdr-listan


};