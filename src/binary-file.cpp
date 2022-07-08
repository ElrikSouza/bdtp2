#include <cstdio>
#include <fstream>
#include <iostream>

#include "paper.h"

/**
 * Esta é uma classe para abstrair a leitura e escrita de blocos de um tamanho X em um arquivo binario qualquer.
 */
class BinaryFile {
   private:
    std::fstream _file_stream;
    int _num_of_blocks;
    int _block_size;

   public:
    BinaryFile() {}
    BinaryFile(int num_of_blocks, int block_size) : _num_of_blocks(num_of_blocks), _block_size(block_size) {}

    void create_or_rewrite(const char* filename) {
        _file_stream.open(filename, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
    }

    void open_as_readonly(const char* filename) {
        _file_stream.open(filename, std::ios::in | std::ios::binary);
    }

    void close() {
        _file_stream.close();
    }

    void zero_the_file_out() {
        char* zeros = new char[_block_size]{0};

        for (int i = 0; i < _num_of_blocks; i++) {
            _file_stream.write(zeros, _block_size);
        }

        delete[] zeros;
    }

    void write_block(const char* block_buffer, int block_index) {
        int actual_position = _block_size * block_index;
        _file_stream.seekp(actual_position, std::ios::beg);
        _file_stream.write(block_buffer, _block_size);
    }

    unsigned char* read_block(int block_index) {
        unsigned char* block_buffer = new unsigned char[_block_size];

        int actual_position = _block_size * block_index;
        _file_stream.seekg(actual_position, std::ios::beg);
        _file_stream.read((char*)block_buffer, _block_size);

        return block_buffer;
    }
};

// int main() {
//     auto a = BinaryFile(2, 4096);
//     Paper paper;
//     paper.id = 4150;
//     paper.citations = 8989;
//     paper.authors = std::string("abcd");
//     paper.snippet = std::string("1234");
//     paper.updated_at = std::string("2020-03-04 23:45:21");
//     paper.year = 2020;
//     paper.title = std::string("xyz");
//     paper.authors = std::string("first");
//     paper.print();

//     a.create_or_rewrite("test.bin");
//     a.zero_the_file_out();
//     a.write_block(paper.serialize(), 0);
//     unsigned char* read = a.read_block(0);

//     Paper r;
//     r.deserialize(read, 0);
//     r.print();

//     std::cout << "done";

//     return 0;
// }