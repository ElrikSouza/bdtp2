#include <iostream>
#include <string>
#include <fstream>

using namespace std;

int indexOfCharacterFirstOcurrance(string str, char character, int offset) {
    return str.find_first_of(character, offset);
}
