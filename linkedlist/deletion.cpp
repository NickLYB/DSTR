#include <iostream>
#include <chrono>
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

    auto start = high_resolution_clock::now();
    int nodesVisited = 0;

    nodesVisited++;
    if (sHead->data.passengerId == id) {
        SNode* temp = sHead;
        sHead = sHead->next;

        SNode* current = sHead;
        while (current->next != nullptr && current->next->data.passengerId != id) {
            current = current->next;
        }

        if (sHead == nullptr) {
            sTail = nullptr;
        }
        
        delete temp;
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);

        cout << "Passenger " << id << " deleted (was Head)." << endl;
        cout << "[Performance] Time: " << duration.count() << " us | Nodes Visited: " << nodesVisited << endl;
        return;
    }

    SNode* current = sHead->next;
    SNode* prev = sHead;

    while (current != nullptr) {
        nodesVisited++;
        if (current->data.passengerId == id) {
            prev->next = current->next;

            if (current == sTail) {
                sTail = prev;
            }

            delete current;

            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);

            cout << "Passenger " << id << " deleted." << endl;
            cout << "[Performance] Time: " << duration.count() << " us | Nodes Visited: " << nodesVisited << endl;
            return;
        }
        prev = current;
        current = current->next;

    }
    
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Passenger ID " << id << " not found." << endl;
    cout << "[Performance] Time: " << duration.count() << " us | Nodes Visited: " << nodesVisited << endl;
}

//delete by row and column
//single linked list
void LinkedListSystem::deleteBySeatSingle(string row, string column) {
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

//interface function to choose deletion method
void LinkedListSystem::deletePassenger() {
    int choice;
    do{
        clearScreen();
        cout << "Delete Passenger Menu" << endl;
        cout << "---------------------" << endl;
        cout << "1. Delete by Passenger ID" << endl;
        cout << "2. Delete by Seat (Row and Column)" << endl;
        cout << "0. Back to Previous Menu" << endl;
        cout << "---------------------" << endl;
        cout << "Select an option: ";
        if (!(cin >> choice)) {
            cout << "Invalid input! Please enter a number." << endl;
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
                string row, column;
                cout << "Enter Seat Row: ";
                getline(cin,row);
                cout << "Enter Seat Column: ";
                getline(cin,column);
                // Call delete by Seat function
                deleteBySeatSingle(row,column);
                waitForEnter();
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
