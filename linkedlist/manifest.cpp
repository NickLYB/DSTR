#include <iostream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <cmath>
#include "../header/Passenger.hpp"
#include "../header/utils.hpp"
#include "../header/linkedlist.hpp"

using namespace std;

SNode* LinkedListSystem::cloneList(SNode* head) {
    if (!head) return nullptr;
    SNode* newList = new SNode(head->data);
    SNode* currentNew = newList;
    SNode* currentOld = head->next;
    while (currentOld) {
        currentNew->next = new SNode(currentOld->data);
        currentNew = currentNew->next;
        currentOld = currentOld->next;
    }
    return newList;
}

// --- Helper: Clean up cloned lists ---
void LinkedListSystem::deleteList(SNode* head) {
    while (head) {
        SNode* temp = head;
        head = head->next;
        delete temp;
    }
}

bool LinkedListSystem::compareByName(const Passenger& a, const Passenger& b) {
    return a.name <= b.name;
}

bool LinkedListSystem::compareById(const Passenger& a, const Passenger& b) {
    // String comparison works perfectly for your 6-digit IDs
    return a.passengerId <= b.passengerId;
}

SNode* LinkedListSystem::mergeSortRecursive(SNode* head, bool (*comp)(const Passenger&, const Passenger&)) {
    if (head == nullptr || head->next == nullptr) return head;

    SNode *a, *b;
    split(head, &a, &b);

    // Pass the 'comp' function down the recursion tree
    return merge(mergeSortRecursive(a, comp), mergeSortRecursive(b, comp), comp);
}

SNode* LinkedListSystem::merge(SNode* left, SNode* right, bool (*comp)(const Passenger&, const Passenger&)) {
    if (left == nullptr) return right;
    if (right == nullptr) return left;

    SNode* result = nullptr;
    // Use the function pointer instead of a hardcoded field
    if (comp(left->data, right->data)) {
        result = left;
        result->next = merge(left->next, right, comp);
    } else {
        result = right;
        result->next = merge(left, right->next, comp);
    }
    return result;
}

void LinkedListSystem::split(SNode* source, SNode** front, SNode** back) {
    SNode* fast;
    SNode* slow;
    slow = source;
    fast = source->next;

    // Fast pointer moves twice as fast as slow pointer
    while (fast != nullptr) {
        fast = fast->next;
        if (fast != nullptr) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    *front = source;
    *back = slow->next;
    slow->next = nullptr; // Break the list into two
}

// Insertion Sort
SNode* LinkedListSystem::getTail(SNode* cur) {
    while (cur != nullptr && cur->next != nullptr) cur = cur->next;
    return cur;
}

SNode* LinkedListSystem::insertionSortInternal(SNode* head, bool (*comp)(const Passenger&, const Passenger&)) {
    if (head == nullptr || head->next == nullptr) return head;

    SNode* sorted = nullptr;
    SNode* current = head;
    while (current != nullptr) {
        SNode* nextNode = current->next;
        if (sorted == nullptr || comp(current->data, sorted->data)) {
            current->next = sorted;
            sorted = current;
        } else {
            SNode* temp = sorted;
            while (temp->next != nullptr && !comp(current->data, temp->next->data)) {
                temp = temp->next;
            }
            current->next = temp->next;
            temp->next = current;
        }
        current = nextNode;
    }
    return sorted;
}

// --- FINAL PERFORMANCE SUMMARY ---
// Generates the evaluation data required for the solution work documentation
void LinkedListSystem::displayFinalPerformance(double tI, double tM, int totalN, string winner) {
    clearScreen();

    // Theoretical Memory Calculation
    // Insertion: ~4 pointers (curr, next, sorted, temp) * 8 bytes
    double memI = 5 * sizeof(SNode*);

    // Merge: depth of log2(N) * (local pointers a, b + return address)
    // For 10,000 records, depth is ~14.
    double memM = 0;
    if (totalN > 0) {
        double depth = log2(static_cast<double>(totalN));
        // Each recursive call stores return address + 3 pointers (left, right, result)
        memM = depth * (4 * sizeof(SNode*));
    }

    cout << "==========================================================" << endl;
    cout << "             OVERALL PERFORMANCE SUMMARY                  " << endl;
    cout << "==========================================================" << endl;
    cout << left << setw(20) << "Algorithm" << setw(15) << "Time (ms)" << "Est. Memory (Stack)" << endl;
    cout << "----------------------------------------------------------" << endl;

    cout << left << setw(20) << "Insertion Sort"
         << setw(15) << fixed << setprecision(2) << tI
         << memI << " bytes" << endl;

    cout << left << setw(20) << "Merge Sort"
         << setw(15) << fixed << setprecision(2) << tM
         << memM << " bytes" << endl;

    cout << "----------------------------------------------------------" << endl;
    cout << "Total Records Processed: " << totalN << endl;
    cout << "Winner (Auto-Selected):  " << winner << endl;
    cout << "----------------------------------------------------------" << endl;
    cout << "Analysis: Merge Sort is " << (tI / tM) << "x faster but uses" << endl;
    cout << "more stack memory due to recursive depth." << endl;
    cout << "==========================================================" << endl;
}

// --- Performance Comparison Test ---
void LinkedListSystem::compareAndSortManifest(double& timeI, double& timeM, int& totalN, string& winner, bool (*comp)(const Passenger&, const Passenger&)) {
    if (sHead == nullptr || sHead->next == nullptr) return;

    totalN = 0;
    SNode* countTemp = sHead;
    while(countTemp != nullptr) {
        totalN++;
        countTemp = countTemp->next;
    }

    cout << "\n[System] Performing actual timing comparison on " << 10000 << "+ records..." << endl;

    // 1. Time Actual Insertion Sort on a CLONE (so we don't mess up the original for the next test)
    cout << "Testing Insertion Sort... Please wait..." << endl;
    SNode* cloneForI = cloneList(sHead);
    auto startI = chrono::high_resolution_clock::now();
    cloneForI = insertionSortInternal(cloneForI, comp);
    auto endI = chrono::high_resolution_clock::now();
    timeI = chrono::duration<double, std::milli>(endI - startI).count();

    // 2. Time Actual Merge Sort on the main list
    cout << "Testing Merge Sort..." << endl;
    auto startM = chrono::high_resolution_clock::now();
    sHead = mergeSortRecursive(sHead, comp);
    sTail = getTail(sHead);
    auto endM = chrono::high_resolution_clock::now();
    timeM = chrono::duration<double, std::milli>(endM - startM).count();

    // 3. Automated Selection
    if (timeI < timeM) {
        winner = "Insertion Sort";
        // If Insertion was miraculously faster, replace main list with the sorted clone
        deleteList(sHead);
        sHead = cloneForI;
        sTail = getTail(sHead);
    } else {
        winner = "Merge Sort";
        // Merge sort already sorted sHead, so just clean up the test clone
        deleteList(cloneForI);
    }

    cout << "------------------------------------------------" << endl;
    cout << "Insertion Sort Time: " << fixed << setprecision(2) << timeI << " ms" << endl;
    cout << "Merge Sort Time:     " << fixed << setprecision(2) << timeM << " ms" << endl;
    cout << "[Auto-Select] Winner: " << winner << " will be used for display." << endl;
    waitForEnter();
}

void LinkedListSystem::displayManifest() {
    if (sHead == nullptr) {
        cout << "The flight manifest is currently empty." << endl;
        waitForEnter();
        return;
    }

    double timeI = 0, timeM = 0;
    int totalN = 0;
    string winner = "None";

    int choice;
    while(true){
    clearScreen();
    cout << "=============================" << endl;
    cout << "      Sorting Options" << endl;
    cout << "=============================" << endl;
    cout << "1. Sort by Passenger Name" << endl;
    cout << "2. Sort by Passenger ID" << endl;
    cout << "3. No Sort (View as is)" << endl;
    cout << "-----------------------------" << endl;
    cout << "0. Back to Previous Menu" << endl;
    cout << "=============================" << endl;
    cout << "Choice: ";
    if(!(cin >> choice)){
        cout << "Invalid input! Please enter a number." << endl;
            choice = -1;
            flushInput();
            waitForEnter(); 
            continue;
    }
    flushInput();

    if (choice == 1) {
        compareAndSortManifest(timeI, timeM, totalN, winner, compareByName);
        break;
    } else if (choice == 2) {
        compareAndSortManifest(timeI, timeM, totalN, winner, compareById);
        break;
    } else if (choice == 3) {
        // No sorting, just view as-is
        break;
    } else if (choice == 0) {
        return;
    }
    else{
        cout << "Invalid choice. Please select again." << endl;
        waitForEnter();
        continue;
    }

    }
    // Pagination Logic
    const int PAGE_SIZE = 20;
    int totalPassengers = 0;
    SNode* temp = sHead;
    while (temp != nullptr) { totalPassengers++; temp = temp->next; }
    int totalPages = (totalPassengers + PAGE_SIZE - 1) / PAGE_SIZE;

    int currentPage = 1;
    bool viewing = true;

    while (viewing) {
        clearScreen();
        cout << "========================== PASSENGER MANIFEST ==========================" << endl;
        cout << "Page " << currentPage << " of " << totalPages << " (Total: " << totalPassengers << ")" << endl;
        cout << "------------------------------------------------------------------------" << endl;
        cout << left << setw(15) << "ID" << setw(25) << "Name" << setw(10) << "Row" << setw(13) << "Col" << setw(15) << "Class" << endl;
        cout << "------------------------------------------------------------------------" << endl;

        SNode* current = sHead;
        for (int i = 0; i < (currentPage - 1) * PAGE_SIZE && current != nullptr; i++) {
            current = current->next;
        }

        for (int i = 0; i < PAGE_SIZE && current != nullptr; i++) {
            cout << left << setw(15) << current->data.passengerId
                 << setw(25) << current->data.name
                 << setw(10) << current->data.seatRow
                 << setw(13) << current->data.seatColumn
                 << setw(15) << current->data.flightClass << endl;
            current = current->next;
        }

        cout << "------------------------------------------------------------------------" << endl;
        cout << "N: Next | P: Prev | J: Jump | L: Last | Q: Quit and View Performance" << endl;
        cout << "Action: ";

        string nav;
        getline(cin, nav);
        if (nav.empty()) continue;
        char cmd = toupper(nav[0]);

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

    // Requirement: Show performance AFTER viewing
    if (winner != "None") {
        displayFinalPerformance(timeI, timeM, totalN, winner);
    }
}

void LinkedListSystem::getPlaneDimensions(int& maxRows, int& maxCols) {
    maxRows = 0;
    maxCols = 0;
    SNode* current = sHead;

    while (current != nullptr) {
        try {
            int r = stoi(current->data.seatRow);
            int c = toupper(current->data.seatColumn[0]) - 'A' + 1;

            if (r > maxRows) maxRows = r;
            if (c > maxCols) maxCols = c;
        } catch (...) { /* Skip malformed data */ }
        current = current->next;
    }
}

void LinkedListSystem::displaySeatingChart() {
    if (sHead == nullptr) {
        cout << "No data available to generate a seating chart." << endl;
        waitForEnter();
        return;
    }

    int maxRows, maxCols;
    getPlaneDimensions(maxRows, maxCols);

    // 1. Create 2D grid representation
    bool** grid = new bool*[maxRows];
    for (int i = 0; i < maxRows; i++) {
        grid[i] = new bool[maxCols]{false};
    }

    // 2. Map Linked List data to the grid (O(N) traversal)
    SNode* current = sHead;
    while (current != nullptr) {
        try {
            int r = stoi(current->data.seatRow) - 1;
            int c = toupper(current->data.seatColumn[0]) - 'A';
            if (r >= 0 && r < maxRows && c >= 0 && c < maxCols) {
                grid[r][c] = true;
            }
        } catch (...) {}
        current = current->next;
    }

    clearScreen();
    cout << "========================== FULL AIRCRAFT SEATING CHART ==========================" << endl;
    cout << "Legend: [X] Occupied  [.] Available" << endl;

    for (int i = 0; i < maxRows; i++) {
        int rowNum = i + 1;

        // --- SECTION HEADERS ---
        // These headers will appear exactly at the start of each class range
        if (rowNum == 1) {
            cout << "\n[ SECTION: FIRST CLASS (Rows 1-70) ]" << endl;
            cout << "       ";
            for(int j = 0; j < maxCols; j++) cout << (char)('A' + j) << "   ";
            cout << "\n" << string(maxCols * 4 + 6, '-') << endl;
        }
        else if (rowNum == 71) {
            cout << "\n\n[ SECTION: BUSINESS CLASS (Rows 71-210) ]" << endl;
            cout << "       ";
            for(int j = 0; j < maxCols; j++) cout << (char)('A' + j) << "   ";
            cout << "\n" << string(maxCols * 4 + 6, '-') << endl;
        }
        else if (rowNum == 211) {
            cout << "\n\n[ SECTION: ECONOMY CLASS (Rows 211-460) ]" << endl;
            cout << "       ";
            for(int j = 0; j < maxCols; j++) cout << (char)('A' + j) << "   ";
            cout << "\n" << string(maxCols * 4 + 6, '-') << endl;
        }

        // 3. Print Row Data
        cout << setw(3) << rowNum << " | ";
        for (int j = 0; j < maxCols; j++) {
            if (grid[i][j]) cout << "[X] ";
            else cout << "[.] ";
        }
        cout << endl;
    }

    // 4. Memory Cleanup
    for (int i = 0; i < maxRows; i++) delete[] grid[i];
    delete[] grid;

    cout << "\n========================== END OF SEATING CHART ==========================" << endl;
}

void LinkedListSystem::ManifestnSeatReport(){

    int choice;
    do{
        clearScreen();

        cout << "======================================" << endl;
        cout << "     Manifest & Seat Report Menu      " << endl;
        cout << "======================================" << endl;
        cout << "1. Passenger Manifest" << endl;
        cout << "2. Full Seating Chart Report" << endl;
        cout << "---------------------------------------" << endl;
        cout << "0. Back to Previous Menu" << endl;
        cout << "======================================" << endl;
        cout << "Select an option: ";

        if(!(cin >> choice)){
            cout << "Invalid input! Please enter a number." << endl;
            flushInput();
            choice = -1;
            waitForEnter();
            continue;
        }
        flushInput();

        switch(choice){
            case 1: {
                displayManifest();
                waitForEnter();
                break;
            }
            case 2: {
                // Call delete by Seat function
                displaySeatingChart();
                waitForEnter();
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

