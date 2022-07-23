#include "buffer.h"

#include "buffer-utils.h"
#include "iostream"

Buffer::Buffer(int buffer_capacity) {
    _buffer_capacity = buffer_capacity;
    _internal_buffer = new char[buffer_capacity];
    _current_position = 0;

    for (int i = 0; i < _buffer_capacity; i++) {
        _internal_buffer[i] = 0;
    }
}

Buffer::Buffer(unsigned char* buffer, int buffer_capacity) {
    _buffer_capacity = buffer_capacity;
    _internal_buffer = (char*)buffer;
    _current_position = 0;
}

bool Buffer::has_ended() {
    return _current_position >= _buffer_capacity;
}

int Buffer::get_current_cursor_position() {
    return _current_position;
}

void Buffer::jump_n_bytes_from_current_position(int bytes) {
    _current_position += bytes;
}

void Buffer::jump_to_absolute_position(int position_in_bytes) {
    _current_position = position_in_bytes;
}

void Buffer::jump_to_the_start() {
    _current_position = 0;
}

void Buffer::write_2byte_number(unsigned short int number) {
    if (_current_position + 2 > _buffer_capacity) {
        std::cout << "NUMBER2 = " << number << " POSICAO ATUAL = " << _current_position << " CAPACIDADE" << _buffer_capacity << std::endl;

        std::cout << "ERRO: BUFFER OVERFLOW"
                  << "\n";
        return;
    }

    write_2byte_number_to_buffer(_internal_buffer, number, _current_position);
    _current_position += 2;
}

void Buffer::write_4byte_number(unsigned int number) {
    if (_current_position + 4 > _buffer_capacity) {
    std::cout << "NUMBER4 = " << number << " POSICAO ATUAL = " << _current_position << " CAPACIDADE" << _buffer_capacity << std::endl;
        std::cout << "ERRO: BUFFER OVERFLOW"
                  << "\n";
        return;
    }

    write_4byte_number_to_buffer(_internal_buffer, number, _current_position);
    _current_position += 4;
}

void Buffer::write_varchar(std::string varchar) {
    int string_length = varchar.length();

    if (_current_position + string_length > _buffer_capacity) {
        std::cout << "ERRO: BUFFER OVERFLOW"
                  << "\n";

        std::cout << "current pos = " << _current_position << std::endl;
        std::cout << "capacity = " << _buffer_capacity << std::endl;
        std::cout << "len = " << string_length << std::endl;
        return;
    }

    _current_position += write_varchar_to_buffer(_internal_buffer, varchar, _current_position);
}

void Buffer::write_fixed_length_string(std::string word, int length) {
    if (_current_position + length > _buffer_capacity) {
        std::cout << "ERRO: BUFFER OVERFLOW"
                  << "\n";
        return;
    }

    write_fixed_length_string_to_buffer(_internal_buffer, word, _current_position);
    _current_position += length;
}

void Buffer::write_bytes(char* bytes, int num_of_bytes) {
    for (int i = 0; i < num_of_bytes; i++) {
        _internal_buffer[_current_position + i] = bytes[i];
    }

    _current_position += num_of_bytes;
}

unsigned short int Buffer::read_2byte_number() {
    unsigned short int result = read_2byte_number_from_buffer((unsigned char*)_internal_buffer, _current_position);
    _current_position += 2;

    return result;
}

unsigned int Buffer::read_4byte_number() {
    unsigned int result = read_4byte_number_from_buffer((unsigned char*)_internal_buffer, _current_position);
    _current_position += 4;

    return result;
}

std::string Buffer::read_varchar() {
    unsigned short int length = read_2byte_number();
    std::string result = read_fixed_length_string_from_buffer(_internal_buffer, length, _current_position);
    _current_position += length;

    return result;
}

std::string Buffer::read_fixed_length_string(int length) {
    std::string result = read_fixed_length_string_from_buffer(_internal_buffer, length, _current_position);
    _current_position += length;

    return result;
}

char* Buffer::get_buffer_bytes() {
    return _internal_buffer;
}

void Buffer::free() {
    if (!already_free) {
        already_free = true;
        delete[] _internal_buffer;
    }
}