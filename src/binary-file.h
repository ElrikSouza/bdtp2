#include <cstdio>
#include <fstream>
#include <iostream>

#include "paper-block.h"
#include "paper.h"

#pragma once

/**
 * Esta é uma classe para abstrair a leitura e escrita de blocos de um tamanho X em um arquivo binario qualquer.
 */
class BinaryFile {
   private:
    std::fstream _file_stream;
    int _num_of_blocks;
    int _block_size;

   public:
    BinaryFile();
    BinaryFile(int num_of_blocks, int block_size);

    void create_or_rewrite(const char* filename);

    void open_as_readonly(const char* filename);

    void close();

    void zero_the_file_out();

    void write_block(const char* block_buffer, int block_index);

    unsigned char* read_block(int block_index);
};
