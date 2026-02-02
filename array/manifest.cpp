#include "../header/array.hpp"
#include "../header/utils.hpp"
#include <iostream>
#include <string>
#include <iomanip>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <cctype>

using namespace std;
using namespace std::chrono;

// ===============================
// Seating Chart
// ===============================
int ArraySystem::getMaxRow(){
    int maxRow = 0;
    for(int i = 0 ; i<passengerCount; i++){
        if (stoi(passengerList[i] -> seatRow) > maxRow){
            maxRow = stoi(passengerList[i] -> seatRow);
        }
    }
    return maxRow;
}

void ArraySystem::displaySeatingChart() {
    clearScreen();
    cout << "==================== FULL SEATING CHART ====================\n";
    cout << "Legend: [X] Occupied   [.] Available" << endl;

    int row = getMaxRow();
    for(int i = 0; i < row; i++) {
        int rowNum = i + 1;

        if(rowNum == 1) {
            cout << "\n[ SECTION: FIRST CLASS (Rows 1-70) ]" << endl;
            cout << "      ";
            for (int j = 0; j < MAX_COLUMNS; j++) cout << " " << char('A' + j) << "  ";
            cout << endl;
            cout << string(MAX_COLUMNS * 4 + 6, '-') << endl;
        }
        else if(rowNum == 71) {
            cout << "\n\n[ SECTION: BUSINESS CLASS (Rows 71-210) ]" << endl;
            cout << "      ";
            for (int j = 0; j < MAX_COLUMNS; j++) cout << " " << char('A' + j) << "  ";
            cout << endl;
            cout << string(MAX_COLUMNS * 4 + 6, '-') << endl;
        }
        else if(rowNum == 211) {
            cout << "\n\n[ SECTION: ECONOMY CLASS (Rows 211-460) ]" << endl;
            cout << "      ";
            for (int j = 0; j < MAX_COLUMNS; j++) cout << " " << char('A' + j) << "  ";
            cout << endl;
            cout << string(MAX_COLUMNS * 4 + 6, '-') << endl;
        }
        cout << setw(3) << i + 1 << " | ";
        for (int j = 0; j < MAX_COLUMNS; j++) cout << (seats[i][j] ? "[X] " : "[.] ");
        cout << endl;
    }

    cout << "\n================== END OF SEATING CHART ==================\n";
    waitForEnter();
}

// ===============================
// Manifest & Seat Report Menu
// ===============================
void ArraySystem::ManifestnSeatReport() {
    int choice;

    do {
        clearScreen();
        cout << "=====================================\n";
        cout << "     Manifest & Seat Report Menu     \n";
        cout << "=====================================\n";
        cout << "1. Passenger Manifest\n";
        cout << "2. Full Seating Chart Report\n";
        cout << "-------------------------------------\n";
        cout << "0. Back to Previous Menu\n";
        cout << "=====================================\n";
        cout << "Select an option: ";

        if (!(cin >> choice)) {
            flushInput();
            continue;
        }
        flushInput();

        switch (choice) {
            case 1:
                displayManifest();   // sorting menu happens INSIDE here
                break;
            case 2:
                displaySeatingChart();
                break;
            case 0:
                break;
            default:
                cout << "Invalid option.";
                waitForEnter();
        }

    } while (choice != 0);
}

// ==========================================================
// Performance Summary
// ==========================================================
void ArraySystem::displayFinalPerformance(double tI, double tS, int totalN, string winner) {
    clearScreen();
    cout << "==========================================================" << endl;
    cout << "                OVERALL PERFORMANCE SUMMARY               " << endl;
    cout << "==========================================================" << endl;
    cout << left << setw(20) << "Algorithm"
         << setw(15) << "Time (ms)"
         << "Est. Memory (Stack)" << endl;
    cout << "----------------------------------------------------------" << endl;

    cout << fixed << setprecision(2);
    cout << left << setw(20) << "Insertion Sort" << setw(15) << tI << "40.00 bytes" << endl;
    cout << left << setw(20) << "Selection Sort" << setw(15) << tS << "40.00 bytes" << endl;

    cout << "----------------------------------------------------------" << endl;
    cout << "Total Records Processed: " << totalN << endl;
    cout << "Winner (Auto-Selected):  " << winner << endl;
    cout << "----------------------------------------------------------" << endl;

    // avoid divide-by-zero
    if (tI > 0 && tS > 0) {
        double ratio = (tI > tS) ? (tI / tS) : (tS / tI);
        cout << "Analysis: " << winner << " was faster by " << ratio << "x." << endl;
    } else {
        cout << "Analysis: Timing values too small to compute ratio." << endl;
    }

    cout << "Both use O(1) stack space as they are iterative." << endl;
    cout << "==========================================================" << endl;
    waitForEnter();
}

// ==========================================================
// Manifest Display (WITH SORTING OPTIONS MENU)
// ==========================================================
void ArraySystem::displayManifest() {

    if (passengerCount == 0) {
        cout << "The flight manifest is currently empty." << endl;
        waitForEnter();
        return;
    }

    // -------------------------------
    // Sorting Options Menu
    // -------------------------------
    int sortOption;
    clearScreen();
    cout << "=== Sorting Options ===" << endl;
    cout << "1. Sort by Passenger Name" << endl;
    cout << "2. Sort by Passenger ID" << endl;
    cout << "0. No Sort (View as is)" << endl;
    cout << "Choice: ";

    if (!(cin >> sortOption)) {
        flushInput();
        return;
    }
    flushInput();

    // -------------------------------
    // Prepare Working Copy
    // -------------------------------
    // Passenger* temp[MAX_PASSENGERS];
    // for (int i = 0; i < passengerCount; i++)temp[i] = passengerList[i];

    double timeI = 0, timeS = 0;
    string winner = "None";
    bool didCompare = false;

    // -------------------------------
    // If user wants sorting (Name or ID)
    // -------------------------------
    if (sortOption == 1 || sortOption == 2) {
        char choice;
        cout << "\nPerform Sorting Performance Comparison before viewing? (y/n): ";
        cin >> choice;
        flushInput();

        if (tolower((unsigned char)choice) == 'y') {
            didCompare = true;

            cout << "\n[System] Comparing algorithms on "
                 << passengerCount << " records..." << endl;

            Passenger* clone[MAX_PASSENGERS];
            for (int i = 0; i < passengerCount; i++)
                clone[i] = passengerList[i];

            // ---- Insertion Sort timing on clone ----
            auto startI = high_resolution_clock::now();
            if (sortOption == 1) insertionSortByName(clone, passengerCount);
            else insertionSortByID(clone, passengerCount);
            auto endI = high_resolution_clock::now();
            timeI = duration<double, milli>(endI - startI).count();

            // ---- Selection Sort timing on temp ----
            auto startS = high_resolution_clock::now();
            if (sortOption == 1) selectionSortByName(passengerList, passengerCount);
            else selectionSortByID(passengerList, passengerCount);
            auto endS = high_resolution_clock::now();
            timeS = duration<double, milli>(endS - startS).count();

            winner = (timeI < timeS) ? "Insertion Sort" : "Selection Sort";

            // if insertion wins, copy clone sorted result into temp
            if (winner == "Insertion Sort") {
                for (int i = 0; i < passengerCount; i++)
                    passengerList[i] = clone[i];
            }

            cout << "\n--------------------------------------------------\n";
            cout << fixed << setprecision(2);
            cout << "Insertion Sort Time: " << timeI << " ms\n";
            cout << "Selection Sort Time: " << timeS << " ms\n";
            cout << "[Auto-Select] Winner: " << winner << " will be used for display.\n";
            cout << "Press Enter to continue...";
            waitForEnter();

        } else {
            // No compare, just do 1 default sort (Selection) based on chosen key
            if (sortOption == 1) selectionSortByName(passengerList, passengerCount);
            else selectionSortByID(passengerList, passengerCount);
        }
    }
    // sortOption == 0 => no sort, display as-is

    // -------------------------------
    // Pagination & Display
    // -------------------------------
    const int PAGE_SIZE = 20;
    int totalPages = (passengerCount + PAGE_SIZE - 1) / PAGE_SIZE;
    int currentPage = 1;
    bool viewing = true;

    while (viewing) {
        clearScreen();
        cout << "========================== PASSENGER MANIFEST ==========================" << endl;
        cout << "Page " << currentPage << " of " << totalPages
             << " (Total: " << passengerCount << ")" << endl;
        cout << "------------------------------------------------------------------------" << endl;
        cout << left << setw(15) << "ID"
             << setw(25) << "Name"
             << setw(10) << "Row"
             << setw(13) << "Col"
             << setw(15) << "Class" << endl;
        cout << "------------------------------------------------------------------------" << endl;

        int start = (currentPage - 1) * PAGE_SIZE;
        int end = min(start + PAGE_SIZE, passengerCount);

        for (int i = start; i < end; i++) {
            cout << left << setw(15) << passengerList[i]->passengerId
                 << setw(25) << passengerList[i]->name
                 << setw(10) << passengerList[i]->seatRow
                 << setw(13) << passengerList[i]->seatColumn
                 << setw(15) << passengerList[i]->flightClass << endl;
        }

        cout << "------------------------------------------------------------------------" << endl;
        cout << "N: Next | P: Prev | J: Jump | L: Last | Q: Quit" << endl;
        cout << "Action: ";

        string nav;
        getline(cin, nav);
        if (nav.empty()) continue;

        char cmd = toupper((unsigned char)nav[0]);
        if (cmd == 'N' && currentPage < totalPages) currentPage++;
        else if (cmd == 'P' && currentPage > 1) currentPage--;
        else if (cmd == 'L') currentPage = totalPages;
        else if (cmd == 'Q') viewing = false;
        else if (cmd == 'J') {
            cout << "Enter page number: ";
            int jump;
            if (cin >> jump && jump >= 1 && jump <= totalPages) currentPage = jump;
            flushInput();
        }
    }

    // show summary only if comparison actually happened
    if (didCompare)
        displayFinalPerformance(timeI, timeS, passengerCount, winner);
}

// ==========================================================
// Sorting Algorithms
// ==========================================================
void ArraySystem::insertionSortByName(Passenger* arr[], int n) {
    for (int i = 1; i < n; i++) {
        Passenger* key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j]->name > key->name) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

void ArraySystem::selectionSortByName(Passenger* arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int minIdx = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j]->name < arr[minIdx]->name)
                minIdx = j;
        }
        Passenger* tempPtr = arr[minIdx];
        arr[minIdx] = arr[i];
        arr[i] = tempPtr;
    }
}

void ArraySystem::insertionSortByID(Passenger* arr[], int n) {
    for (int i = 1; i < n; i++) {
        Passenger* key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j]->passengerId > key->passengerId) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

void ArraySystem::selectionSortByID(Passenger* arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int minIdx = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j]->passengerId < arr[minIdx]->passengerId)
                minIdx = j;
        }
        Passenger* temp = arr[minIdx];
        arr[minIdx] = arr[i];
        arr[i] = temp;
    }
}