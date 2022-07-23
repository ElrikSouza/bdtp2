#include "./auxiliar-block.h"
#include "../buffer/buffer-utils.h"

AuxiliarBlock::AuxiliarBlock(int block_size) {
    _block_size = block_size;
    _buffer = Buffer(block_size);
}

AuxiliarBlock::AuxiliarBlock(char* bytes, int block_size) {
    _buffer = Buffer(block_size);
    _buffer.write_bytes(bytes, 4096);
}

void AuxiliarBlock::copy_all_key_pointer(char* bytes, int num_of_bytes) {
    _buffer.jump_to_the_start();
    unsigned short int aa = read_2byte_number_from_buffer((unsigned char*) bytes, 0);
    unsigned short int bb = read_2byte_number_from_buffer((unsigned char*) bytes, 2);
    unsigned int cc = read_4byte_number_from_buffer((unsigned char*) bytes, 12);
    unsigned int dd = read_4byte_number_from_buffer((unsigned char*) bytes, 16);
    std::cout << "aa = " << aa << " bb = " << bb << " cc = " << cc << " dd = " << dd << std::endl;

    _buffer.write_bytes(bytes + 12, num_of_bytes);
}

void AuxiliarBlock::copy_first_half_of_key_pointer_to_buffer(char* bytes) {
    char* buffer_bytes = _buffer.get_buffer_bytes();

    for (int i = 0; i < _block_size / 2; i++) {
        bytes[i + 12] = buffer_bytes[i];
    }

    write_2byte_number_to_buffer(bytes, 255, 2);
}

void AuxiliarBlock::copy_second_half_of_key_pointer_to_buffer(char* bytes) {
    char* buffer_bytes = _buffer.get_buffer_bytes();

    for (int i = _block_size / 2; i < _block_size; i++) {
        bytes[i + 12 + 4096 / 2] = buffer_bytes[i];
    }

    write_2byte_number_to_buffer(bytes, 256, 2);

}

void AuxiliarBlock::insert_key_pointer(unsigned int key, unsigned int block_index) {
    unsigned int current_index = read_4byte_number_from_buffer((unsigned char*) _buffer.get_buffer_bytes(), 0);
    unsigned int current_key = read_4byte_number_from_buffer((unsigned char*) _buffer.get_buffer_bytes(), 4);

    // std::cout << "CURRENT INDEX = " << current_index << " CURRENT KEY = " << current_key << std::endl;

    int i = 8;
    while (key > current_key) {
        current_index = read_4byte_number_from_buffer((unsigned char*) _buffer.get_buffer_bytes(), i);
        current_key = read_4byte_number_from_buffer((unsigned char*) _buffer.get_buffer_bytes(), i + 4);

        i += 8;
    }
    
    unsigned int aux_index = current_index;
    unsigned int aux_key = current_key;

    write_4byte_number_to_buffer(_buffer.get_buffer_bytes(), block_index, i);
    write_4byte_number_to_buffer(_buffer.get_buffer_bytes(), key, i + 4);

    i += 8;

    while (i < _block_size) {
        current_index = read_4byte_number_from_buffer((unsigned char*) _buffer.get_buffer_bytes(), i);
        current_key = read_4byte_number_from_buffer((unsigned char*) _buffer.get_buffer_bytes(), i + 4);

        write_4byte_number_to_buffer(_buffer.get_buffer_bytes(), aux_index, i);
        write_4byte_number_to_buffer(_buffer.get_buffer_bytes(), aux_key, i + 4);

        aux_index = current_index;
        aux_key = current_key;
        
        i += 8;
    }
}

char* AuxiliarBlock::get_block_buffer() {
    return _buffer.get_buffer_bytes();
}

BPTreeLeafBlock* AuxiliarBlock::deserialize_into_leaf_block() {
    BPTreeLeafBlock* leaf = new BPTreeLeafBlock((unsigned char*) _buffer.get_buffer_bytes(), 4096);

    return leaf;
}

BPTreeInternalBlock* AuxiliarBlock::deserialize_into_internal_block() {
    BPTreeInternalBlock* internal = new BPTreeInternalBlock((unsigned char*) _buffer.get_buffer_bytes(), 4096);

    return internal;
}
