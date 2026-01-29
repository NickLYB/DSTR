#include "../header/array.hpp"
#include "../header/utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

// Constructor
ArraySystem::ArraySystem() {
    passengerCount = 0; // Initialize counter to 0

    // Initialize 2D Array
    for (int i = 0; i < MAX_ROWS; i++) {
        for (int j = 0; j < MAX_COLUMNS; j++) {
            seats[i][j] = nullptr;
        }
    }

    // Initialize 1D Array
    for (int i = 0; i < MAX_PASSENGERS; i++) {
        passengerList[i] = nullptr;
    }
}

// Destructor
ArraySystem::~ArraySystem() {
    // Only delete from the 2D array to avoid double-freeing memory
    // (Since 1D array points to the same objects)
    for (int i = 0; i < MAX_ROWS; i++) {
        for (int j = 0; j < MAX_COLUMNS; j++) {
            if (seats[i][j] != nullptr) {
                delete seats[i][j];
                seats[i][j] = nullptr;
            }
        }
    }
}

void ArraySystem::loadFromFile(string filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }
    string line;
    getline(file, line); // Skip header

    while (getline(file, line)) {
        stringstream ss(line);
        string id, name, rowStr, colStr, fClass;

        // Read CSV components
        getline(ss, id, ',');
        getline(ss, name, ',');
        getline(ss, rowStr, ',');
        getline(ss, colStr, ',');
        getline(ss, fClass, ',');

        try {
            // Convert Row and Column
            int r = stoi(rowStr) - 1;
            char cChar = colStr[0];
            int c = toupper(cChar) - 'A';

            // Validate and Insert
            if (r >= 0 && r < MAX_ROWS && c >= 0 && c < MAX_COLUMNS) {
                if (seats[r][c] == nullptr) {
                    Passenger* newP = new Passenger(id, name, rowStr, colStr, fClass);
                    seats[r][c] = newP;
                    passengerList[passengerCount] = newP;
                    passengerCount++;
                }
            }
        } catch (...) {
            continue;
        }
    }
    file.close();

    // --- CHANGE IS HERE ---
    // I removed the "waitForEnter()" so it goes DIRECTLY to the menu.
    cout << "System Loaded. Accessing Array System..." << endl;
}

void ArraySystem::run() {
    int choice;
    do {
        clearScreen();
        cout << "Array System Menu" << endl;
        cout << "-----------------" << endl;
        cout << "1. Insert Passenger" << endl;
        cout << "2. Delete Passenger" << endl;
        cout << "3. Search Passenger" << endl;
        cout << "4. Display Manifest" << endl;
        cout << "-----------------" << endl;
        cout << "0. Back to Main Menu" << endl;
        cout << "Select an option: ";

        if (!(cin >> choice)) {
            cout << "Invalid input!" << endl;
            flushInput();
            waitForEnter();
            continue;
        }
        flushInput();

        switch(choice) {
            case 1: insertPassenger(); break;
            case 2: deletePassenger(); break;
            case 3: searchPassenger(); break;
            case 4: displayManifest(); break;
            case 0: break;
            default: cout << "Invalid choice." << endl; waitForEnter();
        }
    } while(choice != 0);
}
