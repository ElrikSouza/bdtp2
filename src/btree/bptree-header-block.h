#include "../buffer/buffer.h"
#include "../config.h"

#pragma once

class BPTreeHeaderBlock {
   private:
    unsigned int _root_index;
    unsigned int _next_free_block_index;
    unsigned int _qt_blocks;
    Buffer _buffer{nullptr, 0};

   public:
    BPTreeHeaderBlock();
    BPTreeHeaderBlock(unsigned char* block_buffer);

    unsigned int get_root_node_index();
    unsigned int get_next_free_block_and_point_to_next();
    void set_new_root_index(unsigned int new_root_index);
    char* get_block_buffer();
};