#include <iostream>
#include <chrono>
#include <thread>
#include <string>

typedef enum {speedingUp, turningLeft, turningRight, slowingDown, turnedOff};

class car {
private:

    /*
    * Parametry techniczne samochodu
    */

    double maxSpeed; // km/h
    double currentSpeed; // km/h
    double acceleration; // km/h/s
    double brakingSpeed; // km/h/s
    double steeringWheelTurn; // stopnie
    double mileage; //km

    /*
    * Marka samochodu
    */
    std::string brand;

    /*
    * Obecny stan samochodu, w tym manewr który jest wykonywany
    */
    int state;

    /*
    * Funkcja daje znać kierowcy, jaki manewr wykonać
    */
    int changeState(int newState) {
        if (state != turnedOff) { //Jeżeli samochód jest zgaszony, jedynym sposobem na zmianę jego stanu jest odpalenie go
            state = newState;
            return 0;
        }
        else
            return -1;
    }

    /*
    * Funkcja symuluje działanie samochodu biorąc pod uwagę manewr, który aktualnie jest wykonywany
    * Funkcja przerywa działanie dopiero w momencie zgaszenia samochodu
    */
    void drive(int poolingInterval /* ms */) {       
        std::chrono::milliseconds step(poolingInterval);
        while (state != turnedOff) {
            std::this_thread::sleep_for(step);

            mileage += currentSpeed * poolingInterval / 1000 / 3600;

            switch (state) {
                case speedingUp:
                    currentSpeed += acceleration * poolingInterval / 1000;
                    if (currentSpeed > maxSpeed)
                        currentSpeed -= acceleration * 1.01 * poolingInterval / 1000;
                    if (steeringWheelTurn > 0) //Jeżeli kierownica jest skręcona, próbujemy ją wyprostować
                        steeringWheelTurn -= (steeringWheelTurn * 1.0 * poolingInterval / 1000) + 0.5;
                    else if (steeringWheelTurn < 0)
                        steeringWheelTurn -= (steeringWheelTurn * 1.0 * poolingInterval / 1000) - 0.5;
                    break;

                case turningLeft:
                    currentSpeed -= currentSpeed * 0.05 * poolingInterval / 1000; //Samochód traci część prędkości na zakręcie
                    steeringWheelTurn -= 45 * poolingInterval / 1000; //Kręcimy odwrotnie do ruchu wskazówek zegara, więc zmniejszamy wartość skrętu kierownicy
                    if (steeringWheelTurn > 720) //Umowny limit skrętu kierownicy
                        steeringWheelTurn = 720;
                    break;

                case turningRight:
                    currentSpeed -= currentSpeed * 0.05 * poolingInterval / 1000; //Samochód traci część prędkości na zakręcie
                    steeringWheelTurn += 45 * poolingInterval / 1000;
                    if (steeringWheelTurn < -720) //Umowny limit skrętu kierownicy
                        steeringWheelTurn = -720;
                    break;

                case slowingDown:
                    currentSpeed -= brakingSpeed * poolingInterval / 1000;
                    if (currentSpeed < 0.0)
                        currentSpeed = 0.0;
                    if (steeringWheelTurn > 0) //Jeżeli kierownica jest skręcona, próbujemy ją wyprostować
                        steeringWheelTurn -= (steeringWheelTurn * 1.0 * poolingInterval / 1000) + 0.5;
                    else if (steeringWheelTurn < 0)
                        steeringWheelTurn -= (steeringWheelTurn * 1.0 * poolingInterval / 1000) - 0.5;
                    break;

                default:
                    //Istnieje możliwość, że stan obiektu zostanie zmieniony asynchronicznie,
                    //zanim ta funkcja skonczy dzialanie. Nie powoduje to jednak znaczącego problemu.
                    break;
            }
        }
    }
public:
    /*
    * W konstruktorze należy podać parametry techniczne samochodu
    */
    car(double maxSpd, double acc, double brakingSpd, double mlg, std::string brnd) {
        maxSpeed = maxSpd;
        acceleration = acc;
        brakingSpeed = brakingSpd;
        mileage = mlg;
        brand = brnd;
        steeringWheelTurn = 0.000001;
        currentSpeed = 0.0;
        state = turnedOff;
    }

    /*
    * Gettery pól, które mają być dostępne do wglądu
    * Nie ma setterów, gdyż nie mamy możliwości zmienić parametrów samochodu z zewnątrz
    * Przykładowo, nie możemy ustawić przebiegu samochodu na niższy - zmienia się on automatycznie,
    * wraz z przebytymi kilometrami
    */
    std::string getBrand() {
        return brand;
    }
    double getCurrentSpeed() {
        return currentSpeed;
    }
    double getWheelTurn() {
        return steeringWheelTurn;
    }
    double getMileage() {
        return mileage;
    }

    /*
    * Funkcja wywołuje metodę "drive", która periodycznie aktualizuje parametry samochodu
    * w zależności od stanu, w którym się znajduje
    * Jeżeli metoda "drive" już działa, nie zostanie wywołana ponownie
    */
    int turnOn() {
        if (state == turnedOff) {
            state = speedingUp;
            std::thread t(&car::drive, this, 100);
            t.detach();
            return 0;
        }
        else
            return -1;
    }

    /*
    * Funkcje odpowiedzialne za zmianę stanu samochodu (bądź wykonywanego przez niego manewru)
    */
    int speedUp() {
        return changeState(speedingUp);
    }
    int slowDown() {
        return changeState(slowingDown);
    }
    int turnLeft() {
        return changeState(turningLeft);
    }
    int turnRight() {
        return changeState(turningRight);
    }

    /*
    * Funkcja próbuje ustawić stan samochodu na zgaszony
    * Nie powiedzie się to, jeśli samochód dalej jest w ruchu
    */
    int turnOff() {
        if (state != turnedOff && currentSpeed < 0.01) {
            state = turnedOff;
            return 0;
        }
        else
            return -1;
    }
};

/*
* Funkcja periodycznie odczytuje parametry samochodu i wypisuje je w konsoli
*/
void registerParameters(car &c, int poolingInterval) {
    while (1) {
        std::chrono::milliseconds step(poolingInterval);
        std::this_thread::sleep_for(step);
        //Wszystkie wątki korzystają z jednego strumienia wyjścia, więc zanim podam parametry,
        //łączę je w jednego stringa
        std::string log = c.getBrand() + ":  Obecna predkosc: " + std::to_string(c.getCurrentSpeed())
            + "km/h  Promien skretu kierownicy: " + std::to_string(c.getWheelTurn())
            + "  Przebieg samochodu: " + std::to_string(c.getMileage()) + "km\n";
        std::cout << log;
    }
}

/*
* Samochód wykona 5 okrążeń po torze o kształcie ósemki
* Passat wykonuje okrążenia na własnym, mniejszym torze
*/
void driveAround(car &c) {
    c.turnOn();
    for (int i = 0; i < 5; i++) {
        c.speedUp();
        std::this_thread::sleep_for(std::chrono::milliseconds(15000));
        c.turnRight();
        std::this_thread::sleep_for(std::chrono::milliseconds(10000));
        c.speedUp();
        std::this_thread::sleep_for(std::chrono::milliseconds(15000));
        c.turnLeft();
        std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    }
    while (c.turnOff() != 0) //Po ukończeniu toru, hamujemy aż do zatrzymania samochodu
        c.slowDown();
}

int main()
{

    //Tworzę trzy przykładowe samochody, po czym podłączam do nich funkcje odpowiedzialne
    //za prowadzenie ich, oraz wypisywanie ich parametrów

    //car Ferrari(340.0, 33.0, 20.0, 10000.0, "Ferrari");
    //car Lamborgini(341.0, 40.0, 25.0, 15000.0, "Lamborgini");
    car Passat(190.0, 8.5, 10.0, 300000.0, "Passat");

    //std::thread(registerParameters, std::ref(Ferrari), 1000).detach();
    //std::thread(driveAround, std::ref(Ferrari)).detach();

    //std::thread(registerParameters, std::ref(Lamborgini), 1000).detach();
    //std::thread(driveAround, std::ref(Lamborgini)).detach();

    std::thread(registerParameters, std::ref(Passat), 1000).detach();
    std::thread(driveAround, std::ref(Passat)).detach();

    std::this_thread::sleep_for(std::chrono::minutes(5));
}