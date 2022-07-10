#include <iostream>

#pragma once

class Buffer {
   private:
    char* _internal_buffer;
    int _buffer_capacity;
    int _current_position;

   public:
    Buffer(int buffer_capacity);
    Buffer(unsigned char* buffer, int buffer_length);

    bool has_ended();
    int get_current_cursor_position();

    void jump_n_bytes_from_current_position(int bytes);
    void jump_to_the_start();

    void write_2byte_number(unsigned short int number);
    void write_4byte_number(unsigned int number);
    void write_fixed_length_string(std::string word, int length);
    void write_varchar(std::string varchar);

    void write_bytes(char* bytes, int num_of_bytes);

    unsigned short int read_2byte_number();
    unsigned int read_4byte_number();
    std::string read_varchar();
    std::string read_fixed_length_string(int length);

    char* get_buffer_bytes();
};