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


using namespace std;


void CDR::defineLine(string line)
{	
	
	long long int value = 0;
	int counter = 0;
	string text;
	stringstream iss(line);

	while (getline(iss, text, ','))			//läser in text upp till "," från linjen, använder en counter för att se vart på linjen man läser in.
	{
		
		istringstream ss(text);		// streamar om string värdet som "text" har till en long long int och lägger denna i value.
		ss >> value;

		if (counter == 0)				// första gången så läser den in telefonnummret
			setNumber(value);			
		else if (counter == 1)			// andra gången är personen den ringer
			setCalled(value);
		else if (counter == 2)			// läser in datumet och sätter in denna
			setDate(value);
		else if (counter == 3)			// läser in längden för samtalet.
			setDuration(value);
		
		counter++;
		
	}
	


}


void CDR::setNumber(long long int value)
{
	if (this->number == value)			// om nummret redan finns som ett objekt, så läggs ny data till dess variabler.
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
		delete[] duration;						// denna kod ökar platsen på nummrets date,called,duration arrayer och lägger in det nya samtalet i dessa.
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
	this->number = value;			// om nummret inte redan finns så läggs det nya nummret in här
}
void CDR::setCalled(long long int value)
{
	this->personCalled[callId-1] = value;			// lägger till personen som ringdes nummer längst bak i arrayen
}
void CDR::setDate(long long int value)
{
	this->date[callId-1] = value;		// lägger till datumet längst bak i samtaldatum arrayen
}
void CDR::setDuration(long long int value)
{
	this->duration[callId-1] = value;			// lägger till längden av samtalet längst bak i duration arrayen
}

long long int CDR::getNumber()
{
	return this->number;
}

string CDR::getJsonName()
{	
	string name = this->nameJSON;					// lägger till .json på det namn på textfilen som personen önskade, så att det blir en json fil
	name += ".json";
	return name;
}

void CDR::CDRinfo(ofstream *jsonFile)
{
	*jsonFile << "   \"MSISDN\":\"" << number << "\"," << endl;
		 createDateList(date,duration, jsonFile );			// när JSON filen skapas så skriver denna funktion ut info om detta nummret och alla dess samtal

}


void CDR::createDateList(long long int* date, long long int* duration, ofstream* jsonFile)
{
	long long int* arrPtr = new long long int[callId];				// gör nya dynamiska arrayer som håller alla datum och samtalslängder som tillhör nummret.
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
				tempDuration = durationPtr[i];			// sorterar de två arrayerna i storleksordning liten->störst.
				arrPtr[i] = arrPtr[j];
				durationPtr[i] = durationPtr[j];
				arrPtr[j] = tempInt;
				durationPtr[j] = tempDuration;
			}
		}
	}


	Date* cdrDates = new Date[callId]; // gör en ny class där objekten fungerar som månader som tillhör CDR classen.
	time_t* time = new time_t[callId];
	tm* dateAndMonth = new tm[callId];

	for (int i = 0; i < callId; i++)				// gör en time_t array som håller datumen i sekunder.
	{													// jag stoppar sedan in time_t objekten i en tm array där jag anpassar time_t objekten efter lokaltid.
		time[i] = arrPtr[i];
		dateAndMonth[i] = *localtime(&time[i]);
	}



	for (int i = 0; i < callId; i++)
	{
		cdrDates[i].setDateMonth(dateAndMonth[i].tm_mon + 1);
		cdrDates[i].setDateYear(dateAndMonth[i].tm_year + 1900);		//jag initierar de olika datum objekten med data från tm arrayen, samt tiden av samtalen för varje månad
		cdrDates[i].setDuration(durationPtr[i]);
	}



	Date* tempArr = nullptr;
	int newArrSize = callId;
	for (int i = 0; i <= newArrSize; i++)			// denna for loop går igenom alla datum objekt i arrayen och lägger ihop de datum objekt som representerar samma månad.
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
						cdrDates[x] = cdrDates[x + 1];	// tar bort objektet från arrayen.
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
						cdrDates[x] = cdrDates[x + 1];	// tar bort objektet från arrayen.
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
	int minLocation = 0;			// dessa variabler användes i den kommande while loopen
	Date* tempDates = nullptr;
	Date* newTempDates = nullptr;
	int yCounter = 0;
	int nSize;
	int onlyDate = 0;

	while (triggerCondition)		// denna loop lägger till månader i cdrDates arrayen, månaderna som läggs till ligger "emellan" de månaderna där det finns samtal gjorda, dvs från första månad
	{								// där samtal gjordes till den sista månaden. Dessa nya månader som läggs till har samtalslängd = 0.
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
					mCounter++;						// här uppe så räknar jag fram totalt hur många månader som ligger emellan två "orginal cdrDates"
					monthAndYear = false;			// resultatet läggs i mcounter.
				}
			}
			else if (inRange(cdrDates[index + 1].getDateMonth()))	// kollar om det finns några fler samtal månaden efter denna
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

			int year = cdrDates[index].getDateYear();				// iden är att jag sätter "första" värdet från cdrDates på 0 i den nya arrayen, sen så fyller jag på med "skräp dates"
			int startMonth = cdrDates[index].getDateMonth();		// fram till att jag når det "andra" värdet i cdrDates. loopen fortsätter såhär tills den aldra sista månaden
																	// där ett samtal ringdes.
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
					else if (startMonth == 12 && (tempDates[j - 1].getDateMonth() == 12))		// här lägger jag till "nya månader där samtalslängd = 0
					{																			// fram till dess att mCounter har uppnåts
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
			if (onlyDate == 1) // om det bara finns en månad där samtal gjordes
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
					{																		// gör nya arrayer efter behov och kopierar över de gamla
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

				newTempDates = nullptr;											// gör nya arrayer efter behov och kopierar över de gamla
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
					else if (startMonth == 12 && (newTempDates[j - 1].getDateMonth() == 12))	// här lägger jag till "nya månader där samtalslängd = 0
					{																			// fram till dess att mCounter har uppnåts
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
			if (onlyDate == 1) // om det bara finns en månad där samtal gjordes
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
			*jsonFile << fixed;																					// här läser jag ut alla datum som är registrerade på den aktuella MSIDSN
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
	minutes = ((duration / 1000.00) / 60.00);		// läser in varje inviduel samtalslängd som tillhör ett nummer och gör om milisekunderna till minuter.

	if (minutes > 0)	// om nummret har gjort samtal så räknas priset fram genom att +:a startpriset med minutpriset*(totala minuterna denna månaden)
	{
		price = this->startPrice + (this->pricePerMinute*minutes);
	}
	else
		price = 0;	// om nummret inte har gjort några samtal denna månaden så är priset 0.

	return price;
}


long long int CDR::basePduration()	// när flera samtal ringdes från samma månad så var jag tvungen att lägga till varje invudelt samtals startpris till det gemensamma månandspriset.
{									// denna funktionen räknar ut exakt hur många millisekunder som repsenterar baspriset. Dessa milisekunder läggs sedan på när två stycken samtal ur samma
									// månad läggs ihop.

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




void CDR::setCDRNamePrice(string name, int basePrice, int minPrice)			//sätter namnet på JSON filen, Nummrets start och minutpris.
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


