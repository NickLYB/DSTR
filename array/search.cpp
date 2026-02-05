#include "../header/array.hpp"
#include "../header/utils.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>
#include <cctype>    // for isdigit, toupper
#include <algorithm> // for std::sort, std::min

using namespace std;
using namespace std::chrono;

// ==========================================
//           HELPER FUNCTIONS
// ==========================================

bool isStringNumeric(const string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!isdigit(c)) return false;
    }
    return true;
}

//Binary Search
int binarySearch(Passenger** list, int low, int high, string target) {
    while (low <= high) {
        int mid = low + (high - low) / 2;

        // Direct access to passengerId
        if (list[mid]->passengerId == target) return mid;

        if (list[mid]->passengerId < target)
            low = mid + 1;
        else
            high = mid - 1;
    }
    return -1;
}

//Exponential Search
int runExponentialSearch(Passenger** list, int size, string target) {
    if (size == 0) return 0;

    // 1.check first element
    if (list[0]->passengerId == target) return 1;
    if (size == 1) return 0;

    // 2.find Range (1, 2, 4, 8...)
    int i = 1;
    while (i < size && list[i]->passengerId <= target) {
        i = i * 2;
    }

    // 3.binary Search
    //search in the range [i/2, min(i, size-1)]
    int index = binarySearch(list, i / 2, min(i, size - 1), target);

    // 4.return result
    return (index != -1) ? 1 : 0;
}

// ==========================================
//           MAIN SEARCH FUNCTION
// ==========================================

void ArraySystem::searchPassenger() {
    int choice;

    // --- MAIN MENU LOOP ---
    do {
        clearScreen();
        cout << "=============================" << endl;
        cout << "     Search for Passenger   " << endl;
        cout << "=============================" << endl;
        cout << "1. Passenger ID" << endl;
        cout << "2. Passenger Name" << endl;
        cout << "-----------------------------" << endl;
        cout << "0. Back to Previous Menu." << endl;
        cout << "=============================" << endl;
        cout << "Select an option: ";

        // --- INPUT VALIDATION ---
        if (!(cin >> choice)) {
            cout << "Invalid input! Please enter a number." << endl;
            choice = -1;
            flushInput();
            waitForEnter();
            continue;
        }
        flushInput();

        if (choice == 0) {
            cout << "Returning to Previous Menu." << endl;
            break;
        }

        if (choice == 1 || choice == 2) {
            string searchTerm;

            // --- 1. GET INPUT ---
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

            // --- 2. DISPLAY RESULTS FIRST ---
            int foundCount = 0;
            bool anyFound = false;

            // Pre-scan logic
            for (int i = 0; i < passengerCount; i++) {
                if (choice == 1) {
                    if (passengerList[i]->passengerId == searchTerm) { anyFound = true; break; }
                } else {
                    if (passengerList[i]->name.find(searchTerm) != string::npos) { anyFound = true; break; }
                }
            }

            if (!anyFound) {
                 cout << "\n[Result] No record found for: " << searchTerm << endl;
            } else {
                cout << endl;
                cout << "=======================================================================" << endl;
                cout << "                     Passenger Reservation Details                 " << endl;
                cout << "=======================================================================" << endl;
                cout << left << setw(5)  << "No."
                             << setw(15) << "Passenger ID"
                             << setw(25) << "Passenger Name"
                             << setw(15) << "Flight Class"
                             << setw(15) << "Seat [R,C]" << endl;
                cout << "-----------------------------------------------------------------------" << endl;

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
                cout << "=======================================================================" << endl;
                cout << "Total Orders: " << foundCount << endl;
            }

            // --- 3. ASK FOR BENCHMARK ---
            char benchChoice;
            while (true) {
                cout << endl << "Search Complete. Wanting to benchmark search times? (Y/N): ";
                cin >> benchChoice;
                benchChoice = toupper(benchChoice);
                flushInput();

                if (benchChoice == 'Y' || benchChoice == 'N') break;
                cout << "Invalid input. Please enter 'Y' or 'N'." << endl;
            }

            // --- 4. EXECUTE BENCHMARK IF 'Y' ---
            if (benchChoice == 'Y') {
                const int ITERATIONS = 10000;
                using DoubleMs = duration<double, std::milli>;
                volatile int sink = 0;

                // Memory Estimations
                const size_t memLinear = sizeof(int) + sizeof(bool);
                const size_t memExpo = (sizeof(int) * 5) + sizeof(string);

                clearScreen();

                // --- PRINT HEADERS ---
                cout << (choice == 1 ? "Target ID: " : "Target Name: ") << searchTerm << endl;
                cout << "=================================================================================================================" << endl;
                cout << "                                BENCHMARK: SEARCH PERFORMANCE (" << ITERATIONS << " ITERATIONS)                          " << endl;
                cout << "=================================================================================================================" << endl;
                cout << left << setw(30) << "Algorithm"
                     << setw(20) << "Matches Found"
                     << setw(20) << "Total Time (ms)"
                     << setw(20) << "Avg Time (ms)"
                     << setw(20) << "Est. Memory (Stack)" << endl;
                cout << "-----------------------------------------------------------------------------------------------------------------" << endl;

                auto printRow = [&](const string& algo, int matches, double totalMs, size_t memBytes) {
                    double avgMs = totalMs / ITERATIONS;
                    cout << left << setw(30) << algo
                         << setw(20) << matches
                         << setw(20) << fixed << setprecision(4) << totalMs
                         << setw(20) << fixed << setprecision(6) << avgMs
                         << setw(20) << memBytes << "\n";
                };

                // ----------------------------------------------------
                // BENCHMARK 1: STANDARD LINEAR SEARCH (RUNS FOR BOTH)
                // ----------------------------------------------------
                int matchCountLin = 0;
                auto start = high_resolution_clock::now();
                for(int k=0; k<ITERATIONS; k++) {
                    matchCountLin = 0;
                    for (int i = 0; i < passengerCount; i++) {
                        if (choice == 1) {
                            if (passengerList[i]->passengerId == searchTerm) matchCountLin++;
                        } else {
                            if (passengerList[i]->name.find(searchTerm) != string::npos) matchCountLin++;
                        }
                    }
                    sink += matchCountLin;
                }
                auto end = high_resolution_clock::now();
                DoubleMs dur = end - start;
                printRow("Linear Search", matchCountLin, dur.count(), memLinear);

                // ----------------------------------------------------
                // BENCHMARK 2: EXPONENTIAL SEARCH (ONLY FOR ID SEARCH)
                // ----------------------------------------------------
                if (choice == 1) {
                    Passenger** sortedList = new Passenger*[passengerCount];
                    for(int i=0; i<passengerCount; i++) sortedList[i] = passengerList[i];

                    sort(sortedList, sortedList + passengerCount, [choice](Passenger* a, Passenger* b) {
                        return a->passengerId < b->passengerId;
                    });

                    int matchCountExpo = 0;
                    start = high_resolution_clock::now();
                    for(int k=0; k<ITERATIONS; k++) {
                         matchCountExpo = runExponentialSearch(sortedList, passengerCount, searchTerm);
                         sink += matchCountExpo;
                    }
                    end = high_resolution_clock::now();
                    dur = end - start;
                    printRow("Exponential Search", matchCountExpo, dur.count(), memExpo);

                    delete[] sortedList;
                }

                // --- FOOTERS ---
                cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
                cout << "N (nodes): " << passengerCount << endl;
                cout << "Note: Est. Stack (bytes) counts only local stack objects (sizeof)." << endl;
                cout << "=================================================================================================================" << endl;
                (void)sink;
            }

            waitForEnter();

        } else {
            cout << "Invalid choice. Please select again." << endl;
            waitForEnter();
        }

    } while (choice != 0);
}
