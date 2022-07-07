#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <sstream>
#include <string>
#include "paper.cpp"
#include "utils.cpp"

using namespace std;

int main() {
    string line;

    fstream file("artigo.csv", ios::in);
    if (file.is_open()) {
        int i = 0;
        while (getline(file, line)) {
            int indexAttribute = 0;
            int indexCharacter = 0;
            Paper paper;
            while (indexCharacter < line.length()) {
                string word;

                if (line[indexCharacter] == ';' && line[indexCharacter + 1] == ';') {
                    word = "";

                    switch(indexAttribute) {
                        case 0:
                            paper.id = std::stoi(word);

                            break;
                        case 1:
                            paper.title = (char*) malloc(sizeof(char) * word.length() + 1);
                            word.copy(paper.title, word.length());

                            break;
                        case 2:
                            paper.year = std::stoi(word);

                            break;
                        case 3:
                            paper.authors = (char*) malloc(sizeof(char) * word.length() + 1);
                            word.copy(paper.authors, word.length());

                            break;
                        case 4:
                            paper.citations = std::stoi(word);

                            break;
                        case 5:
                            word.copy(paper.updated_at, word.length());

                            break;
                        default:
                            paper.snippet = (char*) malloc(sizeof(char) * word.length() + 1);
                            word.copy(paper.snippet, word.length());

                            // cout << "line = " << line << "\n";

                            break;
                    }
                    indexAttribute++;
                    indexCharacter++;

                }

                if (line[indexCharacter] == '"') {
                    int indexCloseQuotes = indexOfCharacterFirstOcurrance(line, '"', indexCharacter + 1);
                    while(line[indexCloseQuotes + 1] == '"') {
                        indexCloseQuotes = indexOfCharacterFirstOcurrance(line, '"', indexCloseQuotes + 2);
                    }
                    int indexStartData = indexCharacter + 1;
                    // cout << "indexAttribute = " << indexAttribute << "\n";
                    // cout << "line[indexStartData] = " << line[indexStartData] << "\n";
                    // cout << "line[indexStartData + 1] = " << line[indexStartData + 1] << "\n";



                    if (line[indexStartData] == ';' && line[indexStartData + 1] == ';') {
                        // cout << ">>>true\n";
                        
                        word = "";
                    } else {
                        // cout << ">>>false\n";
                        word = line.substr(indexStartData, indexCloseQuotes - indexStartData);
                    }

                    // cout << "word = " << word << "\n";
                    // cout << "indexCloseQuotes = " << indexCloseQuotes << "\n";
                    // cout << "indexStartData = " << indexStartData << "\n";

                    switch(indexAttribute) {
                        case 0:
                            paper.id = std::stoi(word);

                            break;
                        case 1:
                            paper.title = (char*) malloc(sizeof(char) * word.length() + 1);
                            word.copy(paper.title, word.length());

                            break;
                        case 2:
                            paper.year = std::stoi(word);

                            break;
                        case 3:
                            paper.authors = (char*) malloc(sizeof(char) * word.length() + 1);
                            word.copy(paper.authors, word.length());

                            break;
                        case 4:
                            paper.citations = std::stoi(word);

                            break;
                        case 5:
                            word.copy(paper.updated_at, word.length());

                            break;
                        default:
                            paper.snippet = (char*) malloc(sizeof(char) * word.length() + 1);
                            word.copy(paper.snippet, word.length());

                            // cout << "line = " << line << "\n";

                            break;
                    }
                    indexAttribute++;

                    indexCharacter = indexCloseQuotes + 1;
                } else {
                    indexCharacter++;
                }
            }
            paper.print();
            i++;
            line.clear();
        }
    } else {
        cout << "Could not open the file\n";
    }

    cout << "LEU TUDO!!\n";

    return 0;
}
