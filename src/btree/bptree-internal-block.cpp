#include "bptree-internal-block.h"

#include <vector>

#include "../buffer/buffer-utils.h"

BPTreeInternalBlock::BPTreeInternalBlock() {
    _buffer = Buffer(BLOCK_SIZE);

    // header data
    _is_leaf = 0;
    _buffer.write_2byte_number(_is_leaf);
    _qt_keys = 0;
    _buffer.write_2byte_number(_qt_keys);

    _buffer.jump_to_absolute_position(BPTREE_HEADER_SIZE);

    // <ptr,key> pair
    _first_subtree_block_index = 0;
    _buffer.write_4byte_number(_first_subtree_block_index);
    _first_key_value = 0;
    _buffer.write_4byte_number(_first_key_value);

    // ptr last subtree
    _buffer.jump_to_absolute_position(BPTREE_LAST_INDEX);
    _last_subtree_block_index = 0;
    _buffer.write_4byte_number(_last_subtree_block_index);
}

BPTreeInternalBlock::BPTreeInternalBlock(unsigned char* block_buffer) {
    _buffer = Buffer(block_buffer, BLOCK_SIZE);

    // header data
    _is_leaf = read_2byte_number_from_buffer(block_buffer, 0);
    _buffer.write_2byte_number(_is_leaf);
    _qt_keys = read_2byte_number_from_buffer(block_buffer, 2);
    _buffer.write_2byte_number(_qt_keys);

    _buffer.jump_to_absolute_position(BPTREE_HEADER_SIZE);

    // <ptr,key> pair
    _first_subtree_block_index = read_4byte_number_from_buffer(block_buffer, 12);
    _buffer.write_4byte_number(_first_subtree_block_index);
    _first_key_value = read_4byte_number_from_buffer(block_buffer, 16);
    _buffer.write_4byte_number(_first_key_value);

    // ptr last subtree
    _buffer.jump_to_absolute_position(BPTREE_LAST_INDEX);
    _last_subtree_block_index = read_4byte_number_from_buffer(block_buffer, BPTREE_LAST_INDEX);
    _buffer.write_4byte_number(_last_subtree_block_index);
}

unsigned int BPTreeInternalBlock::get_first_key() {
    _buffer.jump_to_absolute_position(BPTREE_HEADER_SIZE + 4);

    return _buffer.read_4byte_number();
}

void BPTreeInternalBlock::insert_key_for_root(unsigned int old_root_index, unsigned int key, unsigned int new_node) {
    // escreve o cabecalho de qt_keys
    _buffer.jump_to_absolute_position(2);
    _buffer.write_2byte_number(1);

    // pula os espacos vazios do header
    _buffer.jump_to_absolute_position(BPTREE_HEADER_SIZE);

    _buffer.write_4byte_number(old_root_index);
    _buffer.write_4byte_number(key);

    _buffer.jump_to_absolute_position(BPTREE_LAST_INDEX);
    _buffer.write_4byte_number(new_node);
}

void BPTreeInternalBlock::insert_key(unsigned int key, unsigned int node_block_index) {
    char* internal_buffer = _buffer.get_buffer_bytes();
    unsigned short int qt_keys = read_2byte_number_from_buffer((unsigned char*) internal_buffer, 2);

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

    unsigned int last_index = read_4byte_number_from_buffer((unsigned char*) internal_buffer, BPTREE_LAST_INDEX);
    _buffer.write_4byte_number(last_index);
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

    // atualiza o apontador da maior subarvore
    _buffer.jump_to_absolute_position(BPTREE_LAST_INDEX);
    _buffer.write_4byte_number(node_block_index);
}

unsigned int BPTreeInternalBlock::get_matching_pointer(unsigned int search_key) {
    _buffer.jump_to_absolute_position(BPTREE_HEADER_SIZE);

    unsigned int pointer;
    unsigned int internal_key;

    while (_buffer.get_current_cursor_position() <= BLOCK_SIZE - BPTREE_HEADER_SIZE) {
        pointer = _buffer.read_4byte_number();
        internal_key = _buffer.read_4byte_number();

        if (search_key <= internal_key) {
            // retorna o ponteiro da chave
            return pointer;
        }
    }

    return _buffer.read_4byte_number();
}

bool BPTreeInternalBlock::are_there_free_slots() {
    _buffer.jump_to_absolute_position(2);

    return _buffer.read_2byte_number() < BPTREE_MAX_KEYS_PER_BLOCK;
}

char* BPTreeInternalBlock::get_block_buffer() {
    return _buffer.get_buffer_bytes();
}

void BPTreeInternalBlock::free() {
    _buffer.free();
}