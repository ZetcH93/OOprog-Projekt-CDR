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


using namespace std;


void CDR::defineLine(string line)
{	
	
	long long int value = 0;
	int counter = 0;
	string text;
	stringstream iss(line);

	while (getline(iss, text, ','))			//l�ser in text upp till "," fr�n linjen, anv�nder en counter f�r att se vart p� linjen man l�ser in.
	{
		
		istringstream ss(text);		// streamar om string v�rdet som "text" har till en long long int och l�gger denna i value.
		ss >> value;

		if (counter == 0)				// f�rsta g�ngen s� l�ser den in telefonnummret
			setNumber(value);			
		else if (counter == 1)			// andra g�ngen �r personen den ringer
			setCalled(value);
		else if (counter == 2)			// l�ser in datumet och s�tter in denna
			setDate(value);
		else if (counter == 3)			// l�ser in l�ngden f�r samtalet.
			setDuration(value);
		
		counter++;
		
	}
	


}


void CDR::setNumber(long long int value)
{
	if (this->number == value)			// om nummret redan finns som ett objekt, s� l�ggs ny data till dess variabler.
	{
		this->callId++;
		long long int *tempDate = new long long int[callId];		
		long long int *tempDuration = new long long int[callId];		
		long long int *tempCalled = new long long int[callId];
		
		for (int i = 0; i < callId-1; i++)
		{
			tempDate[i] = date[i];
			tempDuration[i] = duration[i];
			tempCalled[i] = personCalled[i];
		}
		date = nullptr;
		duration = nullptr;
		personCalled = nullptr;
		delete[] date;
		delete[] duration;						// denna kod �kar platsen p� nummrets date,called,duration arrayer och l�gger in det nya samtalet i dessa.
		delete[] personCalled;

		date = new long long int[callId];
		duration = new long long int[callId];
		personCalled = new long long int[callId];

		for (int i = 0; i < callId-1; i++)
		{
			date[i] = tempDate[i];
			duration[i] = tempDuration[i];
			personCalled[i] = tempCalled[i];
		}
		
	}
	this->number = value;			// om nummret inte redan finns s� l�ggs det nya nummret in h�r
}
void CDR::setCalled(long long int value)
{
	this->personCalled[callId-1] = value;			// l�gger till personen som ringdes nummer l�ngst bak i arrayen
}
void CDR::setDate(long long int value)
{
	this->date[callId-1] = value;		// l�gger till datumet l�ngst bak i samtaldatum arrayen
}
void CDR::setDuration(long long int value)
{
	this->duration[callId-1] = value;			// l�gger till l�ngden av samtalet l�ngst bak i duration arrayen
}

long long int CDR::getNumber()
{
	return this->number;
}

string CDR::getJsonName()
{	
	string name = this->nameJSON;					// l�gger till .json p� det namn p� textfilen som personen �nskade, s� att det blir en json fil
	name += ".json";
	return name;
}

void CDR::CDRinfo(ofstream *jsonFile)
{
	*jsonFile << "   \"MSISDN\":\"" << number << "\"," << endl;
		 createDateList(date,duration, jsonFile );			// n�r JSON filen skapas s� skriver denna funktion ut info om detta nummret och alla dess samtal

}


void CDR::createDateList(long long int* date, long long int* duration, ofstream* jsonFile)
{
	long long int* arrPtr = new long long int[callId];				// g�r nya dynamiska arrayer som h�ller alla datum och samtalsl�ngder som tillh�r nummret.
	long long int* durationPtr = new long long int[callId];

	for (int i = 0; i < callId; i++)
	{
		arrPtr[i] = date[i];
		durationPtr[i] = duration[i];
	}


	for (int i = 0; i < callId; i++)
	{
		for (int j = i + 1; j < callId; j++)
		{
			if (arrPtr[i] > arrPtr[j])
			{
				long long int tempInt;
				long long int tempDuration;
				tempInt = arrPtr[i];
				tempDuration = durationPtr[i];			// sorterar de tv� arrayerna i storleksordning liten->st�rst.
				arrPtr[i] = arrPtr[j];
				durationPtr[i] = durationPtr[j];
				arrPtr[j] = tempInt;
				durationPtr[j] = tempDuration;
			}
		}
	}


	Date* cdrDates = new Date[callId]; // g�r en ny class d�r objekten fungerar som m�nader som tillh�r CDR classen.
	time_t* time = new time_t[callId];
	tm* dateAndMonth = new tm[callId];

	for (int i = 0; i < callId; i++)				// g�r en time_t array som h�ller datumen i sekunder.
	{													// jag stoppar sedan in time_t objekten i en tm array d�r jag anpassar time_t objekten efter lokaltid.
		time[i] = arrPtr[i];
		dateAndMonth[i] = *localtime(&time[i]);
	}



	for (int i = 0; i < callId; i++)
	{
		cdrDates[i].setDateMonth(dateAndMonth[i].tm_mon + 1);
		cdrDates[i].setDateYear(dateAndMonth[i].tm_year + 1900);		//jag initierar de olika datum objekten med data fr�n tm arrayen, samt tiden av samtalen f�r varje m�nad
		cdrDates[i].setDuration(durationPtr[i]);
	}



	Date* tempArr = nullptr;
	int newArrSize = callId;
	for (int i = 0; i <= newArrSize; i++)			// denna for loop g�r igenom alla datum objekt i arrayen och l�gger ihop de datum objekt som representerar samma m�nad.
	{
		if ((i == newArrSize - 1) || (i > newArrSize -1))
		{
			for (int j = 0; j < newArrSize; j++)
			{
				if ((cdrDates[j].getDateMonth() == cdrDates[j + 1].getDateMonth()) && (cdrDates[j].getDateYear() == cdrDates[j + 1].getDateYear()))
				{
					cdrDates[j].addDuration(cdrDates[j + 1].getDuration());
					--newArrSize;
					tempArr = new Date[newArrSize];
					for (int x = j + 1; x < newArrSize; x++)
					{
						cdrDates[x] = cdrDates[x + 1];
					}
					for (int i = 0; i < newArrSize; i++)
					{
						tempArr[i] = cdrDates[i];
					}
					cdrDates = nullptr;
					delete[] cdrDates;
					cdrDates = new Date[newArrSize];
					for (int x = 0; x < newArrSize; x++)
					{
						cdrDates[x] = tempArr[x];
					}
					tempArr = nullptr;
					delete[] tempArr;
					j--;
				}

			}
		}

		else {
			for (int j = i + 1; j < newArrSize; j++)
			{
				if ((cdrDates[i].getDateMonth() == cdrDates[i - 1].getDateMonth()) && (cdrDates[i].getDateYear() == cdrDates[i - 1].getDateYear()))
				{
					--newArrSize;
					cdrDates[i - 1].addDuration(cdrDates[i].getDuration());
					tempArr = new Date[newArrSize];
					for (int x = i; x < newArrSize; x++)
					{
						cdrDates[x] = cdrDates[x + 1];	// tar bort objektet fr�n arrayen.
					}
					for (int x = 0; x < newArrSize; x++)
					{
						tempArr[x] = cdrDates[x];
					}
					cdrDates = nullptr;
					delete[] cdrDates;
					cdrDates = new Date[newArrSize];
					for (int x = 0; x < newArrSize; x++)
					{
						cdrDates[x] = tempArr[x];
					}
					tempArr = nullptr;
					delete[] tempArr;
				}

				else if ((cdrDates[i].getDateMonth() == cdrDates[j].getDateMonth()) && (cdrDates[i].getDateYear() == cdrDates[j].getDateYear()))
				{
					--newArrSize;
					cdrDates[i].addDuration(cdrDates[j].getDuration());
					tempArr = new Date[newArrSize];
					for (int x = j; x < newArrSize; x++)
					{
						cdrDates[x] = cdrDates[x + 1];	// tar bort objektet fr�n arrayen.
					}
					for (int x = 0; x < newArrSize; x++)
					{
						tempArr[x] = cdrDates[x];
					}
					cdrDates = nullptr;
					delete[] cdrDates;
					cdrDates = new Date[newArrSize];
					for (int x = 0; x < newArrSize; x++)
					{
						cdrDates[x] = tempArr[x];
					}
					tempArr = nullptr;
					delete[] tempArr;

				}
			}
		}
	}



	int iSwitch = 0;
	bool triggerCondition = true;
	int index = 0;
	int minLocation = 0;			// dessa variabler anv�ndes i den kommande while loopen
	Date* tempDates = nullptr;
	Date* newTempDates = nullptr;
	int yCounter = 0;
	int nSize;
	int onlyDate = 0;

	while (triggerCondition)		// denna loop l�gger till m�nader i cdrDates arrayen, m�naderna som l�ggs till ligger "emellan" de m�naderna d�r det finns samtal gjorda, dvs fr�n f�rsta m�nad
	{								// d�r samtal gjordes till den sista m�naden. Dessa nya m�nader som l�ggs till har samtalsl�ngd = 0.
		int mCounter = 0;
		bool monthAndYear = true;


		while (monthAndYear)
		{


			if ((cdrDates[index].getDateYear() + yCounter) == cdrDates[index + 1].getDateYear())
			{
				int tempMin = cdrDates[index].getDateMonth();
				int tempMax = cdrDates[index + 1].getDateMonth();

				if (yCounter > 0)
				{
					tempMin = (tempMin - yCounter * 12);
					yCounter = 0;
				}

				while (tempMin != tempMax)
				{
					tempMin++;
					mCounter++;						// h�r uppe s� r�knar jag fram totalt hur m�nga m�nader som ligger emellan tv� "orginal cdrDates"
					monthAndYear = false;			// resultatet l�ggs i mcounter.
				}
			}
			else if (inRange(cdrDates[index + 1].getDateMonth()))	// kollar om det finns n�gra fler samtal m�naden efter denna
			{
				monthAndYear = false;
				onlyDate = 1;
			}
			else
			{
				yCounter = 0;
				int yMin = cdrDates[index].getDateYear();
				int yMax = cdrDates[index + 1].getDateYear();
				while (yMin != yMax)
				{
					yMin++;
					yCounter++;
				}
			}
		}
		if (iSwitch == 0)
		{
			tempDates = new Date[(index + 1) + mCounter];

			tempDates[0].setDateYear(cdrDates[index].getDateYear());
			tempDates[0].setDateMonth(cdrDates[index].getDateMonth());
			tempDates[0].setDuration(cdrDates[index].getDuration());

			int year = cdrDates[index].getDateYear();				// iden �r att jag s�tter "f�rsta" v�rdet fr�n cdrDates p� 0 i den nya arrayen, sen s� fyller jag p� med "skr�p dates"
			int startMonth = cdrDates[index].getDateMonth();		// fram till att jag n�r det "andra" v�rdet i cdrDates. loopen forts�tter s�h�r tills den aldra sista m�naden
																	// d�r ett samtal ringdes.
			for (int j = 1; j < mCounter; j++)
			{
				if ((startMonth == cdrDates[index].getDateMonth()) && (year == cdrDates[index].getDateYear()) && (cdrDates[index].getDateMonth() != 12))
				{
					tempDates[j].setDateMonth(++startMonth);
					tempDates[j].setDateYear(cdrDates[index].getDateYear());
					tempDates[j].setDuration(0);
					if (startMonth != 12)
						startMonth++;
				}
				else if ((startMonth + 1) == 13)
				{
					if (cdrDates[index].getDateMonth() == startMonth)
					{
						startMonth = 1;
						year++;
						j--;

					}
					else if (startMonth == 12 && (tempDates[j - 1].getDateMonth() == 12))		// h�r l�gger jag till "nya m�nader d�r samtalsl�ngd = 0
					{																			// fram till dess att mCounter har uppn�ts
						startMonth = 1;
						year++;
						j--;

					}
					else
					{
						tempDates[j].setDateMonth(startMonth);
						tempDates[j].setDateYear(year++);
						tempDates[j].setDuration(0);
						startMonth = 1;
					}
				}
				else
				{
					tempDates[j].setDateMonth(startMonth);
					tempDates[j].setDateYear(year);
					tempDates[j].setDuration(0);
					startMonth++;
				}
			}
			if (onlyDate == 1) // om det bara finns en m�nad d�r samtal gjordes
			{
				newTempDates = new Date[1];
				newTempDates[index + mCounter].setDateMonth(cdrDates[index].getDateMonth());
				newTempDates[index + mCounter].setDateYear(cdrDates[index].getDateYear());
				newTempDates[index + mCounter].setDuration(cdrDates[index].getDuration());
				triggerCondition = false;
			}
			else
			{
				tempDates[index + mCounter].setDateMonth(cdrDates[index + 1].getDateMonth());
				tempDates[index + mCounter].setDateYear(cdrDates[index + 1].getDateYear());
				tempDates[index + mCounter].setDuration(cdrDates[index + 1].getDuration());
			}

			nSize = (1 + mCounter);
			minLocation = (nSize - 1);


			index++;
			iSwitch++;

		}
		else if (iSwitch >= 1)
		{


			if (iSwitch == 2 || iSwitch == 1)
			{
				nSize = (nSize + mCounter);
				if (iSwitch == 1)
				{
					newTempDates = nullptr;
					newTempDates = new Date[nSize];
					for (int j = 0; j < nSize; j++)
					{
						newTempDates[j].setDateMonth(tempDates[j].getDateMonth());
						newTempDates[j].setDateYear(tempDates[j].getDateYear());
						newTempDates[j].setDuration(tempDates[j].getDuration());
					}
				}
				if (iSwitch == 2)
				{
					tempDates = nullptr;
					tempDates = new Date[nSize];
					for (int j = 0; j < nSize; j++)
					{																		// g�r nya arrayer efter behov och kopierar �ver de gamla
						tempDates[j].setDateMonth(newTempDates[j].getDateMonth());
						tempDates[j].setDateYear(newTempDates[j].getDateYear());
						tempDates[j].setDuration(newTempDates[j].getDuration());
					}

					newTempDates = nullptr;
					newTempDates = new Date[nSize];
					for (int j = 0; j < nSize; j++)
					{
						newTempDates[j].setDateMonth(tempDates[j].getDateMonth());
						newTempDates[j].setDateYear(tempDates[j].getDateYear());
						newTempDates[j].setDuration(tempDates[j].getDuration());
					}

				}
			}

			else if (iSwitch > 1 && iSwitch != 2)
			{
				nSize = (nSize + mCounter);
				tempDates = nullptr;
				tempDates = new Date[nSize];
				for (int j = 0; j < nSize; j++)
				{
					tempDates[j].setDateMonth(newTempDates[j].getDateMonth());
					tempDates[j].setDateYear(newTempDates[j].getDateYear());
					tempDates[j].setDuration(newTempDates[j].getDuration());
				}

				newTempDates = nullptr;											// g�r nya arrayer efter behov och kopierar �ver de gamla
				newTempDates = new Date[nSize];
				for (int j = 0; j < nSize; j++)
				{
					newTempDates[j].setDateMonth(tempDates[j].getDateMonth());
					newTempDates[j].setDateYear(tempDates[j].getDateYear());
					newTempDates[j].setDuration(tempDates[j].getDuration());
				}
			}


			int year = cdrDates[index].getDateYear();
			int startMonth = cdrDates[index].getDateMonth();


			for (int j = minLocation + 1; j < nSize; j++)
			{
				if ((startMonth == cdrDates[index].getDateMonth()) && (year == cdrDates[index].getDateYear()) && (cdrDates[index].getDateMonth() != 12))
				{
					newTempDates[j].setDateMonth(++startMonth);
					newTempDates[j].setDateYear(cdrDates[index].getDateYear());
					newTempDates[j].setDuration(0);
					if (startMonth != 12)
						startMonth++;
				}
				else if ((startMonth + 1) == 13)
				{
					if (cdrDates[index].getDateMonth() == startMonth)
					{
						startMonth = 1;
						year++;
						j--;

					}
					else if (startMonth == 12 && (newTempDates[j - 1].getDateMonth() == 12))	// h�r l�gger jag till "nya m�nader d�r samtalsl�ngd = 0
					{																			// fram till dess att mCounter har uppn�ts
						startMonth = 1;
						year++;
						j--;

					}
					else
					{
						newTempDates[j].setDateMonth(startMonth);
						newTempDates[j].setDateYear(year++);
						newTempDates[j].setDuration(0);
						startMonth = 1;
					}
				}
				else
				{
					newTempDates[j].setDateMonth(startMonth);
					newTempDates[j].setDateYear(year);
					newTempDates[j].setDuration(0);
					startMonth++;
				}
			}
			if (onlyDate == 1) // om det bara finns en m�nad d�r samtal gjordes
			{

				newTempDates[nSize - 1].setDateMonth(cdrDates[index].getDateMonth());
				newTempDates[nSize - 1].setDateYear(cdrDates[index].getDateYear());
				newTempDates[nSize - 1].setDuration(cdrDates[index].getDuration());
				triggerCondition = false;
			}
			else
			{
				newTempDates[nSize - 1].setDateYear(cdrDates[index + 1].getDateYear());
				newTempDates[nSize - 1].setDateMonth(cdrDates[index + 1].getDateMonth());
				newTempDates[nSize - 1].setDuration(cdrDates[index + 1].getDuration());
				minLocation = nSize - 1;
			}

			index++;
			iSwitch++;

			if (index == newArrSize - 1)
				triggerCondition = false;

		}
	}


		long double totalSum = 0;
		for (int i = 0; i < nSize; i++)
		{
			*jsonFile << "\"" << newTempDates[i].getDateYear() << "-";
			*jsonFile << fixed;
			*jsonFile << setfill('0') << setw(2) << newTempDates[i].getDateMonth() << "\": ";
			*jsonFile << fixed;																					// h�r l�ser jag ut alla datum som �r registrerade p� den aktuella MSIDSN
			*jsonFile << setprecision(2) << timeConvert(newTempDates[i].getDuration()) << "," << endl;

			totalSum += timeConvert(newTempDates[i].getDuration());

		}
		*jsonFile << "\"Total\":" << setprecision(2) << fixed << totalSum << endl;


		newTempDates = nullptr;
		tempArr = nullptr;
		tempDates = nullptr;

		delete[] newTempDates;
		delete[] tempArr;
		delete[] tempDates;

	}




double CDR::timeConvert(long long int duration)
{
	long double minutes;
	long double price;
	minutes = ((duration / 1000.00) / 60.00);		// l�ser in varje inviduel samtalsl�ngd som tillh�r ett nummer och g�r om milisekunderna till minuter.

	if (minutes > 0)	// om nummret har gjort samtal s� r�knas priset fram genom att +:a startpriset med minutpriset*(totala minuterna denna m�naden)
	{
		price = this->startPrice + (this->pricePerMinute*minutes);
	}
	else
		price = 0;	// om nummret inte har gjort n�gra samtal denna m�naden s� �r priset 0.

	return price;
}


long long int CDR::basePduration()	// n�r flera samtal ringdes fr�n samma m�nad s� var jag tvungen att l�gga till varje invudelt samtals startpris till det gemensamma m�nandspriset.
{									// denna funktionen r�knar ut exakt hur m�nga millisekunder som repsenterar baspriset. Dessa milisekunder l�ggs sedan p� n�r tv� stycken samtal ur samma
									// m�nad l�ggs ihop.

	long long int x = 0;
	bool value = true;

	while (value == true)
	{
		if ((x / 1000.00 / 60.00)*(this->pricePerMinute) == this->startPrice)
			value = false;
		else
		{
			x++;
		}
	}
	return x;
}




void CDR::setCDRNamePrice(string name, int basePrice, int minPrice)			//s�tter namnet p� JSON filen, Nummrets start och minutpris.
{	
	
	this->nameJSON = name;
	this->startPrice = basePrice;
	this->pricePerMinute = minPrice;

}

CDR::~CDR()
{
	date = nullptr;
	duration = nullptr;
	personCalled = nullptr;

	delete[] date;
	delete[] duration;
	delete[] personCalled;

}

CDR::CDR()
{
	this->callId = 1;
	this->number = 0;
	this->nameJSON = "";
	this->startPrice = 0;
	this->pricePerMinute = 0;

}


