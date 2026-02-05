#include "../header/linkedlist.hpp"
#include "../header/utils.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

using namespace std;
using namespace std::chrono;

LinkedListSystem::LinkedListSystem() {
    sHead = nullptr;
    sTail = nullptr;
}
LinkedListSystem::~LinkedListSystem() {
    
    SNode* sCurrent = sHead;
    while(sCurrent != nullptr){
        SNode* nextNode = sCurrent -> next;
        delete sCurrent;
        sCurrent = nextNode; 
    }
    
}
void LinkedListSystem::loadFromFile(string filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }
    string line;
    getline(file, line); // Skip header line

    size_t passengerCount = 0;
    auto start = high_resolution_clock::now();
    while (getline(file, line)) {
        stringstream ss(line);
        string id, name, row, col, fClass;

        getline(ss, id, ',');
        getline(ss, name, ',');
        getline(ss, row, ',');
        getline(ss, col, ',');
        getline(ss, fClass, ',');

        Passenger p(id, name, row, col, fClass);

        SNode* newSNode = new SNode(p);
        if (sHead == nullptr) {
            sHead = newSNode;
            sTail = newSNode; 
        } else {
            sTail->next = newSNode; 
            sTail = newSNode;       
        }
        passengerCount++;

    }
    auto stop = high_resolution_clock::now();
    auto ms = duration_cast<milliseconds>(stop - start).count();
    file.close();
    cout << endl;
    cout << "Loading passengers from file: " << filename << endl;
    cout << "Passengers loaded into Linkedlist successfully." << endl;

    size_t nodeHeap = passengerCount * sizeof(SNode);
    cout << "System Loaded. Accessing Linked List System..." << endl;
    cout << "Passenger object size: " << sizeof(Passenger) << " bytes" << endl;
    cout << "SNode size: " << sizeof(SNode) << " bytes" << endl;
    cout << "Estimated heap for nodes: " << nodeHeap << " bytes" << endl;
    cout << "Estimated total (minimum): " << nodeHeap << " bytes" << endl;
    cout << "Loading time: " << ms << " ms" << endl;
    cout << "Passengers loaded into Linkedlist successfully." << endl;
    waitForEnter();
}
bool LinkedListSystem::saveToFile(string filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file for writing: " << filename << endl;
        return false;
    }
    // Write header
    file << "PassengerID,Name,SeatRow,SeatColumn,FlightClass\n";
    SNode* current = sHead;
    while (current != nullptr) {
        file << current->data.passengerId << ","
             << current->data.name << ","
             << current->data.seatRow << ","
             << current->data.seatColumn << ","
             << current->data.flightClass << "\n";
        current = current->next;
    }
    file.close();
    return true;
}

void LinkedListSystem::run() {
    int choice;
    do {
        clearScreen();
        cout << "==========================" << endl;
        cout << "Linked List Menu" << endl;
        cout << "==========================" << endl;
        cout << "1. Insert Passenger" << endl;
        cout << "2. Delete Passenger" << endl;
        cout << "3. Search Passenger" << endl;
        cout << "4. Display Manifest" << endl;
        cout << "--------------------------" << endl;
        cout << "0. Back to Main Menu" << endl;
        cout << "==========================" << endl;
        cout << "Select an option: ";
        
        if (!(cin >> choice)) {
            cout << "Invalid input! Please enter a number." << endl;
            choice = -1;
            flushInput();
            waitForEnter(); 
            continue;
        }
        flushInput();

        switch(choice) {
            case 1:
                // Code to insert passenger
                LinkedListSystem::insertPassengerMenu();
                break;
            case 2:
                // Code to delete passenger
                LinkedListSystem::deletePassenger();
                break;
            case 3:
                // Code to search passenger
                LinkedListSystem::searchPassenger();
                break;
            case 4:
                // Code to display manifest
                LinkedListSystem::ManifestnSeatReport();

                break;
            case 0:
                cout << "Returning to Main Menu." << endl;
                waitForEnter();
                break;
            default:
                cout << "Invalid choice. Please select again." << endl;
                waitForEnter();
                break;
        }
    } while(choice != 0);
}