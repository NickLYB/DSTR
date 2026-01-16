#include "../header/linkedlist.hpp"
#include "../header/utils.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

LinkedListSystem::LinkedListSystem() {
    sHead = nullptr;
    sTail = nullptr;
    // dHead = nullptr;
    // dTail = nullptr;
}
LinkedListSystem::~LinkedListSystem() {
    
    SNode* sCurrent = sHead;
    while(sCurrent != nullptr){
        SNode* nextNode = sCurrent -> next;
        delete sCurrent;
        sCurrent = nextNode; 
    }
    
    //not sure if want to use
    // DNode* dCurrent = dHead;
    // while(dCurrent !=nullptr){
    //     DNode* nextNode = dCurrent -> next;
    //     delete dCurrent;
    //     dCurrent = nextNode;
    // }

    //log
    //cout << "[Memory] Linked List cleared and memory freed." << endl;
}
void LinkedListSystem::loadFromFile(string filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }
    string line;
    getline(file, line); // Skip header line

    while (getline(file, line)) {
        stringstream ss(line);
        string id, name, row, col, fClass;

        getline(ss, id, ',');
        getline(ss, name, ',');
        getline(ss, row, ',');
        getline(ss, col, ',');
        getline(ss, fClass, ',');

        Passenger p(id, name, row, col, fClass);

        SNode* newSNode = new SNode(p);
        if (sHead == nullptr) {
            sHead = newSNode;
            sTail = newSNode; 
        } else {
            sTail->next = newSNode; 
            sTail = newSNode;       
        }

        // DNode* newDNode = new DNode(p);
        // if (dHead == nullptr) {
        //     dHead = newDNode;
        //     dTail = newDNode;
        // } else {
        //     dTail->next = newDNode; 
        //     newDNode->prev = dTail; 
        //     dTail = newDNode;       
        // }
    }
    file.close();
    cout << "Loading passengers from file: " << filename << endl;
    cout << "Passengers loaded into Linkedlist successfully." << endl;
    waitForEnter();
}
void LinkedListSystem::run() {
    int choice;
    do {
        clearScreen();
        cout << "Linked List Menu" << endl;
        cout << "----------------" << endl;
        cout << "1. Insert Passenger" << endl;
        cout << "2. Delete Passenger" << endl;
        cout << "3. Search Passenger" << endl;
        cout << "4. Display Manifest" << endl;
        cout << "----------------" << endl;
        cout << "0. Back to Main Menu" << endl;
        cout << "Select an option: ";
        
        if (!(cin >> choice)) {
            cout << "Invalid input! Please enter a number." << endl;
            flushInput();
            waitForEnter(); 
            continue;
        }
        flushInput();

        switch(choice) {
            case 1:
                // Code to insert passenger
                cout << "Insert Passenger selected." << endl;
                waitForEnter();
                break;
            case 2:
                // Code to delete passenger
                LinkedListSystem::deletePassenger();
                waitForEnter();
                break;
            case 3:
                // Code to search passenger
                cout << "Search Passenger selected." << endl;
                waitForEnter();
                break;
            case 4:
                // Code to display manifest
                cout << "Display Manifest selected." << endl;
                waitForEnter();
                break;
            case 0:
                cout << "Returning to Main Menu." << endl;
                waitForEnter();
                break;
            default:
                cout << "Invalid choice. Please select again." << endl;
        }
    } while(choice != 0);
}