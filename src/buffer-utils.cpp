#include "buffer-utils.h"

#include <cstdio>
#include <iostream>

void write_2byte_number_to_buffer(char* buffer, unsigned short int number, int offset) {
    buffer[offset] = (number >> 8) & 0xff;
    buffer[offset + 1] = number & 0xff;
}

void write_4byte_number_to_buffer(char* buffer, unsigned int number, int offset) {
    buffer[offset] = (number >> 24) & 0xFF;
    buffer[offset + 1] = (number >> 16) & 0xFF;
    buffer[offset + 2] = (number >> 8) & 0xFF;
    buffer[offset + 3] = number & 0xFF;
}

void write_fixed_length_string(char* buffer, std::string word, int offset) {
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

unsigned short int read_2byte_number_from_buffer(unsigned char* bufferr, int offset) {
    unsigned short int number = 0;
    unsigned char* buffer = (unsigned char*)bufferr;

    printf("%.2x/%.2x", buffer[offset], buffer[offset + 1]);
    number += (buffer[offset] << 8);
    number += buffer[offset + 1];

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
    std::cout << "fixed len: " << length << "\n";
    return std::string(&buffer[offset], length);
}

std::string read_varchar_from_buffer(char* buffer, int offset) {
    unsigned short int length = read_2byte_number_from_buffer((unsigned char*)buffer, offset);
    return read_fixed_length_string_from_buffer(buffer, length, offset + 2);
}