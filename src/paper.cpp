#include <iostream>

#pragma once

struct Paper {
    unsigned int id;
    unsigned short int year;
    char* title;
    char* snippet;
    char* authors;
    unsigned int citations;
    char updated_at[20];

    void print() {
        std::cout << "id: " << id << "\n"
                  << "titulo: " << title << "\n"
                  << "snippet: " << snippet << "\n"
                  << "autores: " << authors << "\n"
                  << "citacoes: " << citations << "\n"
                  << "atualizacao: " << updated_at << "\n"
                  << "\n";
    }
};