#include <iostream>
#include <string>

#include "buffer-utils.h"

#pragma once

#define LENGTH_HEADERS 6
#define FIXED_LENGTH 30
#define REGISTER_HEADER 2

class Paper {
   public:
    unsigned int id;
    unsigned short int year;
    std::string title;
    std::string snippet;
    std::string authors;
    unsigned int citations;
    std::string updated_at;

    void print();

    unsigned short int get_register_size();

    const char* serialize();

    void deserialize(unsigned char* buffer, int buffer_offset);
};