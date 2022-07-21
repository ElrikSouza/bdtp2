#include "bptree-root-block.h"

BPTreeRootBlock::BPTreeRootBlock() {
    _key_value = 0;
    _left_subtree_block_index = 0;
    _right_subtree_block_index = 0;
    _buffer = Buffer(4096);
}

void BPTreeRootBlock::insert_key(unsigned int key, unsigned int lesser_than, unsigned int greater_than) {
    // header
    _buffer.write_4byte_number(0);
    _buffer.write_4byte_number(0);
    _buffer.write_4byte_number(0);

    _buffer.write_4byte_number(key);
    _buffer.write_4byte_number(lesser_than);

    _buffer.jump_to_the_start();
    _buffer.jump_n_bytes_from_current_position(4092);
    _buffer.write_4byte_number(greater_than);
}

char* BPTreeRootBlock::get_block_buffer() {
    return _buffer.get_buffer_bytes();
}
void BPTreeRootBlock::free() {
    _buffer.free();
}