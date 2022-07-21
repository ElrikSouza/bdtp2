#include "bptree-internal-block.h"

#include <vector>

#include "../buffer/buffer-utils.h"

BPTreeInternalBlock::BPTreeInternalBlock() {
    _first_key_value = 0;
    _first_subtree_block_index = 0;
    _buffer = Buffer(4096);
}

BPTreeInternalBlock::BPTreeInternalBlock(unsigned char* block_buffer) {
    _first_key_value = read_4byte_number_from_buffer(block_buffer, 0);
    _first_subtree_block_index = read_4byte_number_from_buffer(block_buffer, 4);
    _buffer = Buffer(block_buffer, 4096);
}

// // assumindo que o bloco em questao nao precisa de split, insere o par <key,split_node_block_index> de forma ordenada
// void BPTreeInternalBlock::insert_key(unsigned int key, unsigned int split_node_block_index) {
//     unsigned int current_key_value = _first_key_value;
//     unsigned int current_subtree_block_index = _first_subtree_block_index;

//     _buffer.jump_to_the_start();
//     int offset = _buffer.get_current_cursor_position();
//     // insere ordenado no bloco
//     while (!_buffer.has_ended() && (key >= current_key_value)) {
//         current_key_value = _buffer.read_4byte_number();
//         current_subtree_block_index = _buffer.read_4byte_number();
//     }

//     unsigned int aux_current_key_value = current_key_value;
//     unsigned int aux_current_subtree_block_index = current_subtree_block_index;

//     // escreve o novo par na posicao correta
//     _buffer.write_4byte_number(key);
//     _buffer.write_4byte_number(split_node_block_index);

//     // da shift no restante das chaves
//     while (!_buffer.has_ended()) {
//         current_key_value = read_4byte_number_from_buffer((unsigned char*)_buffer.get_buffer_bytes(),
//                                                           _buffer.get_current_cursor_position());
//         current_subtree_block_index = read_4byte_number_from_buffer((unsigned char*)_buffer.get_buffer_bytes(),
//                                                                     _buffer.get_current_cursor_position() + 4);

//         _buffer.write_4byte_number(aux_current_key_value);
//         _buffer.write_4byte_number(aux_current_subtree_block_index);

//         aux_current_key_value = current_key_value;
//         aux_current_subtree_block_index = current_subtree_block_index;
//     }
// }
unsigned int BPTreeInternalBlock::get_first_key() {
    _buffer.jump_to_the_start();
    _buffer.jump_n_bytes_from_current_position(12);

    return _buffer.read_4byte_number();
}

unsigned int BPTreeInternalBlock::get_middle_key() {
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

void BPTreeInternalBlock::insert_key(unsigned int key, unsigned int pointer) {
    _buffer.jump_to_the_start();
    _buffer.jump_n_bytes_from_current_position(12);

    std::vector<unsigned int> keys;
    std::vector<unsigned int> pointers;

    unsigned int current_key = _buffer.read_4byte_number();
    unsigned int current_pointer = _buffer.read_4byte_number();

    while (_buffer.get_current_cursor_position() <= 4096 - 12 && current_key != 0) {
        keys.push_back(current_key);
        pointers.push_back(current_pointer);

        current_key = _buffer.read_4byte_number();
        current_pointer = _buffer.read_4byte_number();
    }

    _buffer.jump_to_the_start();
    _buffer.jump_n_bytes_from_current_position(4092);

    unsigned int greater_than_pointer = _buffer.read_4byte_number();

    if (greater_than_pointer != 0) {
        pointers.push_back(greater_than_pointer);
    }

    int new_key_index = 0;
    std::cout << "[debug] keys" << keys.size() << std::endl;
    while (key > keys[new_key_index] && new_key_index < keys.size()) {
        new_key_index++;
    }

    if (keys.size() < pointers.size() && new_key_index == keys.size()) {
        pointers.insert(pointers.begin() + new_key_index + 1, pointer);
    } else {
        pointers.insert(pointers.begin() + new_key_index, pointer);
    }

    keys.insert(keys.begin() + new_key_index, key);

    _buffer.jump_to_the_start();
    _buffer.jump_n_bytes_from_current_position(12);

    for (int i = 0; i < keys.size(); i++) {
        _buffer.write_4byte_number(keys[i]);
        _buffer.write_4byte_number(pointers[i]);
    }

    // se tiver um ponteiro ¨maior que¨ a ultima chave, coloca esse ponteiro de volta
    if (pointers.size() > keys.size()) {
        _buffer.jump_to_the_start();
        _buffer.jump_n_bytes_from_current_position(4092);
        _buffer.write_4byte_number(pointers[pointers.size() - 1]);
    }

    for (int a : keys) {
        std::cout << a << ",";
    }
    std::cout << "\n";

    for (int a : pointers) {
        std::cout << a << ",";
    }
}

void BPTreeInternalBlock::transfer_data_and_pointers_to_split_node(BPTreeInternalBlock* split_node,
                                                                   unsigned int split_node_block_index) {}

unsigned int BPTreeInternalBlock::get_matching_pointer(unsigned int search_key) {
    _buffer.jump_to_the_start();
    _buffer.jump_n_bytes_from_current_position(12);

    unsigned int internal_key;
    unsigned int pointer;

    while (_buffer.get_current_cursor_position() <= 4096 - 12) {
        internal_key = _buffer.read_4byte_number();
        pointer = _buffer.read_4byte_number();

        if (search_key < internal_key) {
            // retorna o ponteiro da chave
            return pointer;
        }
    }

    return _buffer.read_4byte_number();
}

bool BPTreeInternalBlock::are_there_free_slots() {
    // vamos pular para o slot da ultima chave (tem dois ponteiros na frente + o espaco da chave, por isso 12)
    _buffer.jump_to_the_start();
    _buffer.jump_n_bytes_from_current_position(4096 - 12);

    // nao tem como ter uma chave 0 no ultimo slot.
    unsigned int last_key = _buffer.read_4byte_number();

    if (last_key != 0) {
        std::cout << "debug: full" << std::endl;
    }

    // se tem 0  entao o ultimo slot ta vazio
    return last_key == 0;
}

char* BPTreeInternalBlock::get_block_buffer() {
    return _buffer.get_buffer_bytes();
}
