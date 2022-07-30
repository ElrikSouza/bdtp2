#include "bptree-header-block.h"

#include "../buffer/buffer-utils.h"

BPTreeHeaderBlock::BPTreeHeaderBlock() {
    _buffer = Buffer(BLOCK_SIZE);
    _root_index = 1;
    _buffer.write_4byte_number(_root_index);

    _next_free_block_index = 2;
    _buffer.write_4byte_number(_next_free_block_index);

    _qt_blocks = 0;
    _buffer.write_4byte_number(_qt_blocks);
}

BPTreeHeaderBlock::BPTreeHeaderBlock(unsigned char* block_buffer) {
    _buffer = Buffer(block_buffer, BLOCK_SIZE);

    _root_index = _buffer.read_4byte_number();
    _next_free_block_index = _buffer.read_4byte_number();
    _qt_blocks = _buffer.read_4byte_number();
}

unsigned int BPTreeHeaderBlock::get_root_node_index() {
    return _root_index;
}

unsigned int BPTreeHeaderBlock::get_next_free_block_and_point_to_next() {
    unsigned int old_free_block = _next_free_block_index;

    _next_free_block_index += 1;
    _qt_blocks += 1;

    _buffer.jump_to_absolute_position(4);
    _buffer.write_4byte_number(_next_free_block_index);
    _buffer.write_4byte_number(_qt_blocks);

    return old_free_block;
}

unsigned int BPTreeHeaderBlock::get_qt_blocks() {
    return _qt_blocks;
}

void BPTreeHeaderBlock::set_new_root_index(unsigned int new_root_index) {
    _root_index = new_root_index;
    _buffer.jump_to_the_start();
    _buffer.write_4byte_number(_root_index);
}

char* BPTreeHeaderBlock::get_block_buffer() {
    return _buffer.get_buffer_bytes();
}