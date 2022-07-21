#include "../buffer/buffer.h"

#pragma once

class BPTreeRootBlock {
   private:
    unsigned int _key_value;
    unsigned int _left_subtree_block_index;
    unsigned int _right_subtree_block_index;
    Buffer _buffer{nullptr, 0};

   public:
    BPTreeRootBlock();

    void insert_key(unsigned int key, unsigned int lesser_than, unsigned int greater_than);
    char* get_block_buffer();
    void free();
};