#ifndef LINKEDLIST_HPP
#define LINKEDLIST_HPP

#include "Passenger.hpp"
#include <string>
#include <chrono>
using namespace std;

class LinkedListSystem {
private:
    SNode* sHead;
    SNode* sTail;

    // DNode* dHead;
    // DNode* dTail;

    SNode* cloneList(SNode* head);
    void deleteList(SNode* head);
    
    static bool compareByName(const Passenger& a, const Passenger& b);
    static bool compareById(const Passenger& a, const Passenger& b);

    // Helpers for Merge Sort
    SNode* merge(SNode* left, SNode* right, bool (*comp)(const Passenger&, const Passenger&));
    void split(SNode* source, SNode** front, SNode** back);
    SNode* mergeSortRecursive(SNode* head, bool (*comp)(const Passenger&, const Passenger&));


    SNode* getTail(SNode* cur);
    SNode* insertionSortInternal(SNode* head, bool (*comp)(const Passenger&, const Passenger&));


    //insert
    void insertPassenger();
    string generateNewId();
    bool hasDigits(string str);
    void addPassengerToList(Passenger p);
    void insertBenchmark();

    //delete
    void deleteByIdSingle(string id);
    void deleteBySeatSingle(string row, string col);
    void deleteBenchmark();
    
    //manifest
    void compareAndSortManifest(double& timeI, double& timeM, int& totalN, string& winner, bool (*comp)(const Passenger&, const Passenger&));

    void displayFinalPerformance(double tI, double tM, int totalN, string winner);
    void displayManifest();
    void getPlaneDimensions(int& maxRows, int& maxCols);
    void displaySeatingChart();

    // Search Algorithms
    //ID
    //Read-Only Searches
    bool linearSearchIterativeById(SNode* head, string id, bool showResult);
    bool linearSearchRecursiveById(SNode* head, string id, bool showResult);
    bool sentinelLinearSearchById(SNode* head, SNode* tail, string id, bool showResult);
    //Heuristic Searches
    bool moveToFrontSearchById(SNode*& head, string id);
    bool transpositionSearchById(SNode*& head, string id);
    //Name
    bool linearSearchIterativeByName(SNode* head, string name, bool showResult);
    bool linearSearchIterativeByNameContains(SNode* head, string name, bool showResult);
    // Benchmarking
    void benchmarkSearchId(SNode* head, SNode* tail, string id);
    void benchmarkSearchName(SNode* head, string id);
    int linearSearchIterativeByNameContainsCount(SNode* head, const string& name);


public:
    LinkedListSystem();
    ~LinkedListSystem();
    void loadFromFile(string filename);
    bool saveToFile(string filename);
    void run();

    void insertPassengerMenu();
    void deletePassenger();
    void searchPassenger();
    void ManifestnSeatReport();
};

#endif // LINKEDLIST_HPP