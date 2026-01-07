#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <cstdlib>
#include <limits>

using namespace std;

inline void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

inline void waitForEnter() {
    cout << "Press Enter to continue...";
    cin.get();
}

inline void flushInput() {
    cin.clear(); 
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}
#endif // UTILS_HPP