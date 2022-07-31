#include "../buffer/buffer.h"
#include "../config.h"

#pragma once

class BPTreeLeafBlock {
   private:
    unsigned short int _is_leaf;
    unsigned short int _qt_keys;
    unsigned int _first_block_index;
    unsigned int _first_key_value;
    unsigned int _next_block;
    Buffer _buffer{nullptr, 0};

   public:
    BPTreeLeafBlock();
    BPTreeLeafBlock(unsigned char* block_buffer);

    unsigned int get_first_key();
    bool are_there_free_slots();
    void insert_key(unsigned int key, unsigned int data_file_block_index);
    unsigned int point_to_new_block(unsigned int block_index);
    char* get_block_buffer();
    void free();
};