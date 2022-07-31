#include "bptree-leaf-block.h"

#include "../buffer/buffer-utils.h"

BPTreeLeafBlock::BPTreeLeafBlock() {
    _buffer = Buffer(BLOCK_SIZE);

    // header data
    _is_leaf = 1;
    _buffer.write_2byte_number(_is_leaf);
    _qt_keys = 0;
    _buffer.write_2byte_number(_qt_keys);

    _buffer.jump_to_absolute_position(BPTREE_HEADER_SIZE);

    // <ptr,key> pair
    _first_block_index = 0;
    _buffer.write_4byte_number(_first_block_index);
    _first_key_value = 0;
    _buffer.write_4byte_number(_first_key_value);

    // ptr next block
    _buffer.jump_to_absolute_position(BPTREE_LAST_INDEX);
    _next_block = 0;
    _buffer.write_4byte_number(_next_block);
}

BPTreeLeafBlock::BPTreeLeafBlock(unsigned char* block_buffer) {
    _buffer = Buffer(block_buffer, BLOCK_SIZE);

    // header data
    _is_leaf = read_2byte_number_from_buffer(block_buffer, 0);
    _buffer.write_2byte_number(_is_leaf);
    _qt_keys = read_2byte_number_from_buffer(block_buffer, 2);
    _buffer.write_2byte_number(_qt_keys);

    _buffer.jump_to_absolute_position(BPTREE_HEADER_SIZE);

    // <ptr,key> pair
    _first_block_index = read_4byte_number_from_buffer(block_buffer, 12);
    _buffer.write_4byte_number(_first_block_index);
    _first_key_value = read_4byte_number_from_buffer(block_buffer, 16);
    _buffer.write_4byte_number(_first_key_value);

    // ptr next block
    _buffer.jump_to_absolute_position(BPTREE_LAST_INDEX);
    _next_block = read_4byte_number_from_buffer(block_buffer, BPTREE_LAST_INDEX);
    _buffer.write_4byte_number(_next_block);
}

unsigned int BPTreeLeafBlock::get_first_key() {
    _buffer.jump_to_absolute_position(BPTREE_HEADER_SIZE + 4);

    return _buffer.read_4byte_number();
}

bool BPTreeLeafBlock::are_there_free_slots() {
    _buffer.jump_to_absolute_position(2);

    return _buffer.read_2byte_number() < BPTREE_MAX_KEYS_PER_BLOCK;
}

/**
 * assumindo que sempre ha espaco, ou seja, nao eh necessario alterar o ponteiro da lista encadeada
 */
void BPTreeLeafBlock::insert_key(unsigned int key, unsigned int data_file_block_index) {
    _buffer.jump_to_absolute_position(2);
    unsigned short int qt_keys = _buffer.read_2byte_number();

    _buffer.jump_to_absolute_position(BPTREE_HEADER_SIZE);

    unsigned int current_index = _buffer.read_4byte_number();
    unsigned int current_key = _buffer.read_4byte_number();
    int keys_read = 1;

    // encontra a posicao correta para inserir a chave
    while (keys_read < qt_keys && key > current_key) {
        current_index = _buffer.read_4byte_number();
        current_key = _buffer.read_4byte_number();

        keys_read++;
    }

    //insere a chave na posicao correta
    unsigned int aux_index = current_index;
    unsigned int aux_key = current_key;

    if (qt_keys == 0) {
        _buffer.jump_to_absolute_position(BPTREE_HEADER_SIZE);
    }

    _buffer.write_4byte_number(data_file_block_index);
    _buffer.write_4byte_number(key);

    // da um shift no restante das chaves
    while(keys_read < qt_keys) {
        unsigned int current_position = _buffer.get_current_cursor_position();
        current_index = _buffer.read_4byte_number();
        current_key = _buffer.read_4byte_number();

        _buffer.jump_to_absolute_position(current_position);

        _buffer.write_4byte_number(aux_index);
        _buffer.write_4byte_number(aux_key);

        aux_index = current_index;
        current_key = current_key;

        keys_read++;
    }

    // atualiza a quantidade de chaves
    _buffer.jump_to_absolute_position(2);
    _buffer.write_2byte_number(qt_keys + 1);
}

unsigned int BPTreeLeafBlock::point_to_new_block(unsigned int block_index) {
    _buffer.jump_to_absolute_position(BPTREE_LAST_INDEX);
    unsigned int old_next_block = _buffer.read_4byte_number();

    _buffer.jump_to_absolute_position(BPTREE_LAST_INDEX);
    _buffer.write_4byte_number(block_index);

    return old_next_block;
}

char* BPTreeLeafBlock::get_block_buffer() {
    return _buffer.get_buffer_bytes();
}

void BPTreeLeafBlock::free() {
    _buffer.free();
}
