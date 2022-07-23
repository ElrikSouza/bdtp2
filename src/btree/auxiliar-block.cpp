#include "./auxiliar-block.h"

AuxiliarBlock::AuxiliarBlock(char* bytes, int block_size) {
    _buffer = Buffer(block_size);
    _buffer.write_bytes(bytes, 4096);
}

BPTreeLeafBlock* AuxiliarBlock::deserialize_into_leaf_block() {
    BPTreeLeafBlock* leaf = new BPTreeLeafBlock((unsigned char*) _buffer.get_buffer_bytes(), 4096);

    return leaf;
}

BPTreeInternalBlock* AuxiliarBlock::deserialize_into_internal_block() {
    BPTreeInternalBlock* internal = new BPTreeInternalBlock((unsigned char*) _buffer.get_buffer_bytes(), 4096);

    return internal;
}
