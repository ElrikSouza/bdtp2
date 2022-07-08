#include <iostream>

#pragma once

void write_2byte_number_to_buffer(char* buffer, unsigned short int number, int offset);
void write_4byte_number_to_buffer(char* buffer, unsigned int number, int offset);
void write_fixed_length_string(char* buffer, std::string word, int offset);
int write_varchar_to_buffer(char* buffer, std::string varchar, int offset);

unsigned int read_4byte_number_from_buffer(unsigned char* buffer, int offset);
unsigned short int read_2byte_number_from_buffer(unsigned char* buffer, int offset);
std::string read_fixed_length_string_from_buffer(char* buffer, int length, int offset);
std::string read_varchar_from_buffer(char* buffer, int offset);