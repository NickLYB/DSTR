#include "../header/array.hpp"
#include "../header/utils.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>
#include <cctype>
#include <algorithm>

using namespace std;
using namespace std::chrono;

// helper function
bool isStringNumeric(const string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!isdigit(c)) return false;
    }
    return true;
}

int runLinearSearch(Passenger** list, int size, string target, int choice) {
    int count = 0;
    for (int i = 0; i < size; i++) {
        if (choice == 1) {
            // search by passenger ID
            if (list[i]->passengerId == target) {
                count++;
            }
        } else {
            // search by passenger name
            if (list[i]->name.find(target) != string::npos) {
                count++;
            }
        }
    }
    return count;
}

// binary search
int binarySearch(Passenger** list, int low, int high, string target) {
    while (low <= high) {
        int mid = low + (high - low) / 2;

        if (list[mid]->passengerId == target) return mid;

        if (list[mid]->passengerId < target)
            low = mid + 1;
        else
            high = mid - 1;
    }
    return -1;
}

// exponential search
int runExponentialSearch(Passenger** list, int size, string target) {
    if (size == 0) return 0;

    // 1.check first element
    if (list[0]->passengerId == target) return 1;
    if (size == 1) return 0;

    // 2.find Range
    int i = 1;
    while (i < size && list[i]->passengerId <= target) {
        i = i * 2;
    }

    // 3.binary Search in the found range
    int index = binarySearch(list, i / 2, min(i, size - 1), target);

    // 4.return result
    return (index != -1) ? 1 : 0;
}

void ArraySystem::searchPassenger() {
    int choice;

    do {
        clearScreen();
        cout << "=============================" << endl;
        cout << "     Search for Passenger    " << endl;
        cout << "=============================" << endl;
        cout << "1. Passenger ID" << endl;
        cout << "2. Passenger Name" << endl;
        cout << "-----------------------------" << endl;
        cout << "0. Back to Previous Menu." << endl;
        cout << "=============================" << endl;
        cout << "Select an option: ";

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

            if (choice == 1) {
                // search by passenger ID
                while (true) {
                    cout << endl << "Enter Passenger ID (6 digits): ";
                    getline(cin, searchTerm);
                    if (searchTerm.length() == 6 && isStringNumeric(searchTerm)) break;
                    cout << "Error: Invalid ID. Must be exactly 6 digits." << endl;
                }
            } else {
                // search by passenger Name
                while (true) {
                    cout << endl << "Enter Passenger Name (Full Name/Part of Name): ";
                    getline(cin, searchTerm);
                    if (!searchTerm.empty()) break;
                    cout << "Error: Name cannot be empty." << endl;
                }
            }

            int foundCount = 0;
            bool anyFound = false;

            if (runLinearSearch(passengerList, passengerCount, searchTerm, choice) > 0) {
                anyFound = true;
            }

            if (!anyFound) {
                 cout << "\n[Result] No record found for: " << searchTerm << endl;
            } else {
                cout << endl;
                cout << "=======================================================================" << endl;
                cout << "                      Passenger Reservation Details                    " << endl;
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
                        // search by passenger ID
                        if (passengerList[i]->passengerId == searchTerm) match = true;
                    } else {
                        // search by passenger name
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

            // option to benchmark
            char benchChoice;
            while (true) {
                cout << endl << "Search Complete. Wanting to benchmark search times? (Y/N): ";
                cin >> benchChoice;
                benchChoice = toupper(benchChoice);
                flushInput();

                if (benchChoice == 'Y' || benchChoice == 'N') break;
                cout << "Invalid input. Please enter 'Y' or 'N'." << endl;
            }

            if (benchChoice == 'Y') {
                const int ITERATIONS = 10000;
                using DoubleMs = duration<double, std::milli>;
                volatile int sink = 0; // prevents compiler optimization

                // memory estimation
                const size_t memLinear = sizeof(int) + sizeof(bool);
                const size_t memExpo = (sizeof(int) * 5) + sizeof(string);
                clearScreen();

                cout << (choice == 1 ? "Target ID: " : "Target Name: ") << searchTerm << endl;
                cout << "=================================================================================================================" << endl;
                cout << "                                BENCHMARK: SEARCH PERFORMANCE (" << ITERATIONS << " ITERATIONS)                                  " << endl;
                cout << "=================================================================================================================" << endl;
                cout << left << setw(30) << "Algorithm"
                     << setw(20) << "Matches Found"
                     << setw(20) << "Total Time (ms)"
                     << setw(20) << "Avg Time (ms)"
                     << setw(20) << "Est. Memory (Stack)" << endl;
                cout << "-----------------------------------------------------------------------------------------------------------------" << endl;

                // helper lambda to print a benchmark row
                auto printRow = [&](const string& algo, int matches, double totalMs, size_t memBytes) {
                    double avgMs = totalMs / ITERATIONS;
                    cout << left << setw(30) << algo
                         << setw(20) << matches
                         << setw(20) << fixed << setprecision(4) << totalMs
                         << setw(20) << fixed << setprecision(6) << avgMs
                         << setw(20) << memBytes << "\n";
                };

                // benchmark 1: linear search
                int matchCountLin = 0;
                auto start = high_resolution_clock::now();

                for(int k=0; k<ITERATIONS; k++) {
                    // call linear search
                    matchCountLin = runLinearSearch(passengerList, passengerCount, searchTerm, choice);
                    sink += matchCountLin; // prevent code removal optimization
                }

                auto end = high_resolution_clock::now();
                DoubleMs dur = end - start;
                printRow("Linear Search", matchCountLin, dur.count(), memLinear);

                // benchmark 2: exponential search
                if (choice == 1) {
                    Passenger** sortedList = new Passenger*[passengerCount];
                    for(int i=0; i<passengerCount; i++) sortedList[i] = passengerList[i];

                    // sort by passenger ID
                    sort(sortedList, sortedList + passengerCount, [choice](Passenger* a, Passenger* b) {
                        return a->passengerId < b->passengerId;
                    });

                    // run benchmark loop
                    int matchCountExpo = 0;
                    start = high_resolution_clock::now();

                    for(int k=0; k<ITERATIONS; k++) {
                         // call exponential search
                         matchCountExpo = runExponentialSearch(sortedList, passengerCount, searchTerm);
                         sink += matchCountExpo;
                    }
                    end = high_resolution_clock::now();
                    dur = end - start;
                    printRow("Exponential Search", matchCountExpo, dur.count(), memExpo);

                    delete[] sortedList; // clean up sorted list memory
                }
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
