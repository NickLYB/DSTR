#ifndef ARRAY_HPP
#define ARRAY_HPP

#include "Passenger.hpp"
#include <string>

using namespace std;

const int MAX_ROWS = 2500; //space for insertion benckmarking
const int MAX_COLUMNS = 26;
// Maximum possible passengers is Rows * Columns
const int MAX_PASSENGERS = MAX_ROWS * MAX_COLUMNS;

class ArraySystem {
private:
    // Requirement 1: 2D Array for Seat Management
    Passenger* seats[MAX_ROWS][MAX_COLUMNS];

    // Requirement 2: 1D Array for Passenger List
    Passenger* passengerList[MAX_PASSENGERS];
    int passengerCount; // Keeps track of how many passengers are in the list

    // Sorting & Performance Helpers
    void insertionSortByName(Passenger* arr[], int n);
    void selectionSortByName(Passenger* arr[], int n);
    void displayFinalPerformance(double tI, double tS, int totalN, string winner);
    void insertionSortByID(Passenger* arr[], int n);
    void selectionSortByID(Passenger* arr[], int n);
    
    void insertPassenger();
    void insertBenchmark();
    
    void deletePassenger();
    void deleteBenchmark();
    
    int getMaxRow();

public:
    ArraySystem();
    ~ArraySystem();

    void loadFromFile(string filename);
    bool saveToFile(string filename);
    void run();

    void insertPassengerMenu();
    void deletePassengerMenu();
    void searchPassenger();
    void displayManifest();

    // Reports
    void displaySeatingChart();
    void ManifestnSeatReport();
};
#endif // ARRAY_HPP
