#include "buffer-utils.h"

#include <cstdio>
#include <iostream>

void write_2byte_number_to_buffer(char* buffer, unsigned short int number, int offset) {
    // fatia um unsigned short int em dois chars
    buffer[offset] = (number >> 8) & 0xff;
    buffer[offset + 1] = number & 0xff;
}

void write_4byte_number_to_buffer(char* buffer, unsigned int number, int offset) {
    // fatia um unsigned int em 4 chars
    buffer[offset] = (number >> 24) & 0xFF;
    buffer[offset + 1] = (number >> 16) & 0xFF;
    buffer[offset + 2] = (number >> 8) & 0xFF;
    buffer[offset + 3] = number & 0xFF;
}

void write_fixed_length_string_to_buffer(char* buffer, std::string word, int offset) {
    for (int i = 0; i < word.length(); i++) {
        buffer[offset + i] = word[i];
    }
}

int write_varchar_to_buffer(char* buffer, std::string varchar, int offset) {
    unsigned short int length = varchar.length();
    write_2byte_number_to_buffer(buffer, length, offset);

    offset += 2;

    for (int i = 0; i < varchar.length(); i++) {
        buffer[offset + i] = varchar[i];
    }

    return length + 2;
}

unsigned short int read_2byte_number_from_buffer(unsigned char* buffer, int offset) {
    unsigned short int number = 0;

    // tem que transformar em unsigned para usar o shift left sem destruir o número original
    unsigned char* unsigned_buffer = (unsigned char*)buffer;

    number += (unsigned_buffer[offset] << 8);
    number += unsigned_buffer[offset + 1];

    return number;
}

unsigned int read_4byte_number_from_buffer(unsigned char* buffer, int offset) {
    unsigned int number = 0;

    for (int i = 0; i < 4; i++) {
        number += buffer[offset + i] << ((3 - i) * 8);
    }

    return number;
}

std::string read_fixed_length_string_from_buffer(char* buffer, int length, int offset) {
    return std::string(&buffer[offset], length);
}

std::string read_varchar_from_buffer(char* buffer, int offset) {
    unsigned short int length = read_2byte_number_from_buffer((unsigned char*)buffer, offset);
    return read_fixed_length_string_from_buffer(buffer, length, offset + 2);
}