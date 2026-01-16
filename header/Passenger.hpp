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


    Passenger() {}

    Passenger(string id, string n, string row, string col, string fClass) {
        this->passengerId = id;
        this->name = n;
        this->seatRow = row;
        this->seatColumn = col;
        this->flightClass = fClass;
    }
};

struct SNode {
    Passenger data;
    SNode* next;

    SNode(Passenger p) {
        this->data = p;
        this->next = nullptr;
    }
};

// struct DNode {
//     Passenger data;
//     DNode* prev;
//     DNode* next;

//     DNode(Passenger p) {
//         this->data = p;
//         this->prev = nullptr;
//         this->next = nullptr;
//     }
// };
#endif // PASSENGER_HPP
