#include "Passenger.hpp"
#include "FileManager.hpp"

using namespace std;

int main() {
    FileManager fm;
    fm.readCSVFile("flight_passenger_data.csv.csv");
    return 0;
}