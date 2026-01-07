#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

using namespace std;

class FileManager {
public:
    void readCSVFile(const string& filename){
ifstream file(filename);
    
    if (!file.is_open()) {
        cout << "Error: Could not open file " << filename << endl;
        return;
    }

    string line;
    getline(file, line); 

    while (getline(file, line)) {
        stringstream ss(line);
        string id, name, row, col, fClass;

        getline(ss, id, ',');
        getline(ss, name, ',');
        getline(ss, row, ',');
        getline(ss, col, ',');
        getline(ss, fClass, ',');

        Passenger newPassenger(id, name, row, col, fClass);

        cout << "Loaded: " << newPassenger.name << " (" << newPassenger.passengerId << ")" << endl;
        
    }

    file.close();
    };
};