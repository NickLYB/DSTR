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

//Will try multiple way to delete

//delete by id
//single linked list
void LinkedListSystem::deleteByIdSingle(string id) {
    if (sHead == nullptr) {
        cout << "List is empty." << endl;
        return;
    }

    int nodesVisited = 0;

    nodesVisited++;
    if (sHead->data.passengerId == id) {
        SNode* temp = sHead;
                
 

            clearScreen();
        cout << "=============================" << endl;
        cout << "Passenger: " << id << endl;
        cout << "Name: " << temp->data.name << endl;
        cout << "Seat: " << temp->data.seatRow << temp->data.seatColumn << endl;
        cout << "Class: " << temp->data.flightClass << endl;
        cout << "=============================" << endl;
        cout << "Confirm deletion? (Y/N): ";
       
        char confirm;
        cin >> confirm;
        flushInput();


        if (confirm == 'N' || confirm == 'n') {
            cout << "Deletion cancelled." << endl;
            return;
        }
        else if (confirm == 'Y' || confirm == 'y') {
            sHead = sHead->next;
            if (sHead == nullptr) sTail = nullptr;
            delete temp;
        }
        else {
            cout << "Invalid input. Deletion cancelled." << endl;
            return;
        }


        if(saveToFile("flight_passenger_data.csv")) {
            cout << "\n[Success] Passenger record deleted from file." << endl;
        }
        else{
            cout << "\n[Error] Failed to update file after deletion." << endl;
        }
        return;
    }

    SNode* current = sHead->next;
    SNode* prev = sHead;

    while (current != nullptr) {
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
            else if (confirm == 'Y' || confirm == 'y') {
                prev->next = current->next;
                if(current == sTail) sTail = prev;
                delete current;
            }
            else {
                cout << "Invalid input. Deletion cancelled." << endl;
                return;
            }
            
            if(saveToFile("flight_passenger_data.csv")) {
                cout << "\n[Success] Passenger record deleted from file." << endl;
            }
            else{
                cout << "\n[Error] Failed to update file after deletion." << endl;
            }
            return;
        }
        prev = current;
        current = current->next;
    }

    cout << "Passenger ID " << id << " not found." << endl;
}

// --- Helper: count nodes in a list ---
static int countNodes(SNode* head) {
    int n = 0;
    while (head) { n++; head = head->next; }
    return n;
}
// --- Helper: delete head (O(1)) ---
static bool deleteHeadInMemory(SNode*& head, int& stepsVisited) {
    stepsVisited = 0;
    if (!head) return false;
    stepsVisited = 1; // visited the head node
    SNode* temp = head;
    head = head->next;
    delete temp;
    return true;
}
// --- Helper: delete tail (O(n)) - safe, no dangling pointers ---
static bool deleteTailInMemory(SNode*& head, int& stepsVisited) {
    stepsVisited = 0;
    if (!head) return false;

    // If only 1 node
    if (!head->next) {
        stepsVisited = 1;
        delete head;
        head = nullptr;
        return true;
    }

    // Traverse to the end
    SNode* prev = nullptr;
    SNode* cur = head;
    while (cur->next) {
        stepsVisited++;
        prev = cur;
        cur = cur->next;
    }
    stepsVisited++; // count visiting the tail node itself

    prev->next = nullptr;
    delete cur;
    return true;
}
// --- Helper: delete at index (0-based). This simulates "random ID" well for sequential IDs ---
static bool deleteAtIndexInMemory(SNode*& head, int index, int& stepsVisited) {
    stepsVisited = 0;
    if (!head || index < 0) return false;

    if (index == 0) {
        return deleteHeadInMemory(head, stepsVisited);
    }

    SNode* prev = nullptr;
    SNode* cur = head;
    int i = 0;

    while (cur && i < index) {
        stepsVisited++;
        prev = cur;
        cur = cur->next;
        i++;
    }
    if (!cur) return false;

    stepsVisited++; // visiting the node to delete
    prev->next = cur->next;
    delete cur;
    return true;
}
// --- Helper: full traversal "miss" search by ID (no delete) ---
static int traverseMissById(SNode* head, const string& targetId) {
    int steps = 0;
    while (head) {
        steps++;
        if (head->data.passengerId == targetId) { // should never happen
            break;
        }
        head = head->next;
    }
    return steps;
}
void LinkedListSystem::deleteBenchmark() {
    if (!sHead) {
        cout << "[Info] List is empty. Nothing to benchmark.\n";
        waitForEnter();
        return;
    }

    const int BENCHMARK_OPS = 5000;

    int passengerCount = countNodes(sHead);
    int opsForDelete = (passengerCount < BENCHMARK_OPS) ? passengerCount : BENCHMARK_OPS;

    clearScreen();
    cout << string(110, '=') << endl;
    cout << string(40, ' ') << "BENCHMARK: DELETE PERFORMANCE (IN-MEMORY)" << endl;
    cout << string(110, '-') << endl;
    cout << "Dataset Size (from CSV loaded in memory): " << passengerCount << endl;
    cout << "Ops per scenario (requested): " << BENCHMARK_OPS << endl;
    cout << "Ops used for HIT scenarios (cannot exceed size): " << opsForDelete << endl;
    cout << "Note: CSV is NOT modified. No user input. No file I/O." << endl;
    cout << string(110, '-') << endl;

    cout << left
        << setw(30) << "Scenario"
        << setw(20) << "Total Time (ms)"
        << setw(20) << "Avg Time (ns/op)"
        << setw(30) << "Avg Traversal Steps"
        << setw(10) << "Frees"
        << endl;
    cout << string(110, '-') << endl;

    // Clone original once
    SNode* originalHead = cloneList(sHead);

    // Fixed seed for repeatability in report
    srand(12345);

    auto runScenario = [&](const string& name, int ops, auto deleteFunc, bool isMissScenario) {

        SNode* head = cloneList(originalHead);
        int currentSize = countNodes(head);

        long long totalSteps = 0;
        int frees = 0;

        auto start = high_resolution_clock::now();

        if (!isMissScenario) {
            for (int i = 0; i < ops; i++) {
                if (!head) break; // safety

                int steps = 0;
                bool ok = deleteFunc(head, currentSize, steps);

                totalSteps += steps;
                if (ok) { frees++; currentSize--; }
            }
        } else {
            // MISS: do BENCHMARK_OPS traversals on the full list (no deletion)
            const string missId = "NON_EXISTENT_ID_999999";
            for (int i = 0; i < BENCHMARK_OPS; i++) {
                int steps = traverseMissById(head, missId);
                totalSteps += steps;
            }
            // frees stays 0
        }

        auto stop = high_resolution_clock::now();
        auto ms = duration_cast<milliseconds>(stop - start).count();

        double avgNs = 0.0;
        double avgSteps = 0.0;

        if (!isMissScenario) {
            int usedOps = (ops == 0) ? 1 : ops;
            avgNs = (ms * 1000000) / usedOps;
            avgSteps = static_cast<double>(totalSteps) / usedOps;
        } else {
            avgNs = (ms * 1000000) / BENCHMARK_OPS;
            avgSteps = static_cast<double>(totalSteps) / BENCHMARK_OPS;
        }

        cout << left << setw(30) << name
             << setw(20) << ms
             << setw(20) << fixed << setprecision(2) << avgNs
             << setw(30) << fixed << setprecision(2) << avgSteps
             << setw(10) << frees
             << endl;

        deleteList(head);
    };

    // Scenario 1: Delete head repeatedly
    runScenario("Delete HIT (Head)",
                opsForDelete,
                [](SNode*& head, int& /*size*/, int& steps) -> bool {
                    return deleteHeadInMemory(head, steps);
                },
                false);

    // Scenario 2: Delete random position repeatedly (simulates random ID)
    runScenario("Delete HIT (RandomPos)",
                opsForDelete,
                [](SNode*& head, int& size, int& steps) -> bool {
                    if (size <= 0) return false;
                    int idx = rand() % size;
                    return deleteAtIndexInMemory(head, idx, steps);
                },
                false);

    // Scenario 3: Delete tail repeatedly (worst-case traversal)
    runScenario("Delete HIT (Tail)",
                opsForDelete,
                [](SNode*& head, int& /*size*/, int& steps) -> bool {
                    return deleteTailInMemory(head, steps);
                },
                false);

    // Scenario 4: Delete MISS (search full traversal, no deletion)
    runScenario("Delete MISS (NotFound)",
                BENCHMARK_OPS,
                [](SNode*& /*head*/, int& /*size*/, int& /*steps*/) -> bool {
                    return false;
                },
                true);

    deleteList(originalHead);

    cout << string(110, '=') << "\n";
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
