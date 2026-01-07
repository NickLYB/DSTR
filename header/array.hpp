#ifndef ARRaY_HPP
#define ARRaY_HPP

#include "Passenger.hpp"
#include <string>

using namespace std;

const int MAX_ROWS = 30;
const int MAX_COLUMNS = 6;

class ArraySystem {
private:
    Passenger* seats[MAX_ROWS][MAX_COLUMNS];
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
#endif // ARRaY_HPP