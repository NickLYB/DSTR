#include "../header/array.hpp"
#include "../header/utils.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>
#include <cctype>    // for isdigit
#include <algorithm> // for min, sort
#include <vector>    // Required for the temporary sorted list

using namespace std;
using namespace std::chrono;

// Helper to check if string is numeric
bool isStringNumeric(const string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!isdigit(c)) return false;
    }
    return true;
}

// --- HELPER: Binary Search for Method 3 ---
// Returns index in the SORTED list, or -1 if not found
int binarySearch(const vector<Passenger*>& list, int low, int high, string target, int choice) {
    while (low <= high) {
        int mid = low + (high - low) / 2;

        string midValue = (choice == 1) ? list[mid]->passengerId : list[mid]->name;

        // Name Search: Check for Prefix Match (Starts with...)
        if (choice == 2) {
             if (midValue.rfind(target, 0) == 0) return mid;
        } else {
             // ID Search: Exact Match
             if (midValue == target) return mid;
        }

        if (midValue < target)
            low = mid + 1;
        else
            high = mid - 1;
    }
    return -1;
}

// --- HELPER: Exponential Search for Method 3 ---
void runExponentialSearch(const vector<Passenger*>& list, int size, string target, int choice) {
    // 1. Check first element
    string firstVal = (choice == 1) ? list[0]->passengerId : list[0]->name;

    bool match = (choice == 1) ? (firstVal == target) : (firstVal.rfind(target, 0) == 0);
    if (match) return;

    // 2. Find Range (1, 2, 4, 8...)
    int i = 1;
    while (i < size) {
        string currVal = (choice == 1) ? list[i]->passengerId : list[i]->name;
        if (currVal > target) break; // Optimization: Stop if we exceeded target
        i = i * 2;
    }

    // 3. Binary Search in found range
    binarySearch(list, i / 2, min(i, size - 1), target, choice);
}

void ArraySystem::searchPassenger() {
    int choice;

    clearScreen();
    cout << "================================" << endl;
    cout << "    Search Passenger Options    " << endl;
    cout << "================================" << endl;
    cout << "1. Search by Passenger ID" << endl;
    cout << "2. Search by Passenger Name" << endl;
    cout  << "--------------------------------" << endl;
    cout << "0. Back to Previous Menu" << endl;
    cout << "================================";

    // --- VALIDATION 1: MENU SELECTION ---
    while (true) {
        cout << endl << "Select option: ";
        if (cin >> choice) {
            if (choice == 1 || choice == 2) {
                flushInput();
                break;
            }else if(choice == 0){
            flushInput();
            return; // Back to previous menu
        } 
        }
        else {
            cin.clear();
        }
        flushInput();
        cout << "Invalid input." << endl;
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
            cout << endl << "Enter Passenger Name (Start of name): ";
            getline(cin, searchTerm);
            if (!searchTerm.empty()) break;
            cout << "Error: Name cannot be empty." << endl;
        }
    }

    // ==========================================
    // PART 1: ALGORITHM BENCHMARKING
    // ==========================================

    cout << endl << "Running Algorithm Speed Test..." << endl;

    double durationSentinel = 0.0;
    double durationStandard = 0.0;
    double durationExpo = 0.0;    // Method 3
    double durationMTF = 0.0;     // Method 4

    bool sentinelPossible = (passengerCount < (MAX_ROWS * MAX_COLUMNS));

    // ------------------------------------------
    // METHOD 1: SENTINEL LINEAR SEARCH
    // ------------------------------------------
    // FIX: Allowed for Name search as well (acts as Linear Scan for Name)
    if (sentinelPossible) {

        // Setup Sentinel only for ID (Exact match logic)
        // For Name, we don't rely on the sentinel stopping power for "Find All"
        if (choice == 1) {
            Passenger sentinel;
            sentinel.passengerId = searchTerm;
            passengerList[passengerCount] = &sentinel;
        }

        auto startSentinel = high_resolution_clock::now();

        for(int k=0; k<10000; k++) {
            if (choice == 1) {
                // [ID]: True Sentinel Logic (Stops at First)
                int i = 0;
                while (passengerList[i]->passengerId != searchTerm) i++;
            } else {
                // [NAME]: Full Linear Scan (Finds ALL)
                // This "makes it work" instead of N/A, effectively same as Standard Search
                for(int i = 0; i < passengerCount; i++) {
                     if (passengerList[i]->name.find(searchTerm) != string::npos) {
                         continue; // Continue finding more
                     }
                }
            }
        }

        auto endSentinel = high_resolution_clock::now();

        // Cleanup
        if (choice == 1) passengerList[passengerCount] = nullptr;

        duration<double, std::milli> ms_double = endSentinel - startSentinel;
        durationSentinel = ms_double.count();
    }

    // ------------------------------------------
    // METHOD 2: STANDARD LINEAR SEARCH
    // ------------------------------------------
    auto startStandard = high_resolution_clock::now();

    for(int k=0; k<10000; k++) {
        for (int i = 0; i < passengerCount; i++) {
            bool match = false;
            if (choice == 1) {
                if (passengerList[i]->passengerId == searchTerm) match = true;
            } else {
                if (passengerList[i]->name.find(searchTerm) != string::npos) match = true;
            }

            // Stop for ID (Unique), Continue for Name (Find All)
            if (match && choice == 1) break;
        }
    }
    auto endStandard = high_resolution_clock::now();
    duration<double, std::milli> ms_double_std = endStandard - startStandard;
    durationStandard = ms_double_std.count();

    // ------------------------------------------
    // METHOD 3: EXPONENTIAL SEARCH
    // ------------------------------------------
    // PREPARATION: Sort a copy of the list (Time excluded from benchmark)
    vector<Passenger*> sortedList;
    if (passengerCount > 0) {
        sortedList.assign(passengerList, passengerList + passengerCount);
        sort(sortedList.begin(), sortedList.end(), [choice](Passenger* a, Passenger* b) {
            if (choice == 1) return a->passengerId < b->passengerId;
            return a->name < b->name;
        });
    }

    auto startExpo = high_resolution_clock::now();

    for(int k=0; k<10000; k++) {
        if (!sortedList.empty()) {
            runExponentialSearch(sortedList, passengerCount, searchTerm, choice);
        }
    }

    auto endExpo = high_resolution_clock::now();
    duration<double, std::milli> ms_double_expo = endExpo - startExpo;
    durationExpo = ms_double_expo.count();

    // ------------------------------------------
    // METHOD 4: MOVE-TO-FRONT SEARCH
    // ------------------------------------------
    bool foundFirst = false;
    int firstIndex = -1;

    auto startMTF = high_resolution_clock::now();

    for(int k=0; k<10000; k++) {
        for (int i = 0; i < passengerCount; i++) {
            bool match = false;
            if (choice == 1) {
                if (passengerList[i]->passengerId == searchTerm) match = true;
            } else {
                if (passengerList[i]->name.find(searchTerm) != string::npos) match = true;
            }

            if (match) {
                if(k == 0 && !foundFirst) {
                    foundFirst = true;
                    firstIndex = i;
                }
                if (choice == 1) break; // Break for ID
            }
        }
    }
    auto endMTF = high_resolution_clock::now();
    duration<double, std::milli> ms_double_mtf = endMTF - startMTF;
    durationMTF = ms_double_mtf.count();

    // Apply Swap
    if (foundFirst && firstIndex > 0) {
        Passenger* temp = passengerList[0];
        passengerList[0] = passengerList[firstIndex];
        passengerList[firstIndex] = temp;
    }

    // --- Benchmark Results ---
    cout << fixed << setprecision(4);

    cout << "------------------------------------------------" << endl;
    if (sentinelPossible)
        cout << "1. Sentinel Search Time      : " << durationSentinel << " ms" << endl;
    else
        cout << "1. Sentinel Search           : (Array Full)" << endl;

    cout << "2. Standard Linear Search    : " << durationStandard << " ms" << endl;
    cout << "3. Exponential Search Time   : " << durationExpo << " ms (Sorted Copy)" << endl;
    cout << "4. Move-to-Front Search Time : " << durationMTF << " ms" << endl;
    cout << "------------------------------------------------" << endl;

    // Determine Fastest
    double minTime = 999999.0;
    string fastMethod = "";

    if (sentinelPossible) { minTime = durationSentinel; fastMethod = "Sentinel Search"; }

    if (durationStandard < minTime) { minTime = durationStandard; fastMethod = "Standard Linear Search"; }

    if (durationExpo < minTime) { minTime = durationExpo; fastMethod = "Exponential Search"; }

    if (durationMTF < minTime) { minTime = durationMTF; fastMethod = "Move-to-Front Search"; }

    cout << ">> Fastest Method: " << fastMethod << endl;
    cout << "------------------------------------------------" << endl;

    cout.unsetf(ios::fixed);

    // ==========================================
    // PART 2: DISPLAY ALL ORDERS
    // ==========================================
    int foundCount = 0;

    // Use a simple check to see if we found anything (using MTF result for safety)
    if (!foundFirst && durationExpo == 0.0) {
        // This handles the "No Record" case primarily
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

        if (foundCount == 0) cout << "\nNo record found for: " << searchTerm << endl;
    }

    waitForEnter();
}
