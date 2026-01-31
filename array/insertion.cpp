#include "../header/array.hpp"
#include "../header/utils.hpp"
#include <iomanip>
#include <iostream>
#include <string>
#include <cctype>   // isdigit, toupper, isalpha
#include <vector> //????
#include <algorithm> // for max
#include <chrono>

using namespace std;
using namespace std::chrono;

// --- HELPER FUNCTIONS ---

bool isNumeric(const string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!isdigit(c)) return false;
    }
    return true;
}

bool isValidName(const string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (isdigit(c)) return false;
    }
    return true;
}

string generatePassengerID(Passenger* list[], int count) {
    int maxId = 100000;
    for (int i = 0; i < count; i++) {
        if (list[i] != nullptr) {
            try {
                string sId = list[i]->passengerId;
                int currId = stoi(sId);
                if (currId > maxId) maxId = currId;
            } catch (...) {}
        }
    }
    return to_string(maxId + 1);
}

void ArraySystem::insertBenchmark(){
    const int BENCHMARK_OPS = 5000;
    
    if(passengerCount <= 0){
        cout <<"No Passenger Loaded" << endl;
        waitForEnter();
        return;
    }
    int remaining = MAX_PASSENGERS - passengerCount;
    if(remaining <= 0){
        cout << "Passenger list is full.Cannot benchmark insert." << endl;
        waitForEnter();
        return;
    }

    int maxId = 100000;
    for (int i = 0; i < passengerCount; i++) {
        if (passengerList[i] != nullptr) {
            try {
                int currId = stoi(passengerList[i]->passengerId);
                if (currId > maxId) maxId = currId;
            } catch (...) {}
        }
    }
    cout << string(100, '=') << endl;
    cout << string(25, ' ') << "BENCHMARK: ARRAY INSERT PERFORMANCE (APPEND ONLY, IN-MEMORY)\n";
    cout << string(100, '-') << "\n";
    cout << "Initial dataset size (loaded from CSV): " << passengerCount << "\n";
    cout << "Capacity (MAX_ROWS*MAX_COLUMNS): " << MAX_PASSENGERS << "\n";
    cout << "Note: CSV is NOT modified. No UI input/printing. No seat map updates.\n";
    cout << "Insertion measured: append into passengerList[passengerCount].\n";
    cout << string(100, '-') << endl;

    cout << left
         << setw(12) << "Ops (N)"
         << setw(20) << "Total Time (ms)"
         << setw(20) << "Avg Time (ns/op)"
         << setw(20) << "Avg Steps"
         << setw(15) << "Allocations"
         << endl;
    cout << string(100, '-') << endl;

    auto runCase = [&](int opsRequested) {
        int ops = opsRequested;
        if (ops > remaining) ops = remaining; 

        int originalCount = passengerCount;
        int startIndex = passengerCount;

        int allocations = 0;
        long long totalSteps = 0;

        auto start = high_resolution_clock::now();

        for (int i = 0; i < ops; i++) {
            string id = to_string(maxId + 1 + i);
            Passenger* p = new Passenger(id, "BENCH", "1", "A", "Economy");

            passengerList[passengerCount] = p;
            passengerCount++;

            allocations++;
            totalSteps += 1;
        }

        auto stop = high_resolution_clock::now();
        auto ms = duration_cast<milliseconds>(stop - start).count();
        double avgNs = (ms * 1e6) / (ops > 0 ? ops : 1);
        double avgSteps = (ops > 0) ? (double)totalSteps / ops : 0.0;

        cout << left
             << setw(12) << ops
             << setw(20) << ms
             << setw(20) << fixed << setprecision(2) << avgNs
             << setw(20) << fixed << setprecision(2) << avgSteps
             << setw(15) << allocations
             << endl;

        for (int i = startIndex; i < startIndex + ops; i++) {
            delete passengerList[i];
            passengerList[i] = nullptr;
        }
        passengerCount = originalCount;
    };

    runCase(1000);
    runCase(5000);
    runCase(10000);
    runCase(50000);

    cout << string(100, '=') << "\n";
    waitForEnter();
}

// --- MAIN INSERT FUNCTION ---
void ArraySystem::insertPassenger() {
    string name, rowStr, fClass;
    char colChar;
    int rIndex = -1;
    int cIndex = -1;

    // --- STEP 1: DISPLAY EMPTY PLACES (SEPARATED BY CLASS) ---
    // Note: Total count display removed as requested.

    clearScreen();
    cout << "=== PASSENGER INSERTION MENU ===" << endl;
    cout << "(Displaying top 15 available seats per class)\n" << endl;

    // --- DISPLAY FIRST CLASS (Rows 1-70) ---
    cout << "--- First Class (Rows 1-70) ---" << endl;
    int count = 0;
    for (int i = 0; i < 70 && i < MAX_ROWS; i++) {
        for (int j = 0; j < MAX_COLUMNS; j++) {
            if (seats[i][j] == nullptr) {
                cout << "[" << (i + 1) << "," << (char)('A' + j) << "] ";
                count++;
                if (count % 5 == 0) cout << endl; // New line every 5 seats
            }
            if (count >= 15) break;
        }
        if (count >= 15) break;
    }
    if (count == 0) cout << "(None available)";
    cout << "\n" << endl;

    // --- DISPLAY BUSINESS CLASS (Rows 71-210) ---
    cout << "--- Business Class (Rows 71-210) ---" << endl;
    count = 0;
    for (int i = 70; i < 210 && i < MAX_ROWS; i++) {
        for (int j = 0; j < MAX_COLUMNS; j++) {
            if (seats[i][j] == nullptr) {
                cout << "[" << (i + 1) << "," << (char)('A' + j) << "] ";
                count++;
                if (count % 5 == 0) cout << endl;
            }
            if (count >= 15) break;
        }
        if (count >= 15) break;
    }
    if (count == 0) cout << "(None available)";
    cout << "\n" << endl;

    // --- DISPLAY ECONOMY CLASS (Rows 211-460) ---
    cout << "--- Economy Class (Rows 211-460) ---" << endl;
    count = 0;
    for (int i = 210; i < MAX_ROWS; i++) {
        for (int j = 0; j < MAX_COLUMNS; j++) {
            if (seats[i][j] == nullptr) {
                cout << "[" << (i + 1) << "," << (char)('A' + j) << "] ";
                count++;
                if (count % 5 == 0) cout << endl;
            }
            if (count >= 15) break;
        }
        if (count >= 15) break;
    }
    if (count == 0) cout << "(None available)";
    cout << "\n-----------------------------------------------------" << endl;

    cout << "Please enter booking details below:" << right << setw(15) << "[-]:Return" << endl;

    // --- STEP 2: INPUT DETAILS ---

    // 1. AUTO-GENERATE ID
    string id = generatePassengerID(passengerList, passengerCount);
    cout << "Generated Passenger ID: " << id << endl;

    // 2. PASSENGER NAME
    while (true) {
        cout << "Enter Name: ";
        getline(cin, name);
        if(name == "-") return;
        if (isValidName(name)) break;
        cout << "Error: Name cannot contain digits or be empty.\n";
    }

    // 3. FLIGHT CLASS
    while (true) {
        cout << "Enter Flight Class (Economy/Business/First): ";
        getline(cin, fClass);
        if(fClass == "-") return;
        fClass[0] = (fClass[0]);
        if (fClass == "Economy" || fClass == "Business" || fClass == "First") {
            break;
        }
        cout << "Error: Must enter 'Economy', 'Business', or 'First' exactly.\n";
    }

    // --- STEP 3: SEAT SELECTION LOOP (Repeat if occupied) ---

    // Determine Row Limits based on Class
    int minRow, maxRow;
    if (fClass == "First") { minRow = 1; maxRow = 70; }
    else if (fClass == "Business") { minRow = 71; maxRow = 210; }
    else { minRow = 211; maxRow = 460; }

    while (true) {
        // 4. ROW VALIDATION
        while (true) {
            cout << "Enter Row (" << minRow << "-" << maxRow << " for " << fClass << "): ";
            getline(cin, rowStr);
            if(rowStr == "-") return;
            if (isNumeric(rowStr)) {
                try {
                    int tempRow = stoi(rowStr);
                    if (tempRow >= minRow && tempRow <= maxRow) {
                        rIndex = tempRow - 1;
                        break;
                    }
                } catch (...) {}
            }
            cout << "Error: For " << fClass << ", row must be between " << minRow << " and " << maxRow << ".\n";
        }

        // 5. COLUMN VALIDATION
        while (true) {
            cout << "Enter Column (A-Z): ";
            cin >> colChar;
            if(string(1, colChar) == "-") return;
            flushInput();

            colChar = toupper(colChar);

            if (colChar >= 'A' && colChar <= 'Z') {
                cIndex = colChar - 'A';
                if (cIndex < MAX_COLUMNS) break;
            }
            cout << "Error: Column must be between A and Z.\n";
        }

        // 6. CHECK AVAILABILITY
        if (seats[rIndex][cIndex] != nullptr) {
            // Seat is occupied -> Error message and Loop continues
            cout << "\n=============================" << endl;
            cout << "Error: Seat " << (rIndex + 1) << (char)('A' + cIndex) << " is ALREADY OCCUPIED." << endl;
            cout << "Please enter a different seat (Row & Column)." << endl;
            cout << "=============================\n" << endl;
            // The loop will restart, asking for Row and Column again
        } else {
            // Seat is empty -> Break loop
            break;
        }
    }

    // --- STEP 4: INSERT PASSENGER ---

    string actualColStr(1, (char)('A' + cIndex));
    Passenger* newP = new Passenger(id, name, rowStr, actualColStr, fClass);
    seats[rIndex][cIndex] = newP;

    if (passengerCount < (MAX_ROWS * MAX_COLUMNS)) {
        passengerList[passengerCount] = newP;
        passengerCount++;
    } else {
        cout << "Warning: Passenger list full, added to seat map only." << endl;
    }

    // --- SUCCESS MESSAGE ---
        cout << "\n=============================" << endl;
        cout << "Passenger booked seat successfully !" << endl;
        cout << endl;
        cout << "Passenger ID: " << newP->passengerId << endl;
        cout << "Passenger Name: " << newP->name << endl;
        cout << "Flight Class: " << newP->flightClass << endl;
        cout << "Seat: [" << (rIndex + 1) << " , " << actualColStr << "]" << endl;
        cout << endl;
        cout << "Thank you! Have a nice trip !" << endl;
        cout << "=============================" << endl;

    if(saveToFile("flight_passenger_data.csv")) {
        cout << "\n[Success] Reservation saved to file successfully." << endl;
    }
    else{
        cout << "\n[Error] Failed to save reservation to file." << endl;
    }
    waitForEnter();
}

//interface
void ArraySystem::insertPassengerMenu(){
    int choice;
    do{
        clearScreen();
        cout << "=============================" << endl;
        cout << "Insert New Passenger" << endl;
        cout << "=============================" << endl;
        cout << "1. Insert New Passenger" << endl;
        cout << "2. Benchmark Insertions" << endl;
        cout<< "------------------------------" << endl;
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
        switch(choice) {
            case 1: {
                insertPassenger();
                break;
            }
            case 2: {
                insertBenchmark();
                break;
            }
            case 0:{
                cout << "Returning to Previous Menu." << endl;
                break;
            } default:
                cout << "Invalid choice. Please select again." << endl;
                waitForEnter();
                break;
        }
    } while (choice != 0);
}