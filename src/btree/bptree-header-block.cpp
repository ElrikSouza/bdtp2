#include "bptree-header-block.h"
#include "buffer/buffer-utils.h"

BPTreeHeaderBlock::BPTreeHeaderBlock() {
    _root_index = 0;
    _next_free_block_index = 4096;
    _buffer = Buffer(4096);
}

BPTreeHeaderBlock::BPTreeHeaderBlock(unsigned char* block_buffer) {
    _root_index = read_4byte_number_from_buffer(block_buffer, 0);
    _next_free_block_index = read_4byte_number_from_buffer(block_buffer, 4);
    _buffer = Buffer(block_buffer, 4096);
}

unsigned int BPTreeHeaderBlock::get_root_node_index() {
    return _root_index;
}

unsigned int BPTreeHeaderBlock::get_next_free_block_and_point_to_next() {
    unsigned int old_free_block = _next_free_block_index;

    _next_free_block_index += 4096;

    return old_free_block;
}

void BPTreeHeaderBlock::set_new_root_index(unsigned int new_root_index) {
    _root_index = new_root_index;
}

char* BPTreeHeaderBlock::get_block_buffer() {
    return _buffer.get_buffer_bytes();
}