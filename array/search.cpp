#include "../header/array.hpp"
#include "../header/utils.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>
#include <cctype> // for isdigit

using namespace std;
using namespace std::chrono;

// Helper to check if string is numeric (copy this above searchPassenger if not in utils)
bool isStringNumeric(const string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!isdigit(c)) return false;
    }
    return true;
}

void ArraySystem::searchPassenger() {
    int choice;

    cout << endl << "=== Search Passenger Options ===" << endl;
    cout << "1. Search by Passenger ID" << endl;
    cout << "2. Search by Passenger Name" << endl;

    // --- VALIDATION 1: MENU SELECTION ---
    // Loop until user enters 1 or 2
    while (true) {
        cout << endl << "Select option: ";
        if (cin >> choice) {
            if (choice == 1 || choice == 2) {
                flushInput(); // Clear the newline from buffer
                break; // Valid input, exit loop
            }
        } else {
            // Clear error flags if non-integer entered
            cin.clear();
        }
        flushInput(); // Discard invalid characters
        cout << "Invalid input. Please enter 1 or 2 only." << endl;
    }

    string searchTerm;

    // --- VALIDATION 2: SEARCH INPUT ---
    if (choice == 1) {
        // ID Validation: Loop until exactly 6 digits
        while (true) {
            cout << endl << "Enter Passenger ID (6 digits): ";
            getline(cin, searchTerm);

            if (searchTerm.length() == 6 && isStringNumeric(searchTerm)) {
                break; // Valid
            }
            cout << "Error: Invalid ID. Must be exactly 6 digits." << endl;
        }
    } else {
        // Name Validation: Standard non-empty check
        while (true) {
            cout << endl << "Enter Passenger Name: ";
            getline(cin, searchTerm);
            if (!searchTerm.empty()) break;
            cout << "Error: Name cannot be empty." << endl;
        }
    }

    // ==========================================
    // PART 1: ALGORITHM BENCHMARKING (Speed Test)
    // ==========================================

    cout << endl << "Running Algorithm Speed Test (Finding first match)..." << endl;

    double durationSentinel = 0.0;
    bool sentinelPossible = (passengerCount < (MAX_ROWS * MAX_COLUMNS)); // Check against max capacity

    // --- Method 1: Sentinel Linear Search ---
    if (sentinelPossible) {
        Passenger sentinel;
        // Setup sentinel data based on search type
        if (choice == 1) sentinel.passengerId = searchTerm;
        else sentinel.name = searchTerm;

        passengerList[passengerCount] = &sentinel; // Place Sentinel

        auto startSentinel = high_resolution_clock::now();

        // [REQUIREMENT CHECK]: Looping 1000 times to calculate search time
        for(int k=0; k<1000; k++) {
            int i = 0;
            if (choice == 1) {
                while (passengerList[i]->passengerId != searchTerm) i++;
            } else {
                while (passengerList[i]->name != searchTerm) i++;
            }
        }

        auto endSentinel = high_resolution_clock::now();

        // CLEANUP SENTINEL
        passengerList[passengerCount] = nullptr;

        // Calculate duration in Milliseconds
        duration<double, std::milli> ms_double = endSentinel - startSentinel;
        durationSentinel = ms_double.count();
    }

    // --- Method 2: Move-to-Front Search ---
    // Note: We benchmark the "Linear Search" aspect first.
    // The actual "Move" happens after benchmarking to avoid skewing the test
    // (otherwise iterations 2-1000 would find it instantly at index 0).

    bool foundFirst = false;
    int firstIndex = -1;
    double durationMTF = 0.0;

    auto startMTF = high_resolution_clock::now();

    // [REQUIREMENT CHECK]: Looping 1000 times
    for(int k=0; k<1000; k++) {
        for (int i = 0; i < passengerCount; i++) {
            bool match = false;
            if (choice == 1) { if (passengerList[i]->passengerId == searchTerm) match = true; }
            else { if (passengerList[i]->name == searchTerm) match = true; }

            if (match) {
                if(k==0) { // Capture location only on first pass
                    foundFirst = true;
                    firstIndex = i;
                }
                break; // Stop loop immediately when found
            }
        }
    }
    auto endMTF = high_resolution_clock::now();

    duration<double, std::milli> ms_double_mtf = endMTF - startMTF;
    durationMTF = ms_double_mtf.count();

    // OPTIMIZATION: Apply Move-to-Front Logic (Swap Data)
    if (foundFirst && firstIndex > 0) {
        Passenger* temp = passengerList[0];
        passengerList[0] = passengerList[firstIndex];
        passengerList[firstIndex] = temp;
    }

    // --- Benchmark Results ---
    cout << fixed << setprecision(4); // Show 4 decimal places (e.g. 0.0152 ms)

    cout << "------------------------------------------------" << endl;
    if(sentinelPossible) cout << "Sentinel Search Time      : " << durationSentinel << " ms" << endl;
    cout << "Move-to-Front Search Time : " << durationMTF << " ms" << endl;

    if (sentinelPossible && durationSentinel < durationMTF)
        cout << ">> Sentinel Search was Faster." << endl;
    else
        cout << ">> Move-to-Front Search was Faster." << endl;
    cout << "------------------------------------------------" << endl;

    cout.unsetf(ios::fixed); // Reset format

    // ==========================================
    // PART 2: DISPLAY ALL ORDERS (Table Format)
    // ==========================================

    int foundCount = 0;

    if (!foundFirst) {
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

        // Since we might have moved the record to index 0, we scan the whole list again
        for (int i = 0; i < passengerCount; i++) {
            bool match = false;
            if (choice == 1) {
                if (passengerList[i]->passengerId == searchTerm) match = true;
            } else {
                if (passengerList[i]->name == searchTerm) match = true;
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
