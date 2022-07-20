#include "../buffer/buffer.h"

#pragma once

class BPTreeInternalBlock {
   private:
    unsigned short int _is_leaf;
    unsigned short int _qt_keys;
    unsigned int _first_key_value;
    unsigned int _first_subtree_block_index;
    Buffer _buffer{nullptr, 0};

   public:
    BPTreeInternalBlock();
    BPTreeInternalBlock(unsigned char* block_buffer);

    void insert_key(unsigned int key, unsigned int split_node_block_index);
    unsigned int get_middle_key();
    void transfer_data_and_pointers_to_split_node(BPTreeInternalBlock* split_node, unsigned int split_node_block_index);

    unsigned int get_matching_pointer(unsigned int key);
    bool are_there_free_slots();

    char* get_block_buffer();
};