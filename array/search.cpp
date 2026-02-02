#include "../header/array.hpp"
#include "../header/utils.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>
#include <cctype>    // for isdigit
#include <algorithm> // for std::sort, std::min

using namespace std;
using namespace std::chrono;

// ==========================================
//           HELPER FUNCTIONS
// ==========================================

// 1. Check if string is numeric
bool isStringNumeric(const string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!isdigit(c)) return false;
    }
    return true;
}

// 2. Binary Search (Modified for Raw Pointers)
// Returns index in the sorted array, or -1 if not found
int binarySearch(Passenger** list, int low, int high, string target, int choice) {
    while (low <= high) {
        int mid = low + (high - low) / 2;

        string midValue = (choice == 1) ? list[mid]->passengerId : list[mid]->name;

        // Choice 2 (Name): Check for Prefix Match (Starts with...)
        if (choice == 2) {
             if (midValue.rfind(target, 0) == 0) return mid;
        } else {
             // Choice 1 (ID): Exact Match
             if (midValue == target) return mid;
        }

        if (midValue < target)
            low = mid + 1;
        else
            high = mid - 1;
    }
    return -1;
}

// 3. Exponential Search (Modified for Raw Pointers & Return Value)
int runExponentialSearch(Passenger** list, int size, string target, int choice) {
    // 1. Check first element
    if (size == 0) return -1;
    string firstVal = (choice == 1) ? list[0]->passengerId : list[0]->name;

    bool match = (choice == 1) ? (firstVal == target) : (firstVal.rfind(target, 0) == 0);
    if (match) return 0;

    // 2. Find Range (1, 2, 4, 8...)
    int i = 1;
    while (i < size) {
        string currVal = (choice == 1) ? list[i]->passengerId : list[i]->name;
        if (currVal > target) break; // Optimization: Stop if we exceeded target
        i = i * 2;
    }

    // 3. Binary Search in found range
    return binarySearch(list, i / 2, min(i, size - 1), target, choice);
}

// ==========================================
//           MAIN SEARCH FUNCTION
// ==========================================

void ArraySystem::searchPassenger() {
    int choice;

    cout << endl << "=== Search Passenger Options ===" << endl;
    cout << "1. Search by Passenger ID" << endl;
    cout << "2. Search by Passenger Name" << endl;

    // --- VALIDATION 1: MENU SELECTION ---
    while (true) {
        cout << endl << "Select option: ";
        if (cin >> choice) {
            if (choice == 1 || choice == 2) {
                flushInput();
                break;
            }
        } else {
            cin.clear();
        }
        flushInput();
        cout << "Invalid input. Please enter 1 or 2 only." << endl;
    }

    string searchTerm;

    // --- VALIDATION 2: SEARCH INPUT ---
    if (choice == 1) {
        while (true) {
            cout << endl << "Enter Passenger ID (6 digits): ";
            getline(cin, searchTerm);
            if (searchTerm.length() == 6 && isStringNumeric(searchTerm)) break;
            cout << "Error: Invalid ID. Must be exactly 6 digits." << endl;
        }
    } else {
        while (true) {
            cout << endl << "Enter Passenger Name (Full Name/Part of Name): ";
            getline(cin, searchTerm);
            if (!searchTerm.empty()) break;
            cout << "Error: Name cannot be empty." << endl;
        }
    }

    // ==========================================
    // PART 1: ALGORITHM BENCHMARKING
    // ==========================================

    cout << endl << "Running Algorithm Speed Test..." << endl;

    double durationStandard = 0.0;
    double durationExpo = 0.0;

    // ------------------------------------------
    // METHOD 1: STANDARD LINEAR SEARCH
    // ------------------------------------------
    auto startStandard = high_resolution_clock::now();

    for(int k=0; k<1000; k++) {
        for (int i = 0; i < passengerCount; i++) {
            bool match = false;
            if (choice == 1) {
                if (passengerList[i]->passengerId == searchTerm) match = true;
            } else {
                if (passengerList[i]->name.find(searchTerm) != string::npos) match = true;
            }

            if (match && choice == 1) break;
        }
    }
    auto endStandard = high_resolution_clock::now();
    duration<double, std::milli> ms_double_std = endStandard - startStandard;
    durationStandard = ms_double_std.count();

    // ------------------------------------------
    // METHOD 2: EXPONENTIAL SEARCH
    // ------------------------------------------
    // PREPARATION: Create Dynamic Array manually (NO VECTOR)
    Passenger** sortedList = nullptr;

    if (passengerCount > 0) {
        // 1. Allocate memory
        sortedList = new Passenger*[passengerCount];

        // 2. Copy data
        for(int i = 0; i < passengerCount; i++) {
            sortedList[i] = passengerList[i];
        }

        // 3. Sort using pointers
        sort(sortedList, sortedList + passengerCount, [choice](Passenger* a, Passenger* b) {
            if (choice == 1) return a->passengerId < b->passengerId;
            return a->name < b->name;
        });
    }

    auto startExpo = high_resolution_clock::now();

    volatile int sink = 0; // Prevent compiler optimization
    // Increased loop count because Exponential is very fast
    for(int k=0; k < 100000; k++) {
        if (passengerCount > 0) {
            sink += runExponentialSearch(sortedList, passengerCount, searchTerm, choice);
        }
    }

    auto endExpo = high_resolution_clock::now();
    duration<double, std::milli> ms_double_expo = endExpo - startExpo;
    durationExpo = ms_double_expo.count() / 100.0; // Normalize time

    // 4. CLEANUP MEMORY (Crucial for DSTR assignment)
    if (passengerCount > 0) {
        delete[] sortedList;
    }

    // --- Benchmark Results ---
    cout << fixed << setprecision(4);

    cout << "------------------------------------------------" << endl;
    cout << "1. Standard Linear Search    : " << durationStandard << " ms" << endl;
    cout << "2. Exponential Search Time   : " << durationExpo << " ms (Sorted Copy)" << endl;
    cout << "------------------------------------------------" << endl;

    // Determine Fastest
    double minTime = durationStandard;
    string fastMethod = "Standard Linear Search";

    if (durationExpo < minTime) {
        minTime = durationExpo;
        fastMethod = "Exponential Search";
    }

    cout << ">> Fastest Method: " << fastMethod << endl;
    cout << "------------------------------------------------" << endl;

    cout.unsetf(ios::fixed);

    // ==========================================
    // PART 2: DISPLAY ALL ORDERS
    // ==========================================
    int foundCount = 0;

    // We scan the main list to display results
    // (We do this check first to handle "Not Found" message)
    bool anyFound = false;
    for (int i = 0; i < passengerCount; i++) {
        if (choice == 1) {
            if (passengerList[i]->passengerId == searchTerm) { anyFound = true; break; }
        } else {
            if (passengerList[i]->name.find(searchTerm) != string::npos) { anyFound = true; break; }
        }
    }

    if (!anyFound) {
         cout << "\nNo record found for: " << searchTerm << endl;
    } else {
        cout << endl;
        cout << "===============================================================" << endl;
        cout << "                 Passenger Reservation Details                 " << endl;
        cout << "===============================================================" << endl;
        cout << left << setw(5)  << "No."
                     << setw(15) << "Passenger ID"
                     << setw(25) << "Passenger Name"
                     << setw(15) << "Flight Class"
                     << setw(15) << "Seat [R,C]" << endl;
        cout << "---------------------------------------------------------------" << endl;

        for (int i = 0; i < passengerCount; i++) {
            bool match = false;
            if (choice == 1) {
                if (passengerList[i]->passengerId == searchTerm) match = true;
            } else {
                if (passengerList[i]->name.find(searchTerm) != string::npos) match = true;
            }

            if (match) {
                foundCount++;
                Passenger* p = passengerList[i];
                string seatCode = "[" + p->seatRow + "," + p->seatColumn + "]";

                cout << left << setw(5)  << foundCount
                     << setw(15) << p->passengerId
                     << setw(25) << p->name.substr(0, 24)
                     << setw(15) << p->flightClass
                     << setw(15) << seatCode << endl;
            }
        }
        cout << "===============================================================" << endl;
        cout << "Total Orders: " << foundCount << endl;
    }

    waitForEnter();
}
