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

class ArrayPassenger {
private:
    std::string id;
    std::string name;
    std::string seatRow;
    std::string seatColumn;
    std::string classType;

public:
    ArrayPassenger(std::string id,
                   std::string name,
                   std::string row,
                   std::string column,
                   std::string classType)
        : id(id),
          name(name),
          seatRow(row),
          seatColumn(column),
          classType(classType) {}

    std::string getID() const { return id; }
    std::string getName() const { return name; }
    std::string getSeatRow() const { return seatRow; }
    std::string getSeatColumn() const { return seatColumn; }
    std::string getClassType() const { return classType; }
};

#endif // PASSENGER_HPP
