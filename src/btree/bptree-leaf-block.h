#include "../buffer/buffer.h"

#pragma once

class BPTreeLeafBlock {
   private:
    unsigned short int _is_leaf;
    unsigned short int _qt_keys;
    unsigned int _first_key_value;
    unsigned int _first_block_index;
    unsigned int _next_block;
    Buffer _buffer{nullptr, 0};

    void _jump_header_bytes();

   public:
    BPTreeLeafBlock();
    BPTreeLeafBlock(unsigned char* block_buffer);
    unsigned int get_middle_key();
    void transfer_data_and_pointers_to_split_node(BPTreeLeafBlock* split_node, unsigned int split_node_block_index);
    bool are_there_free_slots();
    void insert_key(unsigned int key, unsigned int data_file_block_index);

    char* get_block_buffer();
};