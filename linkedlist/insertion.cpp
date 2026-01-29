#include <iostream>
#include <iomanip>
#include "../header/Passenger.hpp"
#include "../header/utils.hpp"
#include "../header/linkedlist.hpp"
#include <cctype> // for isdigit

using namespace std;

//generate new id, auto increment
string LinkedListSystem::generateNewId() {
    int maxId = 100000; // Starting base for 6-digit IDs
    SNode* current = sHead;

    while (current != nullptr) {
        try {
            int currentId = stoi(current->data.passengerId);
            if (currentId > maxId) {
                maxId = currentId;
            }
        } catch (...) {
            // Skip non-numeric IDs if any exist
        }
        current = current->next;
    }

    return to_string(maxId + 1);
}
//check if string has digits
bool LinkedListSystem::hasDigits(string str) {
    for (char c : str) {
        if (isdigit(c)) return true;
    }
    return false;
}

//add passenger to linked list
void LinkedListSystem::addPassengerToList(Passenger p) {
    SNode* newNode = new SNode(p);

    if (sHead == nullptr) {
        sHead = sTail = newNode;
    } else {
        // Appending to the tail for efficient O(1) insertion
        sTail->next = newNode;
        sTail = newNode;
    }
}

//main function
void LinkedListSystem::insertPassenger() {
    int maxRows, maxCols;
    getPlaneDimensions(maxRows, maxCols);

    // Build occupancy grid
    bool** occupied = new bool*[maxRows];
    for(int i = 0; i < maxRows; i++) {
        occupied[i] = new bool[maxCols];
        for(int j = 0; j < maxCols; j++) occupied[i][j] = false;
    }
    SNode* tempNode = sHead;
    while (tempNode != nullptr) {
        try {
            int r = stoi(tempNode->data.seatRow) - 1;
            int c = toupper(tempNode->data.seatColumn[0]) - 'A';
            if (r >= 0 && r < maxRows && c >= 0 && c < maxCols) occupied[r][c] = true;
        } catch (...) {}
        tempNode = tempNode->next;
    }

    clearScreen();
    cout << "=== Advanced Flight Reservation System ===" << endl;

    // Suggest 15 seats for EACH class

    auto suggestForRange = [&](string label, int start, int end) {
        cout << "\n>>> Suggestions for " << label << " (Rows " << start << "-" << end << "):" << endl;
        int found = 0;
        for (int i = start - 1; i < end && i < maxRows && found < 15; i++) {
            for (int j = 0; j < maxCols && found < 15; j++) {
                if (!occupied[i][j]) {
                    cout << "[" << i + 1 << (char)('A' + j) << "] ";
                    found++;
                    if (found % 5 == 0) cout << endl;
                }
            }
        }
        if (found == 0) cout << "No available seats in this section." << endl;
    };

    suggestForRange("First Class", 1, 70);

    suggestForRange("Business Class", 71, 210);
    suggestForRange("Economy Class", 211, 460);

    cout << "\n------------------------------------------------" << endl;

        // Auto-generate ID and get Name
    string id = generateNewId();
    cout << "[System] New Passenger ID: " << id << endl;

    string name, row, col, fClass;

    while (true) {
        cout << "Enter Passenger Name: ";
        getline(cin, name);
        if (!name.empty() && !hasDigits(name)) break;
        cout << "Error: Name cannot contain digits or be empty." << endl;
    }

    // STEP-BY-STEP SEAT VALIDATION
    int r_idx = -1, c_idx = -1;

    // --- Step A: Validate Class ---
    while (true) {
        cout << "\nSelect Class (First/Business/Economy): ";
        getline(cin, fClass);
        string norm = fClass;
        for(auto &c : norm) c = tolower(c);

        if (norm == "first" || norm == "business" || norm == "economy") {
            fClass = norm; // Save normalized version for logic
            break;
        }
        cout << "Error: Invalid class. Please type First, Business, or Economy." << endl;
    }

    // --- Step B: Validate Row against Class ---
    while (true) {
        cout << "Enter Row for " << fClass << " class: ";
        getline(cin, row);
        try {
            int r = stoi(row);
            bool rangeMatch = false;

            if (fClass == "first" && r >= 1 && r <= 70) rangeMatch = true;
            else if (fClass == "business" && r >= 71 && r <= 210) rangeMatch = true;
            else if (fClass == "economy" && r >= 211 && r <= 460) rangeMatch = true;

            if (rangeMatch && r <= maxRows) {
                r_idx = r - 1;
                break;
            } else {
                cout << "Error: Row " << r << " is out of range for " << fClass << " class." << endl;
            }
        } catch (...) {
            cout << "Error: Please enter a valid number for the row." << endl;
        }
    }

    // --- Step C: Validate Column and Seat Availability ---
    while (true) {
        cout << "Enter Column (A-" << (char)('A' + maxCols - 1) << "): ";
        getline(cin, col);

        // Basic check for empty input
        if (col.empty()) {
            cout << "Error: Column cannot be empty." << endl;
            continue;
        }

        // Convert first character to index
        c_idx = toupper(col[0]) - 'A';

        // 1. Check if the column is within aircraft bounds
        if (c_idx >= 0 && c_idx < maxCols) {

            // 2. Final check: Is this specific seat taken?
            if (occupied[r_idx][c_idx]) {
                // Directly force another entry for the same row
                cout << "Error: Seat " << row << (char)('A' + c_idx)
                    << " is already taken! Please enter a different column." << endl;
            } else {
                // Seat is valid and free!
                break;
            }
        } else {
            cout << "Error: Invalid column letter. Please try again." << endl;
        }
    }

    // Dynamic Insertion
    // Format class back to title case for the Passenger object
    fClass[0] = toupper(fClass[0]);
    Passenger p(id, name, row, col, fClass);
    addPassengerToList(p);

    // Cleanup
    for(int i = 0; i < maxRows; i++) delete[] occupied[i];
    delete[] occupied;

    cout << "\n[Success] Reservation confirmed!" << endl;
    cout << "Name: " << name << " | ID: " << id << " | Seat: " << row << col << " (" << fClass << ")" << endl;
}


