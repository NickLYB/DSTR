#include "header/Passenger.hpp"
#include "header/FileManager.hpp"

using namespace std;

int main() {
    FileManager fm;
    fm.readCSVFile("cleaned_flight_passenger_data.csv");
    return 0;
}