#include "../header/array.hpp"
#include "../header/utils.hpp"
#include <iostream>
#include <cctype>
#include <chrono>
#include <iomanip>
#include <cstdlib>

using namespace std;
using namespace std::chrono;

static bool parseRowCol(const string& rowStr, const string& colStr, int &r, int &c) {
    try {
        r = stoi(rowStr) - 1;
    } catch (...) {
        return false;
    }

    if (colStr.empty()) return false;

    char ch = toupper(colStr[0]);
    if (isalpha((unsigned char)ch)) c = ch - 'A';
    else {
        try { c = stoi(colStr) - 1; }
        catch (...) { return false; }
    }
    return true;
}

void ArraySystem::deleteBenchmark() {
    if (passengerCount == 0) {
        cout << "\n[Benchmark] No passengers loaded.\n";
        waitForEnter();
        return;
    }

    const int BENCHMARK_OPS = 5000;

    // We need backup space INSIDE passengerList:
    // backup uses [passengerCount .. 2*passengerCount-1]
    if (passengerCount * 2 > MAX_PASSENGERS) {
        cout << "\n[Benchmark] Not enough free slots to run in-place benchmark.\n";
        cout << "Need at least 2 * passengerCount <= CAPACITY.\n";
        cout << "Current: passengerCount=" << passengerCount
             << ", CAPACITY=" << MAX_PASSENGERS << "\n";
        waitForEnter();
        return;
    }

    int opsHit = (passengerCount < BENCHMARK_OPS) ? passengerCount : BENCHMARK_OPS;

    cout << string(120, '=') << "\n";
    cout << string(38, ' ') << "BENCHMARK: ARRAY DELETE PERFORMANCE (IN-MEMORY)\n";
    cout << string(120, '-') << "\n";
    cout << "Dataset Size (from CSV loaded in memory): " << passengerCount << "\n";
    cout << "Ops per scenario (requested): " << BENCHMARK_OPS << "\n";
    cout << "Ops used for HIT scenarios (cannot exceed size): " << opsHit << "\n";
    cout << "Note: CSV is NOT modified. No seat map updates. No user input. No file I/O.\n";
    cout << "Note: Uses passengerList free slots as backup (array-only).\n";
    cout << string(120, '-') << "\n";

    cout << left
         << setw(28) << "Scenario"
         << setw(18) << "Total Time (ms)"
         << setw(18) << "Avg Time (ns/op)"
         << setw(22) << "Avg Traversal Steps"
         << setw(16) << "Avg Shifts"
         << setw(10) << "Frees"
         << setw(10) << "Result"
         << "\n";
    cout << string(120, '-') << "\n";

    // Fixed seed for reproducible report
    srand(12345);

    // Backup originals into the unused half of the same array
    auto backupOriginal = [&]() {
        for (int i = 0; i < passengerCount; i++) {
            passengerList[passengerCount + i] = passengerList[i];
        }
    };

    // Restore originals from backup area
    auto restoreOriginal = [&]() {
        for (int i = 0; i < passengerCount; i++) {
            passengerList[i] = passengerList[passengerCount + i];
            passengerList[passengerCount + i] = nullptr; // clean backup slot
        }
    };

    auto runScenario = [&](const string& name,
                        bool delHead,
                        bool delRandom,
                        bool delTail,
                        bool missScenario) {

        backupOriginal();

        int workingCount = passengerCount;
        long long totalSteps = 0;
        long long totalShifts = 0;
        int frees = 0;

        auto start = high_resolution_clock::now();

        if (!missScenario) {
            for (int op = 0; op < opsHit; op++) {
                if (workingCount <= 0) break;

                // 1) Choose a target pointer (this simulates "delete by ID": you have an ID, you find it)
                Passenger* target = nullptr;
                if (delHead) {
                    target = passengerList[0];
                } else if (delTail) {
                    target = passengerList[workingCount - 1];
                } else if (delRandom) {
                    target = passengerList[rand() % workingCount];
                }

                // 2) Linear scan to find target (REAL traversal)
                int idx = -1;
                for (int i = 0; i < workingCount; i++) {
                    totalSteps++;
                    if (passengerList[i] == target) {
                        idx = i;
                        break;
                    }
                }

                if (idx == -1) continue; // should not happen in HIT

                // 3) Shift to close gap (array delete behavior)
                int shifts = (workingCount - 1) - idx;
                totalShifts += shifts;

                for (int i = idx; i < workingCount - 1; i++) {
                    passengerList[i] = passengerList[i + 1];
                }
                passengerList[workingCount - 1] = nullptr;
                workingCount--;

                frees++; // successful delete
            }
        } else {
            // MISS = full scan every time (as before)
            for (int op = 0; op < BENCHMARK_OPS; op++) {
                for (int i = 0; i < workingCount; i++) {
                    totalSteps++;
                    if (passengerList[i] == (Passenger*)0xDEADBEEF) {
                        frees++;
                    }
                }
            }
        }

        auto stop = high_resolution_clock::now();

        // Keep "ms" BUT make it a double (avoids 0ms due to integer truncation)
        double ms = duration<double, milli>(stop - start).count();

        int denom = missScenario ? BENCHMARK_OPS : (opsHit > 0 ? opsHit : 1);

        // Still compute ns/op from ms (as you requested)
        double avgNs = (ms * 1e6) / denom;
        double avgSteps = (double)totalSteps / denom;
        double avgShifts = (double)totalShifts / denom;

        cout << left
            << setw(28) << name
            << setw(18) << fixed << setprecision(3) << ms
            << setw(18) << fixed << setprecision(2) << avgNs
            << setw(22) << fixed << setprecision(2) << avgSteps
            << setw(16) << fixed << setprecision(2) << avgShifts
            << setw(10) << frees
            << setw(10) << "Done"
            << "\n";

        restoreOriginal();
    };

    runScenario("Delete HIT (Head)",   true,  false, false, false);
    runScenario("Delete HIT (Random)", false, true,  false, false);
    runScenario("Delete HIT (Tail)",   false, false, true,  false);
    runScenario("Delete MISS",         false, false, false, true);

    cout << string(120, '=') << "\n";
    waitForEnter();
}
void ArraySystem::deletePassenger() {
    if (passengerCount == 0) {
        cout << "\n[Delete] No passengers to delete.\n";
        waitForEnter();
        return;
    }

    clearScreen();
    string targetID;
    cout << "\nEnter Passenger ID to delete: ";
    getline(cin >> ws, targetID);

    int idx = -1;
    for (int i = 0; i < passengerCount; i++) {
        if (passengerList[i] && passengerList[i]->passengerId == targetID) {
            idx = i;
            break;
        }
    }

    if (idx == -1) {
        cout << "\n[Delete] Passenger ID not found.\n";
        waitForEnter();
        return;
    }

    Passenger* p = passengerList[idx];

    // Clear seat (prevent double delete later)
    int r, c;
    if (parseRowCol(p->seatRow, p->seatColumn, r, c)) {
        if (r >= 0 && r < MAX_ROWS && c >= 0 && c < MAX_COLUMNS) {
            seats[r][c] = nullptr;
        }
    } else {
        // If seat row/col invalid, still delete passenger safely
        cout << "\n[Delete] Warning: seatRow/seatColumn invalid. Still deleting passenger.\n";
    }

    // Remove from list first (extra safety)
    for (int i = idx; i < passengerCount - 1; i++) {
        passengerList[i] = passengerList[i + 1];
    }
    passengerList[passengerCount - 1] = nullptr;
    passengerCount--;

    // Now delete passenger object
    delete p;

    cout << "\n[Delete] Passenger deleted. Seat is now available.\n";
    
    if(saveToFile("flight_passenger_data.csv")) {
        cout << "[Delete] Passenger record removed from file successfully.\n";
    } else {
        cout << "[Delete] Warning: Failed to update file after deletion.\n";
    }
    waitForEnter();
}

void ArraySystem::deletePassengerMenu(){
        int choice;
    do {
        clearScreen();
        
        cout << "==========================" << endl;
        cout << "Array Deletion" << endl;
        cout << "==========================" << endl;
        cout << "1. Perform Deletion" << endl;
        cout << "2. Benchmark" << endl;
        cout << "--------------------------" << endl;
        cout << "0. Back to Main Menu" << endl;
        cout << "==========================" << endl;
        cout << "Select an option: ";

        if (!(cin >> choice)) {
            cout << "Invalid input!" << endl;
            choice = -1;
            flushInput();
            waitForEnter();
            continue;
        }
        flushInput();

        switch(choice) {
            case 1: deletePassenger(); break;
            case 2: deleteBenchmark(); break;
            case 0: break;
            default: cout << "Invalid choice." << endl; waitForEnter();
        }
    } while(choice != 0);
}