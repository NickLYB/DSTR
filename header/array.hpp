#ifndef ARRAY_HPP
#define ARRAY_HPP

#include "Passenger.hpp"
#include <string>

using namespace std;

const int MAX_ROWS = 460;
const int MAX_COLUMNS = 26;
// Maximum possible passengers is Rows * Columns
const int MAX_PASSENGERS = MAX_ROWS * MAX_COLUMNS;

class ArraySystem {
private:
    // Requirement 1: 2D Array for Seat Management
    Passenger* seats[MAX_ROWS][MAX_COLUMNS];

    // Requirement 2: 1D Array for Passenger List
    Passenger* passengerList[MAX_PASSENGERS];
    int passengerCount; // Keeps track of how many passengers are in the list

public:
    ArraySystem();
    ~ArraySystem();

    void loadFromFile(string filename);
    void run();

    void insertPassenger();
    void deletePassenger();
    void searchPassenger();
    void displayManifest();
};
#endif // ARRAY_HPP
