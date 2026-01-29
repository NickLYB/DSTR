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

    // Helpers for Merge Sort
    SNode* merge(SNode* left, SNode* right);
    void split(SNode* source, SNode** front, SNode** back);
    SNode* mergeSortRecursive(SNode* head);

    SNode* getTail(SNode* cur);
    SNode* insertionSortInternal(SNode* head);

    string generateNewId();
    bool hasDigits(string str);
    void addPassengerToList(Passenger p);

    void deleteByIdSingle(string id);
    void deleteBySeatSingle(string row, string col);

    void compareAndSortManifest(double& timeI, double& timeM, int& totalN, string& winner);

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
    void benchmarkSearch(SNode* head, SNode* tail, string id);


public:
    LinkedListSystem();
    ~LinkedListSystem();
    void loadFromFile(string filename);
    void run();

    void insertPassenger();
    void deletePassenger();
    void searchPassenger();
    void ManifestnSeatReport();
};

#endif // LINKEDLIST_HPP