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


//programmet läser in rader ur en textfil med hjälp av instream och argument till filen, raderna är uppdelade med 4 långa int strängar som är delade med ett komma,
// programmet läser in en rad i taget och analyserar denna, samt gör om den från en string till en long long int.
// Dessa long long int variablerna sparas i en MSISDN class som är ett "nummer", detta nummret innehåller totalt antal samtal, dessa samtals längd, och datum
// När MSIDSN sen ska bilda en CDR, så räknas alla samtal per månad ihop i en så kallad createDateList funktion där längden och kostnaden räknas fram för varje månad från dess att 
// första samtalet ringdes till det sista, även månader där inga samtal ringdes räknas in, och dessa har kostnad 0. Programmet skriver sen ut CDR för alla de olika MSIDN i en jsonfil genom att 
// använda ofstream.

// den största delen i projektet är den del där alla de olika samtalen i en månad läggs ihop till en ända date class, sen även delen där "tomma" månader läggs till mellan två samtal är väldigt
// stor och onödigt omständig.




int main(int argc, char* argv[])
{
	int totalCalls = 0;
	string jsonName;
	int bPrice, mPrice;

	CDR* MSISDN = new CDR[totalCalls];				// gör en CDR ptr som pekar på en array med just nu 0 st CDR objekt.

	ifstream file(argv[1]);							//öppnar filen

	if (file.is_open() && file.good()) {
		string line;
		while (getline(file, line)) {								// så länge linjen har rader
			int trigger = 0;


			for (int i = 0; i < totalCalls; i++)
			{
				if (MSISDN[i].getNumber() == checkNumber(line))		//kollar om nummret redan finns som ett objekt
				{
					MSISDN[i].defineLine(line);						// om objektet redan finns så läggs det nya samtalet till i objektets datavariabler
					trigger = 1;

				}
			}


			if (totalCalls >= 1 && trigger != 1)					// om linjen som läses in har ett nytt nummer så läggs denna till så länge den inte är den första.
			{
				totalCalls++;
				CDR* tempPtr = new CDR[totalCalls];
				for (int i = 0; i < totalCalls - 1; i++)
				{
					tempPtr[i] = MSISDN[i];
				}

				// en ny dynamic array skapas som innehåller de gamla + det nya nummret.
				MSISDN = nullptr;
				delete[] MSISDN;

				MSISDN = new CDR[totalCalls];

				for (int i = 0; i < totalCalls - 1; i++)
				{
					MSISDN[i] = tempPtr[i];
				}

				tempPtr = nullptr;
				delete[] tempPtr;

				MSISDN[totalCalls - 1].defineLine(line);	// det nya nummret, och dess "linje" analyseras av defineline funktionen där den får fram ,vem som ringdes, date och tid.
			}
			else if (totalCalls == 0)
			{												// första omgången programet läser in en linje och sätter in denna i en, en så länge tom CDR objekt array.
				totalCalls++;
				MSISDN = nullptr;
				delete[] MSISDN;
				MSISDN = new CDR[totalCalls];

				MSISDN[totalCalls - 1].defineLine(line);
			}
		}
	}
	else {
		cout << "Failed to open file..";
		exit(0);
	}


	cout << "Please enter JSON filename: ";
	cin >> jsonName;
	cout << "\nEnter baseprice, for a call: ";
	cin >> bPrice;
	cout << "\nEnter minuteprice: ";
	cin >> mPrice;


	for (int i = 0; i < totalCalls; i++)
	{
		MSISDN[i].setCDRNamePrice(jsonName, bPrice, mPrice); // ger varje telefonnummer jsonNamn, startpris, minutpris
	}




	ofstream jsonFile;
	jsonFile.open(MSISDN[0].getJsonName());
	jsonFile << "{\n \"Invoices\":[" << endl;
	for (int i = 0; i < totalCalls; i++)
	{
		jsonFile << "  {" << endl;
		MSISDN[i].CDRinfo(&jsonFile);
		if (i != totalCalls - 1)			// skriver ut en json fil. innehållet som i exemplet, 
			jsonFile << "  }," << endl;
		else
			jsonFile << "  }" << endl;
	}
	jsonFile << " ]\n}" << endl;
	jsonFile.close();

	return 0;

	MSISDN = nullptr;
	delete[] MSISDN;
}