#include "bptree-leaf-block.h"

#include "../buffer/buffer-utils.h"

BPTreeLeafBlock::BPTreeLeafBlock() {
    _buffer = Buffer(4096);

    _is_leaf = 1;
    _buffer.write_2byte_number(_is_leaf);
    _qt_keys = 0;
    _buffer.write_2byte_number(_qt_keys);
    _buffer.jump_n_bytes_from_current_position(8);
    _first_key_value = 0;
    _buffer.write_4byte_number(_first_key_value);
    _first_block_index = 0;
    _buffer.write_4byte_number(_first_block_index);

    _buffer.jump_to_the_start();
    // unsigned int aa = _buffer.read_2byte_number();
    // unsigned int bb = read_2byte_number_from_buffer((unsigned char*) _buffer.get_buffer_bytes(), 0);
    // std::cout << "valor1 = " << aa << std::endl;
    // std::cout << "valor2 = " << bb << std::endl;
}

BPTreeLeafBlock::BPTreeLeafBlock(unsigned char* block_buffer) {
    _is_leaf = read_2byte_number_from_buffer(block_buffer, 0);
    _qt_keys = read_2byte_number_from_buffer(block_buffer, 2);
    _first_key_value = read_4byte_number_from_buffer(block_buffer, 12);
    _first_block_index = read_4byte_number_from_buffer(block_buffer, 16);
    _buffer = Buffer(block_buffer, 4096);
}

void BPTreeLeafBlock::_jump_header_bytes() {
    _buffer.jump_to_the_start();
    _buffer.jump_n_bytes_from_current_position(12);
}

unsigned int BPTreeLeafBlock::get_middle_key() {
    _jump_header_bytes();
    // posiciona o cursor na chave do meio
    _buffer.jump_n_bytes_from_current_position((_qt_keys / 2) * 8);

    return _buffer.read_4byte_number();
}

void BPTreeLeafBlock::transfer_data_and_pointers_to_split_node(BPTreeLeafBlock* split_node, unsigned int split_node_block_index) {
    _jump_header_bytes();
    // posiciona o cursor na chave do meio
    _buffer.jump_n_bytes_from_current_position((_qt_keys / 2) * 8);

    // copia os pares <chave,apontador> do meio ate o ultimo para o novo no
    while(!_buffer.has_ended()) {
        unsigned int key = _buffer.read_4byte_number();
        unsigned int pointer = _buffer.read_4byte_number();
        split_node->insert_key(key, pointer);
    }

    // corrige os ponteiros da lista encadeada
    split_node->_next_block = _next_block;
    _next_block = split_node_block_index;
}

bool BPTreeLeafBlock::are_there_free_slots() {
    unsigned int current_key = _first_key_value;

    _jump_header_bytes();
    _buffer.jump_n_bytes_from_current_position(4);
    while (!_buffer.has_ended()) {
        if (current_key == 0) return true;

        current_key = _buffer.read_4byte_number();
        _buffer.read_4byte_number();
    }

    return false;
}

void BPTreeLeafBlock::insert_key(unsigned int key, unsigned int data_file_block_index) {
    unsigned int current_key_value = _first_key_value;
    unsigned int current_block_index = _first_block_index;

    _jump_header_bytes();
    int offset = _buffer.get_current_cursor_position();
    // insere ordenado no bloco
    while (!_buffer.has_ended() && (key >= current_key_value)) {
        current_key_value = _buffer.read_4byte_number();
        current_block_index = _buffer.read_4byte_number();
    }

    unsigned int aux_current_key_value = current_key_value;
    unsigned int aux_current_block_index = current_block_index;

    // escreve o novo par na posicao correta
    _buffer.write_4byte_number(key);
    _buffer.write_4byte_number(data_file_block_index);

    // da shift no restante das chaves
    while(!_buffer.has_ended()) {
        current_key_value = read_4byte_number_from_buffer((unsigned char*) _buffer.get_buffer_bytes(), _buffer.get_current_cursor_position());
        current_block_index = read_4byte_number_from_buffer((unsigned char*) _buffer.get_buffer_bytes(), _buffer.get_current_cursor_position() + 4);

        _buffer.write_4byte_number(aux_current_key_value);
        _buffer.write_4byte_number(aux_current_block_index);

        aux_current_key_value = current_key_value;
        aux_current_block_index = current_block_index;
    }

    _qt_keys++;
}

char* BPTreeLeafBlock::get_block_buffer() {
    return _buffer.get_buffer_bytes();
}
