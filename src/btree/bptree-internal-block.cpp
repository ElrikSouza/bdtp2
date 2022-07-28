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

// unsigned int BPTreeInternalBlock::get_middle_key() {
//     unsigned int current_key_value = _first_key_value;
//     int qt_keys = 0;

//     // encontra a quantidade de chaves no bloco
//     _buffer.jump_to_the_start();
//     while (_buffer.has_ended()) {
//         current_key_value = _buffer.read_4byte_number();
//         _buffer.jump_n_bytes_from_current_position(4);

//         qt_keys++;
//     }

//     // posiciona o cursor na chave do meio
//     _buffer.jump_to_the_start();
//     _buffer.jump_n_bytes_from_current_position((qt_keys / 2) * 8);

//     return _buffer.read_4byte_number();
// }

// void BPTreeInternalBlock::insert_key(unsigned int key, unsigned int pointer) {
//     _buffer.jump_to_the_start();
//     _buffer.jump_n_bytes_from_current_position(12);

//     std::vector<unsigned int> keys;
//     std::vector<unsigned int> pointers;

//     unsigned int current_key = _buffer.read_4byte_number();
//     unsigned int current_pointer = _buffer.read_4byte_number();

//     while (_buffer.get_current_cursor_position() <= 4096 - 12 && current_key != 0) {
//         keys.push_back(current_key);
//         pointers.push_back(current_pointer);

//         current_key = _buffer.read_4byte_number();
//         current_pointer = _buffer.read_4byte_number();
//     }

//     _buffer.jump_to_the_start();
//     _buffer.jump_n_bytes_from_current_position(4092);

//     unsigned int greater_than_pointer = _buffer.read_4byte_number();

//     if (greater_than_pointer != 0) {
//         pointers.push_back(greater_than_pointer);
//     }

//     int new_key_index = 0;
//     // std::cout << "[debug] keys" << keys.size() << std::endl;
//     while (key > keys[new_key_index] && new_key_index < keys.size()) {
//         new_key_index++;
//     }

//     if (keys.size() < pointers.size() && new_key_index == keys.size()) {
//         pointers.insert(pointers.begin() + new_key_index + 1, pointer);
//     } else {
//         pointers.insert(pointers.begin() + new_key_index, pointer);
//     }

//     keys.insert(keys.begin() + new_key_index, key);

//     _buffer.jump_to_the_start();
//     _buffer.jump_n_bytes_from_current_position(12);

//     for (int i = 0; i < keys.size(); i++) {
//         _buffer.write_4byte_number(keys[i]);
//         _buffer.write_4byte_number(pointers[i]);
//     }

//     // se tiver um ponteiro ¨maior que¨ a ultima chave, coloca esse ponteiro de volta
//     if (pointers.size() > keys.size()) {
//         _buffer.jump_to_the_start();
//         _buffer.jump_n_bytes_from_current_position(4092);
//         _buffer.write_4byte_number(pointers[pointers.size() - 1]);
//     }
// }

void BPTreeInternalBlock::insert_key(unsigned int key, unsigned int node_block_index) {
    char* internal_buffer = _buffer.get_buffer_bytes();
    unsigned short int qt_keys = read_2byte_number_from_buffer((unsigned char*) internal_buffer, 2);

    _buffer.jump_to_absolute_position(BPTREE_HEADER_SIZE);
    unsigned int current_index = _buffer.read_4byte_number();
    unsigned int current_key = _buffer.read_4byte_number();

    int keys_read = 1;
    std::cout << "qt keys = " << qt_keys << std::endl;
    std::cout << "key = " << key << std::endl;

    // encontra a posicao correta para inserir a chave
    while (keys_read < qt_keys && key > current_key) {
        current_index = _buffer.read_4byte_number();
        current_key = _buffer.read_4byte_number();

        std::cout << "current_index = " << current_index << " current_key" << current_key << " key = " << key << std::endl;

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

// unsigned int BPTreeInternalBlock::transfer_data_and_pointers_to_split_node(BPTreeInternalBlock* split_node,
//                                                                            unsigned int overflow_key_pointer,
//                                                                            unsigned int overflow_key) {
//     _buffer.jump_to_the_start();
//     _buffer.jump_n_bytes_from_current_position(12);

//     std::vector<unsigned int> keys;
//     std::vector<unsigned int> pointers;

//     unsigned int current_key = _buffer.read_4byte_number();
//     unsigned int current_pointer = _buffer.read_4byte_number();

//     std::cout << current_key << std::endl;

//     while (_buffer.get_current_cursor_position() <= 4092 && current_key != 0) {
//         keys.push_back(current_key);
//         pointers.push_back(current_pointer);

//         current_key = _buffer.read_4byte_number();
//         current_pointer = _buffer.read_4byte_number();
//     }

//     std::cout << "keys = " << keys.size();

//     _buffer.jump_to_the_start();
//     _buffer.jump_n_bytes_from_current_position(4092);

//     unsigned int greater_than_pointer = _buffer.read_4byte_number();

//     if (greater_than_pointer != 0) {
//         pointers.push_back(greater_than_pointer);
//     }

//     int new_key_index = 0;
//     // std::cout << "[debug] keys" << keys.size() << std::endl;
//     while (new_key_index < keys.size() && overflow_key > keys[new_key_index]) {
//         new_key_index++;
//     }

//     keys.insert(keys.begin() + new_key_index, overflow_key);
//     std::cout << "pointers= " << pointers.size();
//     pointers.insert(pointers.begin() + new_key_index, overflow_key_pointer);
//     std::cout << "pointers= " << pointers.size();
//     // std::cout << "last= " << pointers();

//     // se a funcao esta sendo chamada, esse bloco tem 510 chaves
//     unsigned int middle_key = keys[255];

//     std::cout << "middle" << middle_key << "   " << keys.size() << std::endl;

//     // zerar o resto do buffer do bloco original
//     char* zeros = new char[4096];
//     _buffer.jump_to_the_start();
//     _buffer.write_bytes(zeros, 4096);
//     split_node->_buffer.jump_to_the_start();
//     split_node->_buffer.write_bytes(zeros, 4096);
//     delete zeros;

//     // header
//     _buffer.jump_to_the_start();
//     _buffer.jump_n_bytes_from_current_position(12);

//     // header
//     split_node->_buffer.jump_to_the_start();
//     split_node->_buffer.jump_n_bytes_from_current_position(12);

//     int keys_transfered = 0;
//     int middle = keys.size() / 2;

//     for (int i = 0; i < 255; i++) {
//         _buffer.write_4byte_number(keys[i]);
//         _buffer.write_4byte_number(pointers[i]);
//     }

//     std::cout << "second part\n" << keys[254] << "---" << keys[256] << std::endl;
//     // ponteiro maior que (o fixo da direita)
//     _buffer.jump_to_the_start();
//     _buffer.jump_n_bytes_from_current_position(4092);
//     _buffer.write_4byte_number(pointers[255]);

//     std::cout << pointers[255] << "/" << pointers[510] << " and " << keys[254] << "  " << keys[255] << "  "
//               << keys[256];

//     for (int i = 256; i < 510; i++) {
//         split_node->_buffer.write_4byte_number(keys[i]);
//         split_node->_buffer.write_4byte_number(pointers[i]);
//     }

//     std::cout << pointers[509] << std::endl;
//     split_node->_buffer.jump_to_the_start();
//     split_node->_buffer.jump_n_bytes_from_current_position(4092);
//     split_node->_buffer.write_4byte_number(pointers[pointers.size() - 1]);
//     std::cout << pointers[pointers.size() - 1] << std::endl;

//     std::cout << "done" << std::endl;
//     return middle_key;
// }

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
    // std::cout << "offset=" << _buffer.get_current_cursor_position() << std::endl;

    return _buffer.read_4byte_number();
}

// bool BPTreeInternalBlock::are_there_free_slots() {
//     // vamos pular para o slot da ultima chave (tem dois ponteiros na frente + o espaco da chave, por isso 12)
//     _buffer.jump_to_the_start();
//     _buffer.jump_n_bytes_from_current_position(4096 - 12);

//     // nao tem como ter uma chave 0 no ultimo slot.
//     unsigned int last_key = _buffer.read_4byte_number();

//     // if (last_key != 0) {
//     //     std::cout << "debug: full" << std::endl;
//     // }

//     // se tem 0  entao o ultimo slot ta vazio
//     return last_key == 0;
// }

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