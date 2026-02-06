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

    // check if list is empty
    if (!sHead) {
        cout << "List is empty." << endl;
        return;
    }

    SNode* current = sHead; //pointer for traverse the list
    SNode* prev = nullptr;  //keep track of previous node for link adjustment
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

            //delete head node
            if (prev == nullptr) { 
                sHead = current->next;
                if (sHead == nullptr) sTail = nullptr; // update tail to null as well when list is empty
            } 
            //delete middle or tail node
            else { 
                prev->next = current->next;
                if (current == sTail) sTail = prev; //update tail if last node is removed
            }

            delete current; //free memory

            if (saveToFile("flight_passenger_data.csv"))
                cout << "[Success] Passenger record deleted from file." << endl;
            else
                cout << "[Error] Failed to update file after deletion." << endl;

            return;
        }

        //move to next node
        prev = current;
        current = current->next;
    }

    cout << "Passenger ID " << id << " not found." << endl;
}

//count the total number of nodes
static int countNodes(SNode* head) {
    int n = 0;
    while (head) { n++; head = head->next; }
    return n;
}

//helper: deletion (in memory), for benchmark
static bool deleteByIdInMemory(SNode*& head, const string& targetId, int& stepsVisited) {
    stepsVisited = 0;
    if (!head) return false;

    //pointer
    SNode* prev = nullptr;
    SNode* cur  = head;

    //linear traversal
    while (cur) {
        stepsVisited++;
        if (cur->data.passengerId == targetId) {
            if (!prev) head = cur->next; //delete head node
            else prev->next = cur->next; //delete middle/tail node

            delete cur; //free memory
            return true;
        }
        //next node
        prev = cur;
        cur = cur->next;
    }
    return false;
}

//Fisher-Yates algorithm, randomly shuffles an integer array, for benchmark delete hit random
static void shuffleIntArray(int* a, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = a[i];
        a[i] = a[j];
        a[j] = tmp;
    }
}

void LinkedListSystem::deleteBenchmark() {
    //check if list is empty
    if (!sHead) {
        cout << "\n[Benchmark] No passengers loaded.\n";
        waitForEnter();
        return;
    }

    //number of operation
    const int BENCHMARK_OPS = 5000;

    //get size and max cap hit operation to avoid deleting more than availble nodes
    int passengerCount = countNodes(sHead);
    int opsHit = (passengerCount < BENCHMARK_OPS) ? passengerCount : BENCHMARK_OPS;

    cout << string(120, '=') << endl;
    cout << string(38, ' ') << "BENCHMARK: LINKED LIST DELETE PERFORMANCE (IN-MEMORY)" << endl;
    cout << string(120, '-') << endl;
    cout << "Dataset Size (from CSV loaded in memory): " << passengerCount << endl;
    cout << "Ops per scenario (requested): " << BENCHMARK_OPS << endl;
    cout << "Ops used for HIT scenarios (cannot exceed size): " << opsHit << endl;
    cout << "Note: CSV is NOT modified. No seat map updates. No user input. No file I/O." << endl;
    cout << "Note: HIT simulates delete-by-ID = linear scan by passengerId then relink." << endl;
    cout << string(120, '-') << endl;

    cout << left
        << setw(28) << "Scenario"
        << setw(18) << "Total Time (ms)"
        << setw(18) << "Avg Time (ns/op)"
        << setw(22) << "Avg Traversal Steps"
        << setw(16) << "Avg Shifts"
        << setw(10) << "Frees"
        << setw(10) << "Result"
        << endl;
    cout << string(120, '-') << endl;

    //clone original list for benchmark
    SNode* originalHead = cloneList(sHead);

    //for target selection
    string* ids = new string[passengerCount];
    {
        SNode* cur = originalHead;
        for (int i = 0; i < passengerCount; i++) {
            ids[i] = cur->data.passengerId;
            cur = cur->next;
        }
    }

    //permutation for random deletion target
    int* perm = new int[passengerCount];
    for (int i = 0; i < passengerCount; i++) perm[i] = i;

    srand(12345);
    shuffleIntArray(perm, passengerCount);

    //benchmark scenario
    auto runScenario = [&](const string& name, bool headScenario, bool randomScenario, bool tailScenario, bool missScenario) {

        //copy list
        SNode* head = cloneList(originalHead);

        //metrics
        long long totalSteps = 0;
        long long totalShifts = 0;
        int frees = 0;

        //start timer
        auto start = high_resolution_clock::now();

        if (!missScenario) {
            for (int op = 0; op < opsHit; op++) {
                //select id based on scenario
                const string& targetId =
                    headScenario  ? ids[op] :   //delete head
                    tailScenario  ? ids[passengerCount - 1 - op] : //delete tail
                    ids[perm[op]];  //delete random

                int steps = 0;

                bool ok = deleteByIdInMemory(head, targetId, steps);
                totalSteps += steps;
                if (ok) frees++;
            }
        } else {
            //miss case
            const string missId = "NON_EXISTENT_ID_999999";
            for (int op = 0; op < BENCHMARK_OPS; op++) {
                int steps = 0;
                bool ok = deleteByIdInMemory(head, missId, steps); // should always be false
                totalSteps += steps;
                if (ok) frees++;
            }
        }

        //stop timer
        auto stop = high_resolution_clock::now();
        //calculate duration
        double ms = duration<double, milli>(stop - start).count();

        int denom = missScenario ? BENCHMARK_OPS : (opsHit > 0 ? opsHit : 1);
        double avgNs = (ms * 1000000.0) / denom;
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
            << endl;

        deleteList(head);
    };

    //run scenario
    runScenario("Delete HIT (Head)", true,  false, false, false);
    runScenario("Delete HIT (Random)", false, true, false, false);
    runScenario("Delete HIT (Tail)", false, false, true, false);
    runScenario("Delete MISS", false, false, false, true);

    //free memory
    delete[] perm;
    delete[] ids;
    deleteList(originalHead);

    cout << string(120, '=') << endl;
    waitForEnter();
}

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
