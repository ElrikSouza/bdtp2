#include "binary-file.h"

#include <cstdio>
#include <fstream>
#include <iostream>

#include "paper-block.h"
#include "paper.h"

BinaryFile::BinaryFile() {}
BinaryFile::BinaryFile(int num_of_blocks, int block_size) : _num_of_blocks(num_of_blocks), _block_size(block_size) {}

void BinaryFile::create_or_rewrite(const char* filename) {
    _file_stream.open(filename, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
}

void BinaryFile::open_as_readonly(const char* filename) {
    _file_stream.open(filename, std::ios::in | std::ios::binary);
}

void BinaryFile::close() {
    _file_stream.close();
}

void BinaryFile::zero_the_file_out() {
    char* zeros = new char[_block_size]{0};

    for (int i = 0; i < _num_of_blocks; i++) {
        _file_stream.write(zeros, _block_size);
    }

    delete[] zeros;
}

void BinaryFile::write_block(const char* block_buffer, int block_index) {
    int actual_position = _block_size * block_index;
    _file_stream.seekp(actual_position, std::ios::beg);
    _file_stream.write(block_buffer, _block_size);
}

unsigned char* BinaryFile::read_block(int block_index) {
    unsigned char* block_buffer = new unsigned char[_block_size];

    int actual_position = _block_size * block_index;
    _file_stream.seekg(actual_position, std::ios::beg);
    _file_stream.read((char*)block_buffer, _block_size);

    return block_buffer;
}