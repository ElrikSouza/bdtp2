#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "../paper.h"

#define INDEX_LAST_ATTRIBUTE 6

using namespace std;

void assign_attribute_to_paper(string word, int indexAttribute, Paper* paper);
Paper* read_csv_line(fstream* file, string* line);
