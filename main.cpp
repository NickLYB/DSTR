#include "header/Passenger.hpp" //struct Passenger
#include "header/array.hpp" //array functions header
#include "header/linkedlist.hpp" //linked list functions header
#include "header/utils.hpp" //utility functions header

#include <iostream>

using namespace std;

int main() {
    int choice;
    string filename = "flight_passenger_data.csv";

    do{
        clearScreen();

        cout << "Data Structure Menu" << endl;
        cout << "-------------------" << endl;
        cout << "1. Array" << endl;
        cout << "2. Linked List" << endl;
        cout << "-------------------" << endl;
        cout << "0. Exit" << endl;
        cout << "Select an option: ";

        if (!(cin >> choice)) {
            cout << "Invalid input! Please enter a number." << endl;
            flushInput();
            waitForEnter(); 
            continue;
        }
        flushInput();

        switch(choice) {
            case 1: {
                ArraySystem arraySystem;
                arraySystem.loadFromFile(filename);
                arraySystem.run();
                break;
            }
            case 2: {
                LinkedListSystem listSystem;
                listSystem.loadFromFile(filename);
                listSystem.run();
                break;
            }
            case 0:
                cout << "Exiting program." << endl;
                break;
            default:
                cout << "Invalid choice. Please select again." << endl;
                waitForEnter();
                break;
        }
    } while (choice != 0);
    return 0;
}