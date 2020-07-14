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


//programmet l�ser in rader ur en textfil med hj�lp av instream och argument till filen, raderna �r uppdelade med 4 l�nga int str�ngar som �r delade med ett komma,
// programmet l�ser in en rad i taget och analyserar denna, samt g�r om den fr�n en string till en long long int.
// Dessa long long int variablerna sparas i en MSISDN class som �r ett "nummer", detta nummret inneh�ller totalt antal samtal, dessa samtals l�ngd, och datum
// N�r MSIDSN sen ska bilda en CDR, s� r�knas alla samtal per m�nad ihop i en s� kallad createDateList funktion d�r l�ngden och kostnaden r�knas fram f�r varje m�nad fr�n dess att 
// f�rsta samtalet ringdes till det sista, �ven m�nader d�r inga samtal ringdes r�knas in, och dessa har kostnad 0. Programmet skriver sen ut CDR f�r alla de olika MSIDN i en jsonfil genom att 
// anv�nda ofstream.

// den st�rsta delen i projektet �r den del d�r alla de olika samtalen i en m�nad l�ggs ihop till en �nda date class, sen �ven delen d�r "tomma" m�nader l�ggs till mellan tv� samtal �r v�ldigt
// stor och on�digt omst�ndig.




int main(int argc, char* argv[])
{
	int totalCalls = 0;
	string jsonName;
	int bPrice, mPrice;

	CDR* MSISDN = new CDR[totalCalls];				// g�r en CDR ptr som pekar p� en array med just nu 0 st CDR objekt.

	ifstream file(argv[1]);							//�ppnar filen

	if (file.is_open() && file.good()) {
		string line;
		while (getline(file, line)) {								// s� l�nge linjen har rader
			int trigger = 0;


			for (int i = 0; i < totalCalls; i++)
			{
				if (MSISDN[i].getNumber() == checkNumber(line))		//kollar om nummret redan finns som ett objekt
				{
					MSISDN[i].defineLine(line);						// om objektet redan finns s� l�ggs det nya samtalet till i objektets datavariabler
					trigger = 1;

				}
			}


			if (totalCalls >= 1 && trigger != 1)					// om linjen som l�ses in har ett nytt nummer s� l�ggs denna till s� l�nge den inte �r den f�rsta.
			{
				totalCalls++;
				CDR* tempPtr = new CDR[totalCalls];
				for (int i = 0; i < totalCalls - 1; i++)
				{
					tempPtr[i] = MSISDN[i];
				}

				// en ny dynamic array skapas som inneh�ller de gamla + det nya nummret.
				MSISDN = nullptr;
				delete[] MSISDN;

				MSISDN = new CDR[totalCalls];

				for (int i = 0; i < totalCalls - 1; i++)
				{
					MSISDN[i] = tempPtr[i];
				}

				tempPtr = nullptr;
				delete[] tempPtr;

				MSISDN[totalCalls - 1].defineLine(line);	// det nya nummret, och dess "linje" analyseras av defineline funktionen d�r den f�r fram ,vem som ringdes, date och tid.
			}
			else if (totalCalls == 0)
			{												// f�rsta omg�ngen programet l�ser in en linje och s�tter in denna i en, en s� l�nge tom CDR objekt array.
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
		if (i != totalCalls - 1)			// skriver ut en json fil. inneh�llet som i exemplet, 
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