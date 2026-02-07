#include <iostream>
#include <chrono>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <iomanip>

#include "../header/Passenger.hpp"
#include "../header/utils.hpp"
#include "../header/linkedlist.hpp"

using namespace std;
using namespace std::chrono;

//1. Linear Search (Iterative)
bool LinkedListSystem::linearSearchIterativeById(SNode* head, string id, bool showResult){
    SNode* current = head;

    while(current != nullptr){
        if(current ->data.passengerId == id){
            if(showResult){
                cout << left << setw(30) << "Linear Search (Iterative)"
                << setw(15) << current->data.passengerId 
                << setw(20) << current->data.name
                << current->data.seatRow << current->data.seatColumn
                << right << setw(15) << current->data.flightClass;
            }
            return true;
        }
        current = current->next;
    }
    if(showResult) cout << "Linear Search (Iterative) Result: Passenger not found.";
    return false;
}

//2. Linear Search (Recursive)
bool LinkedListSystem::linearSearchRecursiveById(SNode* head, string id, bool showResult){
    if(head == nullptr){
        if(showResult) cout << "Linear Search (Recursive) Result: Passenger not found.";
        return false;
    }
    if(head->data.passengerId == id){
        if(showResult){
            cout << left << setw(30) << "Linear Search (Recursive)"
            << setw(15) << head->data.passengerId 
            << setw(20) << head->data.name
            << head->data.seatRow << head->data.seatColumn
            << right << setw(15) << head->data.flightClass;
        }
        return true;
    }
    return linearSearchRecursiveById(head->next, id, showResult);
}

//3. Skip List Search (need sorted list)
// structure
class skipListNode {
public:
    Passenger* passenger;
    skipListNode** forward;
    int nodeLevel;

    skipListNode(Passenger* p, int level);
    ~skipListNode();
};
skipListNode::skipListNode(Passenger* p, int level) { //constructor
    passenger = p;
    nodeLevel = level;
    forward = new skipListNode*[level + 1];
    memset(forward, 0, sizeof(skipListNode*) * (level + 1));
}
skipListNode::~skipListNode() { //destructor
    delete[] forward;
}
// class/interface
class SkipList {
private:
    int MAXLVL; 
    float P; //probability used to general random level
    int level; // higest level in list
    skipListNode* header; // head node

    int randomLevel(); //generate random level for new node insertion
    skipListNode* createNode(Passenger* p, int nodeLevel); //allocation a new node with given level

public:
    SkipList(int maxlvl = 16, float prob = 0.5f);
    ~SkipList();

    void insertElement(Passenger* p); //load passenger to skip list
    bool searchElement(const string& id, bool showResult) const; //search passenger by id
};
//construction
SkipList::SkipList(int maxlvl, float prob) { //constructor
    MAXLVL = maxlvl;
    P = prob;
    level = 0;

    static bool seeded = false;
    if (!seeded) {
        srand((unsigned)time(nullptr));
        seeded = true;
    }

    header = new skipListNode(nullptr, MAXLVL); // dummy smallest
}
SkipList::~SkipList() { //destructor
    skipListNode* cur = header->forward[0];
    while (cur != nullptr) {
        skipListNode* nxt = cur->forward[0];
        delete cur;
        cur = nxt;
    }
    delete header;
}
// helpers
int SkipList::randomLevel() { //generate random height for a new node
    int lvl = 0;
    while (((float)rand() / RAND_MAX) < P && lvl < MAXLVL) {
        lvl++;
    }
    return lvl;
}
skipListNode* SkipList::createNode(Passenger* p, int nodeLevel) { //create a new node with nodelevel forward pointer
    return new skipListNode(p, nodeLevel);
}
// operations
void SkipList::insertElement(Passenger* p) { //insert passenger into skiplist
    
    
    skipListNode* update[64]; //stores the last node before insertion at each level
    skipListNode* current = header;

    //traverse from highest level down to level 0 tofind insert position
    for (int i = level; i >= 0; i--) {
        while (current->forward[i] != nullptr &&
               current->forward[i]->passenger->passengerId < p->passengerId) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    // Move to possible duplicate at level 0
    current = current->forward[0];

    //terminate if duplicate key and opdate passenger pointer
    if (current != nullptr &&
        current->passenger->passengerId == p->passengerId) {
        current->passenger = p;
        return;
    }

    //generate random level for new node
    int rlevel = randomLevel();

    //increase list level if new node exceed current height
    if (rlevel > level) {
        for (int i = level + 1; i <= rlevel; i++) update[i] = header;
        level = rlevel;
    }

    //create new node and link it
    skipListNode* n = createNode(p, rlevel);

    for (int i = 0; i <= rlevel; i++) {
        n->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = n;
    }
}
bool SkipList::searchElement(const string& key, bool showResult) const { //search passengerId
    skipListNode* current = header;

    //traverse from top level down to locate target position
    for (int i = level; i >= 0; i--) {
        while (current->forward[i] != nullptr &&
               current->forward[i]->passenger != nullptr &&
               current->forward[i]->passenger->passengerId < key) {
            current = current->forward[i];
        }
    }

    //candidate node at level 0
    current = current->forward[0];


    if (current != nullptr &&
        current->passenger != nullptr &&
        current->passenger->passengerId == key) {

        if (showResult) {
            cout << left << setw(30) << "Skip List Search"
                << setw(15) << current->passenger->passengerId 
                << setw(20) << current->passenger->name
                << current->passenger->seatRow << current->passenger->seatColumn
                << right << setw(15) << current->passenger->flightClass;
        }
        return true;
    }

    if (showResult)
        cout << "Skip List Search Result: Passenger not found.";

    return false;
}
// linked list -> skip list
static void buildSkipListFromLinkedList(SkipList& sl, SNode* head) {
    for (SNode* cur = head; cur != nullptr; cur = cur->next) {
        sl.insertElement(&cur->data);
    }
}
static bool skipListSearchFromLinkedList(SNode* head, const string& id, bool showResult) {
    SkipList sl(16, 0.5f);
    buildSkipListFromLinkedList(sl, head);
    return sl.searchElement(id, showResult);
}

//4. Sentinel Linear Search
bool LinkedListSystem::sentinelLinearSearchById(SNode* head,SNode* tail, string id, bool showResult){
    if (head == nullptr || tail == nullptr) {
        if (showResult) cout << "Sentinel Linear Search Result: Passenger not found.";
        return false;
    }

    Passenger sentinelData;
    sentinelData.passengerId = id;
    
    SNode sentinelNode(sentinelData);
    sentinelNode.next = nullptr; 

    SNode* oldNext = tail->next;
    tail->next = &sentinelNode;

    SNode* current = head;
    while (current->data.passengerId != id){
        current = current->next;
    }

    tail->next = oldNext;

    if(current == &sentinelNode){
        if(showResult) cout << "Sentinel Linear Search Result: Passenger not found.";
        return false;
    } 

    if(showResult){
        cout << left << setw(30) << "Sentinel Linear Search"
        << setw(15) << current->data.passengerId 
        << setw(20) << current->data.name
        << current->data.seatRow << current->data.seatColumn
        << right << setw(15) << current->data.flightClass;
    }
    return true;
}

//Search by Name
// 1. Linear Search (Iterative) (Exact Search)
bool LinkedListSystem::linearSearchIterativeByName(SNode* head, string name, bool showResult){
    bool found = false;

    if(showResult){
        cout << "Search by Name Result:" << endl;
        cout << "-------------------------------------------------------------------------" << endl;
        cout << left << setw(15) << "ID" << setw(25) << "Name" << setw(10) << "Row" << setw(13) << "Col" << setw(15) << "Class" << endl;
        cout << "-------------------------------------------------------------------------" << endl;
    }

    for(SNode* current = head; current != nullptr; current = current->next){
        if(current->data.name == name){
            found = true;
            if(showResult){
                cout << left << setw(15) << current->data.passengerId
                    << setw(25) << current->data.name
                    << setw(10) << current->data.seatRow
                    << setw(13) << current->data.seatColumn
                    << setw(15) << current->data.flightClass << endl;
            }
        }
    }

    if (!found && showResult) {
        cout << "No passenger found with the name: " << name << endl;
    } else if (showResult) {
        cout << "-------------------------------------------------------------------------" << endl;
    }
    return found;
}
// Contain Search
bool LinkedListSystem::linearSearchIterativeByNameContains(SNode* head, string name, bool showResult){
    bool found = false;

    if(showResult){
        clearScreen();
        cout << "Search by Name (Contains) Result:" << endl;
        cout << "-------------------------------------------------------------------------" << endl;
        cout << left << setw(15) << "ID" << setw(25) << "Name" << setw(10) << "Row" << setw(13) << "Col" << setw(15) << "Class" << endl;
        cout << "-------------------------------------------------------------------------" << endl;
    }

    for(SNode* current = head; current != nullptr; current = current->next){
        if(current->data.name.find(name) != string::npos){
            found = true;
            if(showResult){
                cout << left << setw(15) << current->data.passengerId
                    << setw(25) << current->data.name
                    << setw(10) << current->data.seatRow
                    << setw(13) << current->data.seatColumn
                    << setw(15) << current->data.flightClass << endl;
            }
        }
    }

    if (!found && showResult) {
        cout << "No passenger found containing the name: " << name << endl;
    } else if (showResult) {
        cout << "-------------------------------------------------------------------------" << endl;
    }
    return found;
}
int LinkedListSystem::linearSearchIterativeByNameContainsCount(SNode* head, const string& name){
    int count = 0;
    for(SNode* current = head; current != nullptr; current = current->next){
        if(current->data.name.find(name) != string::npos){
            count++;
        }
    }
    return count;
}

//Benchmarking
void LinkedListSystem::benchmarkSearchId(SNode* head, SNode* tail, string id){
    int iteration = 10000;
    volatile int sink = 0;
    using DoubleMs = duration<double, std::milli>;
    SkipList sl(16, 0.5f);
    buildSkipListFromLinkedList(sl, head);
    
    size_t N = 0;
    for (SNode* cur = head; cur != nullptr; cur = cur->next) N++;

    auto printRow = [&](const string& algo, double totalMs, size_t memBytes) {
        double avgMs = totalMs / iteration;
        cout << left << setw(30) << algo
             << setw(20) << fixed << setprecision(4) << totalMs
             << setw(20) << fixed << setprecision(6) << avgMs
             << setw(20) << memBytes
             << "\n";
    };

    clearScreen();
    cout << "Target ID: " << id << endl;
    cout << "=============================================================================================" << endl;
    cout << "                        BENCHMARK: SEARCH PERFORMANCE (" << iteration << " ITERATIONS)                      " << endl;
    cout << "=============================================================================================" << endl;
    cout << left << setw(30) << "Algorithm" << setw(20) << "Total Time (ms)" << setw(20) << "Avg Time (ms)" << setw(20) << "Est. Memory (Stack)" << endl;
    cout << "---------------------------------------------------------------------------------------------" << endl;

    // Est. stack memory (bytes)
    // Iterative linear: local SNode* current
    const size_t memLinearIter = sizeof(SNode*);
    // Sentinel search: local Passenger data + sentinel node + pointer variables
    const size_t memSentinel = sizeof(Passenger) + sizeof(SNode) + sizeof(SNode*) + sizeof(SNode*);
    // Skip list search: local skipListNode* current + loop index
    const size_t memSkipSearch = sizeof(skipListNode*) + sizeof(int);
    // Recursive linear search: worst-case N stack frames
    // Each frame holds: SNode* + string + bool
    const size_t memRecFrame = sizeof(SNode*) + sizeof(string) + sizeof(bool);
    const size_t memLinearRecWorst = N * memRecFrame;
    
    //1. Linear Search (Iterative)
    auto start = high_resolution_clock::now();
    for(int i = 0; i < iteration; i++)linearSearchIterativeById(head, id, false);
    auto end = high_resolution_clock::now();
    DoubleMs duration = end - start;
    printRow("Linear Search (Iterative)", duration.count(), memLinearIter);

    //2. Linear Search (Recursive)
    start = high_resolution_clock::now();
    for(int i = 0; i < iteration; i++) linearSearchRecursiveById(head, id, false);
    end = high_resolution_clock::now();
    duration = end - start;
    printRow("Linear Search (Recursive)", duration.count(), memLinearRecWorst);

    //3. Skip List Search
    start = high_resolution_clock::now();
    for (int i = 0; i < iteration; i++) sink += sl.searchElement(id,false);
    end = high_resolution_clock::now();
    duration = end - start;
    printRow("Skip List Search", duration.count(), memSkipSearch);

    //4. Sentinel Linear Search
    start = high_resolution_clock::now();
    for(int i = 0; i < iteration; i++) sentinelLinearSearchById(head, tail, id, false);
    end = high_resolution_clock::now();
    duration = end - start;
    printRow("Sentinel Linear Search", duration.count(), memSentinel);

    cout << "---------------------------------------------------------------------------------------------" << endl;
    cout << "N (nodes): " << N << "\n";
    cout << "Note: Est. Stack (bytes) counts only local stack objects (sizeof), not ABI overhead or heap.\n";
    cout << "Note: Skip list build time + heap overhead are excluded (search-only timing).\n";
    cout << "=============================================================================================" << endl;

    (void)sink;
}
void LinkedListSystem::benchmarkSearchName(SNode* head, string name){
    int iteration = 10000;
    volatile int sink = 0;
    using DoubleMs = duration<double, std::milli>;
    size_t N = 0;
    for (SNode* cur = head; cur != nullptr; cur = cur->next) N++;
    auto printRow = [&](const string& algo, int matches, double totalMs, size_t memBytes) {
        double avgMs = totalMs / iteration;
        cout << left << setw(30) << algo
             << setw(20) << fixed << setprecision(4) << matches
             << setw(20) << fixed << setprecision(6) << totalMs
             << setw(20) << fixed << setprecision(6) << avgMs
             << setw(20) << memBytes
             << "\n";
    };
    clearScreen();
    cout << "Target Name: " << name << endl;
    cout << "=================================================================================================================" << endl;
    cout << "                          BENCHMARK: SEARCH PERFORMANCE (" << iteration << " ITERATIONS)                         " << endl;
    cout << "=================================================================================================================" << endl;
    cout << left << setw(30) << "Algorithm" << setw(20) << "Matches Found" << setw(20) << "Total Time (ms)" << setw(20) << "Avg Time (ms)" << setw(20) << "Est. Memory (Stack)" << endl;
    cout << "-----------------------------------------------------------------------------------------------------------------" << endl;

    const size_t memLinearIter = sizeof(SNode*) + sizeof(int);
    int matches = 0;
    auto start = high_resolution_clock::now();
    for(int i = 0; i < iteration; i++){
        matches = linearSearchIterativeByNameContainsCount(head, name);
        sink += matches;
    }
    
    auto end = high_resolution_clock::now();
    DoubleMs duration = end - start;
    printRow("Linear Search (Contains)", matches, duration.count(), memLinearIter);
    cout << "-----------------------------------------------------------------------------------------------------------------" << endl;
    cout << "N (nodes): " << N << endl;
    cout << "Note: Full traversal performed for every iteration." << endl;
    cout << "=================================================================================================================" << endl;
    (void)sink;
}

//interface
void LinkedListSystem::searchPassenger() {
    int choice;
    do{
        clearScreen();
        cout << "==========================" << endl;
        cout << "Search for Passenger" << endl;
        cout << "==========================" << endl;
        cout << "1. Passenger ID" << endl;
        cout << "2. Name" << endl;
        cout << "--------------------------" << endl;
        cout << "0. Back to Previous Menu." << endl;
        cout << "==========================" << endl;
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

                    clearScreen();
                    cout << "Search Results for Passenger ID: " << id << endl;
                    cout << "=====================================================================================" << endl;
                    cout << left << setw(30) << "Algorithm"
                         << setw(15) << "ID"
                         << setw(20) << "Name"
                         << "Seat"
                         << right << setw(15) << "Class" << endl;
                    cout << "-------------------------------------------------------------------------------------" << endl;
                    linearSearchIterativeById(sHead, id, true);
                    cout << endl;
                    linearSearchRecursiveById(sHead, id, true);
                    cout << endl;
                    bool skipFound = skipListSearchFromLinkedList(sHead, id,true);
                    cout << endl;
                    sentinelLinearSearchById(sHead,sTail, id, true);
                    cout << endl;
                    cout << "-------------------------------------------------------------------------------------" <<  endl;
                    cout << "Search Complete. Wanting to benchmark search times? (y/n): ";
                    char benchChoice;
                    cin >> benchChoice;
                    flushInput();
                    if(benchChoice == 'y' || benchChoice == 'Y') {
                        benchmarkSearchId(sHead, sTail, id);
                        waitForEnter();
                    }
                    else if(benchChoice == 'n' || benchChoice == 'N'){
                        //do nothing, just skip to end
                    }
                    else{
                        cout << "Invalid choice. Returning to search menu." << endl;
                        waitForEnter();
                    }
                    break;
                }
                case 2: {
                    string name;
                    cout << "Enter Passenger Name: ";
                    getline(cin,name);
                    linearSearchIterativeByNameContains(sHead, name, true);
                    cout << endl;
                    cout << "Search Complete. Wanting to benchmark search times? (y/n): ";
                    char benchChoice;
                    cin >> benchChoice;
                    flushInput();
                    if(benchChoice == 'y' || benchChoice == 'Y') {
                        benchmarkSearchName(sHead, name);
                    }
                    else if(benchChoice == 'n' || benchChoice == 'N'){
                        //do nothing, just skip to end
                    }
                    else{
                        cout << "Invalid choice. Returning to search menu." << endl;
                        waitForEnter();
                    }
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
        }
    while (choice != 0);

}
