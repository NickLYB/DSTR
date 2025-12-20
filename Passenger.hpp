#include <string>

using namespace std;

struct Passenger {
    string passengerId;
    string name;
    string seatRow;
    string seatColumn;
    string flightClass;

    Passenger(string id, string n, string row, string col, string fClass) {
        this->passengerId = id;
        this->name = n;
        this->seatRow = row;
        this->seatColumn = col;
        this->flightClass = fClass;
    }
};
