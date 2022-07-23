#include "../buffer/buffer.h"

#pragma once

class BPTreeInternalBlock {
   private:
    unsigned short int _is_leaf;
    unsigned short int _qt_keys;
    unsigned int _first_key_value;
    unsigned int _first_subtree_block_index;
    unsigned int _last_subtree_block_index;
    Buffer _buffer{nullptr, 0};

    void _jump_header_bytes();

   public:
    BPTreeInternalBlock(int block_size);
    BPTreeInternalBlock(unsigned char* block_buffer, int block_size);

    void insert_key_for_root(unsigned int old_root_index, unsigned int key, unsigned int new_node);
    void transfer_first_half_of_keys_and_pointers(BPTreeInternalBlock* internal);
    void transfer_second_half_of_keys_and_pointers(BPTreeInternalBlock* internal);

    void insert_key(unsigned int key, unsigned int node_block_index);
    unsigned int get_middle_key();
    unsigned int transfer_data_and_pointers_to_split_node(BPTreeInternalBlock* split_node,
                                                          unsigned int split_node_block_index,
                                                          unsigned int overflow_key);

    unsigned int get_matching_pointer(unsigned int key);
    bool are_there_free_slots();

    char* get_block_buffer();
    unsigned int get_first_key();
    void free();
};