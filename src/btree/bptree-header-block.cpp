#include "bptree-header-block.h"
#include "bptree-root-block.h"

#include "../buffer/buffer-utils.h"

BPTreeHeaderBlock::BPTreeHeaderBlock() {
    _buffer = Buffer(4096);
    _root_index = 0;
    _buffer.write_4byte_number(_root_index);
    _next_free_block_index = 0;
    _buffer.write_4byte_number(_next_free_block_index);
    _qt_blocks = 0;
    _buffer.write_4byte_number(_qt_blocks);
}

BPTreeHeaderBlock::BPTreeHeaderBlock(unsigned char* block_buffer) {
    _buffer = Buffer(block_buffer, 4096);
    _root_index = read_4byte_number_from_buffer(block_buffer, 0);
    _next_free_block_index = read_4byte_number_from_buffer(block_buffer, 4);
    _qt_blocks = read_4byte_number_from_buffer(block_buffer, 8);
}

unsigned int BPTreeHeaderBlock::get_root_node_index() {
    return _root_index;
}

unsigned int BPTreeHeaderBlock::get_next_free_block_and_point_to_next() {
    unsigned int old_free_block = _next_free_block_index;

    _next_free_block_index += 4096;
    write_4byte_number_to_buffer(_buffer.get_buffer_bytes(), _next_free_block_index, 4);

    return old_free_block;
}

void BPTreeHeaderBlock::set_new_root_index(unsigned int new_root_index) {
    _root_index = new_root_index;
}

char* BPTreeHeaderBlock::get_block_buffer() {
    return _buffer.get_buffer_bytes();
}