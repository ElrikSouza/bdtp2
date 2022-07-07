#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <sstream>
#include <string>
#include "paper.cpp"
#include "utils.cpp"

#define INDEX_LAST_ATTRIBUTE 6

using namespace std;

void applyAttribute(string word, int indexAttribute, Paper *paper) {
    switch(indexAttribute) {
        case 0:
            paper->id = std::stoi(word);

            break;
        case 1:
            paper->title = (char*) malloc(sizeof(char) * word.length() + 1);
            word.copy(paper->title, word.length() + 1);

            break;
        case 2:
            paper->year = std::stoi(word);

            break;
        case 3:
            paper->authors = (char*) malloc(sizeof(char) * word.length() + 1);
            word.copy(paper->authors, word.length() + 1);

            break;
        case 4:
            paper->citations = std::stoi(word);

            break;
        case 5:
            word.copy(paper->updated_at, word.length() + 1);

            break;
        default:
            paper->snippet = (char*) malloc(sizeof(char) * word.length() + 1);
            word.copy(paper->snippet, word.length());

            break;
    }
}


Paper readRegistry(fstream* file, string* line) {
    int indexAttribute = 0;
    int indexCharacter = 0;
    Paper paper;

    while (indexCharacter < line->length() || indexAttribute <= INDEX_LAST_ATTRIBUTE) {
        string word;

        // se o campo for vazio, i.e. ';;' ou nulo, i.e. NULL
        // mapeamos o valor para uma string vazia
        if (((*line)[indexCharacter] == ';' && (*line)[indexCharacter + 1] == ';') || ((*line)[indexCharacter] == 'N')) {
            word = '\0';

            applyAttribute(word, indexAttribute, &paper);

            indexAttribute++;
            indexCharacter++;
        }

        // se o campo nao for vazio
        if ((*line)[indexCharacter] == '"') {
            int indexCloseQuotes = indexOfCharacterFirstOcurrance((*line), '"', indexCharacter + 1);
            int indexStartData = indexCharacter + 1;

            // se, em uma mesma linha, nao encontrar as aspas que fecham os dados do campo
            // quer dizer que ha uma quebra de linha no dado do campo
            // entao precisamos continuar na proxima linha
            if (indexCloseQuotes == -1) {
                word = line->substr(indexStartData, line->length());
                getline((*file), (*line));

                indexCharacter = 0;

                indexCloseQuotes = indexOfCharacterFirstOcurrance((*line), '"', indexCharacter + 1);

                word.append(line->substr(0, indexCloseQuotes));

            }

            // se o valor do campo possui aspas, precisamos encontrar as aspas 'verdadeiras' que fecham
            // ou seja, uma aspa que nao possui logo em seguida uma outra aspa
            while((*line)[indexCloseQuotes + 1] == '"') {
                indexCloseQuotes = indexOfCharacterFirstOcurrance((*line), '"', indexCloseQuotes + 2);
            }

            word = line->substr(indexStartData, indexCloseQuotes - indexStartData);

            // remove os caracteres invalidos da string
            int j = 0;
            for (int i = 0; i < word.length(); i++) {
                if (word[i] >= 0 && word[i] < 128) {
                    word[j] = word[i];
                    j++;
                }
            }
            word[j] = '\0';

            applyAttribute(word, indexAttribute, &paper);

            indexAttribute++;
            indexCharacter = indexCloseQuotes + 1;

        } else {
            indexCharacter++;
        }
    }

    return paper;
}

void readCSVFile(string filename) {
    string line;

    fstream file(filename, ios::in);
    cout << "Iniciando leitura do arquivo...\n";
    Paper paper;
    if (file.is_open()) {
        while (getline(file, line)) {
            paper = readRegistry(&file, &line);
            paper.print();
        }
    } else {
        cout << "O arquivo nao existe!\n";
    }

    cout << "Arquivo lido com sucesso.\n";
}

int main() {
    readCSVFile("artigo.csv");

    return 0;
}
