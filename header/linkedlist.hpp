#ifndef LINKEDLIST_HPP
#define LINKEDLIST_HPP

#include "Passenger.hpp"
#include <string>
using namespace std;

class LinkedListSystem {
private:
    Passenger* head;
public:
    LinkedListSystem();
    ~LinkedListSystem();
    void loadFromFile(string filename);
    void run();

    void insertPassenger();
    void deletePassenger();
    void searchPassenger();
    void displayManifest();
};

#endif // LINKEDLIST_HPP