#include <iostream>
#include <math.h>
#include <string>

typedef enum { bin, dec };

/*
* Funkcja zamienia podaną wartość binarną na dziesiętną
*
*/
long toDec(std::string bin)
{
	std::string cutNumber = bin;
	long result = 0;
	int power = pow(2, cutNumber.size() - 1); //Dodajemy cyfry od lewej do prawej, więc zaczynamy od najwyższej potęgi liczby 2
	for (int i = 0; i < cutNumber.size(); i++, power /= 2) {
		if (cutNumber[i] == '1') {
			result += power; //Dodajemy daną potęgę liczby 2
		}
	}

	return result;
}

/*
* Funkcja zamienia podaną wartość dziesiętną na binarną
*
*/
std::string toBin(long dec)
{
	if (dec == 0) //Opisuję "0" jako szczególny przypadek, gdyż log(0) jest niepoprawną operacją
		return "0";
	int power = log2(dec); //Korzystam z logarytmu, aby znaleźć najwyższą potęgę liczby 2, która zmieści się w podanej wartości

	std::string result;
	for (long reference = pow(2, power); reference > 0; reference /= 2) {
		if (dec >= reference) { //Jeżeli w konwertowanej liczbie dziesiętnej zmieści się dana potęga liczby 2,
								//odejmujemy ją i dodajemy do wynikowej liczby binarnej
			result += "1";
			dec -= reference;
		}
		else {
			result += "0";
		}
	}

	return result;
}

int main()
{
	bool mode = bin;
	std::string input;

	while (1) {
		if (mode == bin) {
			std::cout << "Tryb dzialania: bin --> dec\n";
		}
		else {
			std::cout << "Tryb dzialania: dec --> bin\n";
		}
		std::cout << "Aby zmienic tryb dzialania, wpisz \"zmiana\"\n";
		std::cout << "Aby opuscic program, wpisz \"wyjscie\"\n\n";

		if (mode == bin) {
			std::cout << "Podaj dodatnia liczbe binarna: ";
		}
		else {
			std::cout << "Podaj dodatnia liczbe dziesietna: ";
		}

		std::cin >> input;

		if (input == "wyjscie")
			break;
		else if (input == "zmiana")
			mode == bin ? mode = dec : mode = bin;
		else {
			char* p;
			long long val = strtol(input.c_str(), &p, 0); //Funkcja zwraca wartość typu int, więc wynik będzie nie większy niż 2147483647

			std::cout << std::endl;

			if (mode == bin) {
				if (input.find_first_not_of("01") != std::string::npos)
					std::cout << "Podana wartosc nie jest prawidlowa!" << std::endl;
				else
					std::cout << "Wynik dziesietny: " << toDec(input) << std::endl;
			}
			else {
				if (input.find_first_not_of("0123456789") != std::string::npos)
					std::cout << "Podana wartosc nie jest prawidlowa!" << std::endl;
				else if (val >= 2147483647)
					std::cout << "Podana wartosc jest zbyt wielka!" << std::endl;
				else
					std::cout << "Wynik binarny: " << toBin(val) << std::endl;
			}

		}
		std::cout << std::endl;

	}


	//for (int i = 0; i < 256; i++) {
	//    std::cout << toBin(i) << ((i == toDec(toBin(i))) ? "==" : "!=") << toDec(toBin(i)) << std::endl;
	//}
	//int z = 0;
}