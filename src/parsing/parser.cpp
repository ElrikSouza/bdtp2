#include "parser.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "../paper.h"
#include "parser.h"

using namespace std;

int get_first_character_occurance_index(string str, char character, int offset) {
    return str.find_first_of(character, offset);
}

void assign_attribute_to_paper(string word, int indexAttribute, Paper* paper) {
    switch (indexAttribute) {
        case 0:
            paper->id = std::stoi(word);

            break;
        case 1:
            paper->title = word;
            break;
        case 2:
            paper->year = std::stoi(word);

            break;
        case 3:
            paper->authors = word;
            break;
        case 4:
            paper->citations = std::stoi(word);

            break;
        case 5:
            paper->updated_at = word;

            break;
        default:
            paper->snippet = word;
            break;
    }
}

Paper* read_csv_line(fstream* file, string* line) {
    int indexAttribute = 0;
    int indexCharacter = 0;
    Paper* paper = new Paper;

    while (indexCharacter < line->length() || indexAttribute <= INDEX_LAST_ATTRIBUTE) {
        string word;

        // se o campo for vazio, i.e. ';;' ou nulo, i.e. NULL
        // mapeamos o valor para uma string vazia
        if (((*line)[indexCharacter] == ';' && (*line)[indexCharacter + 1] == ';') ||
            ((*line)[indexCharacter] == 'N')) {
            word = '\0';

            assign_attribute_to_paper(word, indexAttribute, paper);

            indexAttribute++;
            indexCharacter++;
        }

        // se o campo nao for vazio
        if ((*line)[indexCharacter] == '"') {
            int indexCloseQuotes = get_first_character_occurance_index((*line), '"', indexCharacter + 1);
            int indexStartData = indexCharacter + 1;

            // se, em uma mesma linha, nao encontrar as aspas que fecham os dados do campo
            // quer dizer que ha uma quebra de linha no dado do campo
            // entao precisamos continuar na proxima linha
            if (indexCloseQuotes == -1) {
                word = line->substr(indexStartData, line->length());
                getline((*file), (*line));

                indexCharacter = 0;

                indexCloseQuotes = get_first_character_occurance_index((*line), '"', indexCharacter + 1);

                word.append(line->substr(0, indexCloseQuotes));
            }

            // se o valor do campo possui aspas, precisamos encontrar as aspas 'verdadeiras' que fecham
            // ou seja, uma aspa que nao possui logo em seguida uma outra aspa
            while ((*line)[indexCloseQuotes + 1] == '"') {
                indexCloseQuotes = get_first_character_occurance_index((*line), '"', indexCloseQuotes + 2);
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

            assign_attribute_to_paper(word, indexAttribute, paper);

            indexAttribute++;
            indexCharacter = indexCloseQuotes + 1;

        } else {
            indexCharacter++;
        }
    }

    return paper;
}
