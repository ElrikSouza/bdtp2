#include "buffer/buffer-utils.h"
#include "bptree-leaf-block.h"

BPTreeLeafBlock::BPTreeLeafBlock() {
    _first_key_value = 0;
    _first_block_index = 0;
    _buffer = Buffer(4096);
}

BPTreeLeafBlock::BPTreeLeafBlock(unsigned char* block_buffer) {
    _first_key_value = read_4byte_number_from_buffer(block_buffer, 0);
    _first_block_index = read_4byte_number_from_buffer(block_buffer, 4);
    _buffer = Buffer(block_buffer, 4096);
}

unsigned int BPTreeLeafBlock::get_middle_key() {
    unsigned int current_key_value = _first_key_value;
    int qt_keys = 0;

    // encontra a quantidade de chaves no bloco
    _buffer.jump_to_the_start();
    while (_buffer.has_ended()) {
        current_key_value = _buffer.read_4byte_number();
        _buffer.jump_n_bytes_from_current_position(4);

        qt_keys++;
    }

    // posiciona o cursor na chave do meio
    _buffer.jump_to_the_start();
    _buffer.jump_n_bytes_from_current_position((qt_keys / 2) * 8);

    return _buffer.read_4byte_number();
}

void BPTreeLeafBlock::transfer_data_and_pointers_to_split_node(BPTreeLeafBlock* split_node, unsigned int split_node_block_index) {
    // TODO
}

bool BPTreeLeafBlock::are_there_free_slots() {
        unsigned int current_key = _first_key_value;

    _buffer.jump_to_the_start();
    _buffer.jump_n_bytes_from_current_position(4);
    while (!_buffer.has_ended()) {
        if (current_key == 0) return true;

        current_key = _buffer.read_4byte_number();
        _buffer.read_4byte_number();
    }

    return false;
}

bool BPTreeLeafBlock::insert_key(unsigned int key, unsigned int data_file_block_index) {
    unsigned int current_key_value = _first_key_value;
    unsigned int current_block_index = _first_block_index;

    _buffer.jump_to_the_start();
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
}

char* BPTreeLeafBlock::get_block_buffer() {
    return _buffer.get_buffer_bytes();
}
