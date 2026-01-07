#include "../header/linkedlist.hpp"
#include "../header/utils.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

LinkedListSystem::LinkedListSystem() {
    head = nullptr;
}
LinkedListSystem::~LinkedListSystem() {
    Passenger* current = head;
    while (current != nullptr) {
        Passenger* nextNode = current->next;
        delete current; 
        current = nextNode;
    }
    
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

        Passenger* newNode = new Passenger(id, name, row, col, fClass);
        newNode->next = nullptr;

        if(head == nullptr) {
            head = newNode;
        } else {
            Passenger* temp = head;
            while (temp->next != nullptr) {
                temp = temp->next;
            }
            temp->next = newNode;
        }
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
                cout << "Delete Passenger selected." << endl;
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