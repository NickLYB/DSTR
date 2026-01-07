#ifndef PASSENGER_HPP
#define PASSENGER_HPP
#include <string>

using namespace std;

struct Passenger {
    string passengerId;
    string name;
    string seatRow;
    string seatColumn;
    string flightClass;

    Passenger* next;

    Passenger() {
        next = nullptr;
    }

    Passenger(string id, string n, string row, string col, string fClass) {
        this->passengerId = id;
        this->name = n;
        this->seatRow = row;
        this->seatColumn = col;
        this->flightClass = fClass;
        this->next = nullptr;
    }
};
#endif // PASSENGER_HPP
