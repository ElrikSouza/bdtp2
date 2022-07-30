#include "auxiliar-block.h"
#include "../buffer/buffer-utils.h"

AuxiliarBlock::AuxiliarBlock(int block_size) {
    _block_size = block_size;
    _buffer = Buffer(block_size);
}

AuxiliarBlock::AuxiliarBlock(char* bytes, int block_size) {
    _buffer = Buffer(block_size);
    _block_size = block_size;
    _buffer.write_bytes(bytes, BLOCK_SIZE);
}

void AuxiliarBlock::copy_all_key_pointer(char* node_buffer, int num_of_bytes) {
    _buffer.jump_to_the_start();

    _buffer.write_bytes(node_buffer + BPTREE_HEADER_SIZE, num_of_bytes - BPTREE_HEADER_SIZE);
    _buffer.jump_to_the_start();
}

void AuxiliarBlock::leaf_copy_first_half_of_key_pointer_to_buffer(char* leaf_node_buffer) {
    char* buffer_bytes = _buffer.get_buffer_bytes();

    for (int i = 0; i < BPTREE_MIN_KEYS_PER_BLOCK; i++) {
        unsigned int current_index = read_4byte_number_from_buffer((unsigned char*) buffer_bytes, i * BPTREE_INDEX_VALUE_PAIR_SIZE);
        unsigned int current_key = read_4byte_number_from_buffer((unsigned char*) buffer_bytes, i * BPTREE_INDEX_VALUE_PAIR_SIZE + 4);

        write_4byte_number_to_buffer(leaf_node_buffer, current_index, (i * BPTREE_INDEX_VALUE_PAIR_SIZE) + BPTREE_HEADER_SIZE);
        write_4byte_number_to_buffer(leaf_node_buffer, current_key, (i * BPTREE_INDEX_VALUE_PAIR_SIZE) + BPTREE_HEADER_SIZE + 4);
    }

    write_2byte_number_to_buffer(leaf_node_buffer, BPTREE_MIN_KEYS_PER_BLOCK, 2);
}

void AuxiliarBlock::leaf_copy_second_half_of_key_pointer_to_buffer(char* leaf_node_buffer) {
    char* buffer_bytes = _buffer.get_buffer_bytes();

    for (int i = 0; i <= BPTREE_MIN_KEYS_PER_BLOCK; i++) {
        unsigned int current_index = read_4byte_number_from_buffer((unsigned char*) buffer_bytes, (i + BPTREE_MIN_KEYS_PER_BLOCK) * BPTREE_INDEX_VALUE_PAIR_SIZE);
        unsigned int current_key = read_4byte_number_from_buffer((unsigned char*) buffer_bytes, (i + BPTREE_MIN_KEYS_PER_BLOCK) * BPTREE_INDEX_VALUE_PAIR_SIZE + 4);

        write_4byte_number_to_buffer(leaf_node_buffer, current_index, (i * BPTREE_INDEX_VALUE_PAIR_SIZE) + BPTREE_HEADER_SIZE);
        write_4byte_number_to_buffer(leaf_node_buffer, current_key, (i * BPTREE_INDEX_VALUE_PAIR_SIZE) + BPTREE_HEADER_SIZE + 4);
    }

    write_2byte_number_to_buffer(leaf_node_buffer, BPTREE_MIN_KEYS_PER_BLOCK + 1, 2);
}

void AuxiliarBlock::insert_leaf_key_pointer(unsigned int key, unsigned int block_index) {
    _buffer.jump_to_the_start();
    unsigned int current_index = read_4byte_number_from_buffer((unsigned char*) _buffer.get_buffer_bytes(), 0);
    unsigned int current_key = read_4byte_number_from_buffer((unsigned char*) _buffer.get_buffer_bytes(), 4);

    current_index = _buffer.read_4byte_number();
    current_key = _buffer.read_4byte_number();

    while (key > current_key && (_buffer.get_current_cursor_position() + 4 < _block_size)) {
        current_index = _buffer.read_4byte_number();
        current_key = _buffer.read_4byte_number();
    }
    unsigned int current_position = _buffer.get_current_cursor_position();
    _buffer.jump_to_absolute_position(current_position - BPTREE_INDEX_VALUE_PAIR_SIZE);
    
    unsigned int aux_index = current_index;
    unsigned int aux_key = current_key;

    _buffer.write_4byte_number(block_index);
    _buffer.write_4byte_number(key);

    // nao eh necessario alterar o ptr da lista encadeada
    while (_buffer.get_current_cursor_position() < _block_size - 4) {
        current_index = _buffer.read_4byte_number();
        current_key = _buffer.read_4byte_number();

        _buffer.write_4byte_number(aux_index);
        _buffer.write_4byte_number(aux_key);

        aux_index = current_index;
        aux_key = current_key;
    }
}

void AuxiliarBlock::internal_copy_first_half_of_key_pointer_to_buffer(char* internal_node_buffer) {
    char* buffer_bytes = _buffer.get_buffer_bytes();

    for (int i = 0; i < BPTREE_MIN_KEYS_PER_BLOCK; i++) {
        unsigned int current_index = read_4byte_number_from_buffer((unsigned char*) buffer_bytes, i * BPTREE_INDEX_VALUE_PAIR_SIZE);
        unsigned int current_key = read_4byte_number_from_buffer((unsigned char*) buffer_bytes, i * BPTREE_INDEX_VALUE_PAIR_SIZE + 4);

        write_4byte_number_to_buffer(internal_node_buffer, current_index, (i * BPTREE_INDEX_VALUE_PAIR_SIZE) + BPTREE_HEADER_SIZE);
        write_4byte_number_to_buffer(internal_node_buffer, current_key, (i * BPTREE_INDEX_VALUE_PAIR_SIZE) + BPTREE_HEADER_SIZE + 4);
    }

    write_2byte_number_to_buffer(internal_node_buffer, BPTREE_MIN_KEYS_PER_BLOCK, 2);
}

void AuxiliarBlock::internal_copy_second_half_of_key_pointer_to_buffer(char* internal_node_buffer) {
    char* buffer_bytes = _buffer.get_buffer_bytes();

    for (int i = 0; i <= BPTREE_MIN_KEYS_PER_BLOCK; i++) {
        unsigned int current_index = read_4byte_number_from_buffer((unsigned char*) buffer_bytes, (i + BPTREE_MIN_KEYS_PER_BLOCK) * BPTREE_INDEX_VALUE_PAIR_SIZE);
        unsigned int current_key = read_4byte_number_from_buffer((unsigned char*) buffer_bytes, (i + BPTREE_MIN_KEYS_PER_BLOCK) * BPTREE_INDEX_VALUE_PAIR_SIZE + 4);

        write_4byte_number_to_buffer(internal_node_buffer, current_index, (i * BPTREE_INDEX_VALUE_PAIR_SIZE) + BPTREE_HEADER_SIZE);
        write_4byte_number_to_buffer(internal_node_buffer, current_key, (i * BPTREE_INDEX_VALUE_PAIR_SIZE) + BPTREE_HEADER_SIZE + 4);
    }

    unsigned int last_subtree_index = read_4byte_number_from_buffer((unsigned char*) buffer_bytes, 4088);
    write_2byte_number_to_buffer(internal_node_buffer, BPTREE_MIN_KEYS_PER_BLOCK + 1, 2);
    write_4byte_number_to_buffer(internal_node_buffer, last_subtree_index, BPTREE_LAST_INDEX);
}

void AuxiliarBlock::insert_internal_key_pointer(unsigned int key, unsigned int block_index) {
    _buffer.jump_to_absolute_position(_block_size - 4);
    unsigned int last_subtree_index = _buffer.read_4byte_number();

    _buffer.jump_to_the_start();
    unsigned int current_index = _buffer.read_4byte_number();
    unsigned int current_key = _buffer.read_4byte_number();

    while (key > current_key && (_buffer.get_current_cursor_position() + 4 < _block_size)) {
        current_index = _buffer.read_4byte_number();
        current_key = _buffer.read_4byte_number();
    }

    unsigned int current_position = _buffer.get_current_cursor_position();
    _buffer.jump_to_absolute_position(current_position - BPTREE_INDEX_VALUE_PAIR_SIZE);

    unsigned int aux_index = current_index;
    unsigned int aux_key = current_key;

    _buffer.write_4byte_number(last_subtree_index);
    _buffer.write_4byte_number(key);

    write_4byte_number_to_buffer(_buffer.get_buffer_bytes(), block_index, _block_size - 4);

    while (_buffer.get_current_cursor_position() < _block_size - 4) {
        current_index = _buffer.read_4byte_number();
        current_key = _buffer.read_4byte_number();

        _buffer.write_4byte_number(aux_index);
        _buffer.write_4byte_number(aux_key);

        aux_index = current_index;
        aux_key = current_key;
    }
}

char* AuxiliarBlock::get_block_buffer() {
    return _buffer.get_buffer_bytes();
}