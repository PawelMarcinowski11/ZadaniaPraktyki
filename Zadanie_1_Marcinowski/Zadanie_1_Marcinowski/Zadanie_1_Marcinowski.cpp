#include <iostream>
#include <fstream>
#include <list>

int main()
{
    int count = 0;
    int erroneous = 0;
    std::list<char> correctObjects;

    std::ifstream input("input.txt", std::ios::binary);
    if (input.is_open()) {
        char buffer;
        input >> std::noskipws; //Ustawiam, aby strumień nie pomijał spacji i innych białych znaków
        while (input >> buffer) {
            char id = (buffer >> 4) & 0x0F; //Pierwsze cztery bity
            char message = (buffer >> 1) & 0x07; //Trzy kolejne bity
            char checkBit = buffer & 0x01; //Ostatni bit

            char checkSum = id + checkBit;

            if (checkSum % 2 == 0 && message != 0) { //Jeżeli suma id obiektu i jego bitu kontrolnego jest parzysta,
                                                     //to bit kontrolny został ustawiony prawidłowo
                                                     //Ponadto, wiadomość składająca się z samych zer jest uznawana za błędną
                correctObjects.push_back(buffer);
            }
            else {
                erroneous++;
            }
            count++;
        }
    }
    else {
        std::cerr << "Blad otwierania pliku";
        return 0;
    }

    std::ofstream output("output.txt");
    output << count << std::endl;
    output << erroneous << std::endl;
    for (char c : correctObjects) {
        output << c;
    }
    output.close();
}