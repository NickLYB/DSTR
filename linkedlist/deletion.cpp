#include <iostream>
#include <chrono>
#include <iomanip>
#include <cstdlib>  
#include <ctime>     
#include "../header/Passenger.hpp"
#include "../header/utils.hpp"
#include "../header/linkedlist.hpp"
using namespace std;
using namespace std::chrono;

//delete by id
//single linked list
void LinkedListSystem::deleteByIdSingle(string id) {
    if (!sHead) {
        cout << "List is empty." << endl;
        return;
    }

    SNode* current = sHead;
    SNode* prev = nullptr;
    int nodesVisited = 0;

    while (current) {
        nodesVisited++;

        if (current->data.passengerId == id) {

            clearScreen();
            cout << "=============================" << endl;
            cout << "Passenger: " << id << endl;
            cout << "Name: " << current->data.name << endl;
            cout << "Seat: " << current->data.seatRow << current->data.seatColumn << endl;
            cout << "Class: " << current->data.flightClass << endl;
            cout << "=============================" << endl;
            cout << "Confirm deletion? (Y/N): ";

            char confirm;
            cin >> confirm;
            flushInput();

            if (confirm == 'N' || confirm == 'n') {
                cout << "Deletion cancelled." << endl;
                return;
            }
            if (confirm != 'Y' && confirm != 'y') {
                cout << "Invalid input. Deletion cancelled." << endl;
                return;
            }

            if (prev == nullptr) {
                sHead = current->next;
                if (sHead == nullptr)
                    sTail = nullptr;
            } else {
                prev->next = current->next;
                if (current == sTail)
                    sTail = prev;
            }

            delete current;

            if (saveToFile("flight_passenger_data.csv"))
                cout << "[Success] Passenger record deleted from file." << endl;
            else
                cout << "[Error] Failed to update file after deletion." << endl;

            return;
        }

        prev = current;
        current = current->next;
    }

    cout << "Passenger ID " << id << " not found." << endl;
}

// --- Helper: count nodes ---
static int countNodes(SNode* head) {
    int n = 0;
    while (head) { n++; head = head->next; }
    return n;
}

// --- Helper: delete by ID in-memory (linear scan + relink + delete) ---
static bool deleteByIdInMemory(SNode*& head, const string& targetId, int& stepsVisited) {
    stepsVisited = 0;
    if (!head) return false;

    SNode* prev = nullptr;
    SNode* cur  = head;

    while (cur) {
        stepsVisited++;
        if (cur->data.passengerId == targetId) {
            if (!prev) head = cur->next;
            else       prev->next = cur->next;
            delete cur;
            return true;
        }
        prev = cur;
        cur = cur->next;
    }
    return false;
}

// --- Helper: Fisher–Yates shuffle on an int array (no vector) ---
static void shuffleIntArray(int* a, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = a[i];
        a[i] = a[j];
        a[j] = tmp;
    }
}

void LinkedListSystem::deleteBenchmark() {
    if (!sHead) {
        cout << "\n[Benchmark] No passengers loaded.\n";
        waitForEnter();
        return;
    }

    const int BENCHMARK_OPS = 5000;

    int passengerCount = countNodes(sHead);
    int opsHit = (passengerCount < BENCHMARK_OPS) ? passengerCount : BENCHMARK_OPS;

    cout << string(120, '=') << "\n";
    cout << string(38, ' ') << "BENCHMARK: LINKED LIST DELETE PERFORMANCE (IN-MEMORY)\n";
    cout << string(120, '-') << "\n";
    cout << "Dataset Size (from CSV loaded in memory): " << passengerCount << "\n";
    cout << "Ops per scenario (requested): " << BENCHMARK_OPS << "\n";
    cout << "Ops used for HIT scenarios (cannot exceed size): " << opsHit << "\n";
    cout << "Note: CSV is NOT modified. No seat map updates. No user input. No file I/O.\n";
    cout << "Note: HIT simulates delete-by-ID = linear scan by passengerId then relink.\n";
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

    // Clone original once (so we don't modify sHead)
    SNode* originalHead = cloneList(sHead);

    // Collect ALL IDs once into a dynamic array (no vector)
    string* ids = new string[passengerCount];
    {
        SNode* cur = originalHead;
        for (int i = 0; i < passengerCount; i++) {
            ids[i] = cur->data.passengerId;
            cur = cur->next;
        }
    }

    // Precompute random order indices once (no vector)
    int* perm = new int[passengerCount];
    for (int i = 0; i < passengerCount; i++) perm[i] = i;

    srand(12345);            // fixed seed for report reproducibility
    shuffleIntArray(perm, passengerCount);

    auto runScenario = [&](const string& name,
                           bool headScenario,
                           bool randomScenario,
                           bool tailScenario,
                           bool missScenario) {

        SNode* head = cloneList(originalHead);

        long long totalSteps = 0;
        long long totalShifts = 0; // always 0 for linked list, kept for UI match
        int frees = 0;

        auto start = high_resolution_clock::now();

        if (!missScenario) {
            for (int op = 0; op < opsHit; op++) {
                // Choose targetId WITHOUT traversing the working list
                // (targets come from pre-collected ids[] to keep selection cost out)
                const string& targetId =
                    headScenario  ? ids[op] :
                    tailScenario  ? ids[passengerCount - 1 - op] :
                    /*random*/      ids[perm[op]];

                int steps = 0;
                bool ok = deleteByIdInMemory(head, targetId, steps);
                totalSteps += steps;
                if (ok) frees++;
            }
        } else {
            const string missId = "NON_EXISTENT_ID_999999";
            for (int op = 0; op < BENCHMARK_OPS; op++) {
                int steps = 0;
                bool ok = deleteByIdInMemory(head, missId, steps); // should always be false
                totalSteps += steps;
                if (ok) frees++;
            }
        }

        auto stop = high_resolution_clock::now();
        double ms = duration<double, milli>(stop - start).count();

        int denom = missScenario ? BENCHMARK_OPS : (opsHit > 0 ? opsHit : 1);
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

        deleteList(head);
    };

    runScenario("Delete HIT (Head)",   true,  false, false, false);
    runScenario("Delete HIT (Random)", false, true,  false, false);
    runScenario("Delete HIT (Tail)",   false, false, true,  false);
    runScenario("Delete MISS",         false, false, false, true);

    delete[] perm;
    delete[] ids;
    deleteList(originalHead);

    cout << string(120, '=') << "\n";
    waitForEnter();
}

//delete by row and column
//single linked list
/*void LinkedListSystem::deleteBySeatSingle(string row, string column) {
    if (sHead == nullptr) {
        cout << "List is empty." << endl;
        return;
    }

    auto start = high_resolution_clock::now();
    int nodesVisited = 0;

    nodesVisited++;
    if (sHead->data.seatRow == row && sHead->data.seatColumn == column) {
        SNode* temp = sHead;
        sHead = sHead->next; 

        if (sHead == nullptr) {
            sTail = nullptr;
        }

        delete temp;

        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        
        cout << "Seat " << row << column << " freed (was Head)." << endl;
        cout << "[Performance] Time: " << duration.count() << " us | Nodes Visited: " << nodesVisited << endl;
        return;
    }

    SNode* current = sHead;
    while (current->next != nullptr) {
        nodesVisited++;
        if (current->next->data.seatRow == row && current->next->data.seatColumn == column) {
            SNode* nodeToDelete = current->next;
            
            current->next = nodeToDelete->next;

            if (nodeToDelete == sTail) {
                sTail = current; 
            }

            delete nodeToDelete;

            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);

            cout << "Seat " << row << column << " freed." << endl;
            cout << "[Performance] Time: " << duration.count() << " us | Nodes Visited: " << nodesVisited << endl;
            return;
        }
        current = current->next;
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Seat " << row << column << " not found." << endl;
    cout << "[Performance] Time: " << duration.count() << " us | Nodes Visited: " << nodesVisited << endl;
}
*/

//interface function to choose deletion method
void LinkedListSystem::deletePassenger() {
    int choice;
    do{
        clearScreen();
        cout << "===================================" << endl;
        cout << "Delete Passenger Menu" << endl;
        cout << "===================================" << endl;
        cout << "1. Delete by Passenger ID" << endl;
        cout << "2. Benchmark" << endl;
        cout << "-----------------------------------" << endl;
        cout << "0. Back to Previous Menu" << endl;
        cout << "===================================" << endl;
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
                string id;
                cout << "Enter Passenger ID: ";
                getline(cin,id);
                // Call delete by ID function
                deleteByIdSingle(id);
                waitForEnter();
                break;
            }
            case 2: {
                deleteBenchmark();
                break;
            }
            case 0:
                cout << "Returning to Previous Menu." << endl;
                break;
            default:
                cout << "Invalid choice. Please select again." << endl;
                waitForEnter();
                break;
        }
    } while (choice != 0);
    
}
