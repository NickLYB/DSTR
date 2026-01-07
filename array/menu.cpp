#include "../header/array.hpp"
#include "../header/utils.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

ArraySystem::ArraySystem() {
    for (int i = 0; i < MAX_ROWS; ++i) {
        for (int j = 0; j < MAX_COLUMNS; ++j) {
            seats[i][j] = nullptr;
        }
    }
}

ArraySystem::~ArraySystem() {
    for (int i = 0; i < MAX_ROWS; ++i) {
        for (int j = 0; j < MAX_COLUMNS; ++j) {
            if (seats[i][j] != nullptr) {
                delete seats[i][j];
                seats[i][j] = nullptr;
            }
        }
    }
    //log
    //cout << "[Memory] Array seats cleared and memory freed." << endl;
}

void ArraySystem::loadFromFile(string filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }
    string line;
    getline(file, line); // Skip header line

    int successCount = 0;

    while (getline(file, line)) {
        stringstream ss(line);
        string id, name, rowStr, colStr, fClass;

        getline(ss, id, ',');
        getline(ss, name, ',');
        getline(ss, rowStr, ',');
        getline(ss, colStr, ',');
        getline(ss, fClass, ',');

        int row = stoi(rowStr) - 1; // Assuming rows in file are 1-indexed
        int col = colStr[0] - 'A'; // Assuming columns are labeled A-F

        if (row >= 0 && row < MAX_ROWS && col >= 0 && col < MAX_COLUMNS) {
            Passenger* newPassenger = new Passenger(id, name, rowStr, colStr, fClass);
            seats[row][col] = newPassenger;
        }
    }
    file.close();
    cout << "Loading passengers from file: " << filename << endl;
    cout << "Passengers loaded into Array successfully." << endl;
    waitForEnter();
}

void ArraySystem::run() {
    int choice;
    do {
        clearScreen();
        cout << "Array Menu" << endl;
        cout << "----------" << endl;
        cout << "1. Insert Passenger" << endl;
        cout << "2. Delete Passenger" << endl;
        cout << "3. Search Passenger" << endl;
        cout << "4. Display Manifest" << endl;
        cout << "----------" << endl;
        cout << "0. Return to Main Menu" << endl;
        cout << "Select an option: ";

        if (!(cin >> choice)) {
            cout << "Invalid input! Please enter a number." << endl;
            flushInput();
            waitForEnter(); 
            continue;
        }
        flushInput();

        switch(choice) {
            case 1:
                cout << "Insert Passenger selected." << endl;
                waitForEnter();
                break;
            case 2:
                cout << "Delete Passenger selected." << endl;
                waitForEnter();
                break;
            case 3:
                cout << "Search Passenger selected." << endl;
                waitForEnter();
                break;
            case 4:
                cout << "Display Manifest selected." << endl;
                waitForEnter();
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
    } while (choice != 0);
}