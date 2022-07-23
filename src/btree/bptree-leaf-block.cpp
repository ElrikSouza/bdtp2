#include "bptree-leaf-block.h"

#include "../buffer/buffer-utils.h"

#define MAX_KEYS_PER_BLOCK 510

BPTreeLeafBlock::BPTreeLeafBlock(int block_size) {
    _buffer = Buffer(block_size);

    // header data
    _is_leaf = 1;
    _buffer.write_2byte_number(_is_leaf);
    _qt_keys = 0;
    _buffer.write_2byte_number(_qt_keys);

    _buffer.jump_n_bytes_from_current_position(8);

    // <ptr,key> pair
    _first_block_index = 0;
    _buffer.write_4byte_number(_first_block_index);
    _first_key_value = 0;
    _buffer.write_4byte_number(_first_key_value);

    // ptr next block
    _buffer.jump_to_the_start();
    _buffer.jump_n_bytes_from_current_position(4092);
    _next_block = 0;
    _buffer.write_4byte_number(_next_block);
}

BPTreeLeafBlock::BPTreeLeafBlock(unsigned char* block_buffer, int block_size) {
    _buffer = Buffer(block_buffer, block_size);

    // header data
    _is_leaf = read_2byte_number_from_buffer(block_buffer, 0);
    _buffer.write_2byte_number(_is_leaf);
    _qt_keys = read_2byte_number_from_buffer(block_buffer, 2);
    _buffer.write_2byte_number(_qt_keys);

    _buffer.jump_n_bytes_from_current_position(8);

    // <ptr,key> pair
    _first_block_index = read_4byte_number_from_buffer(block_buffer, 12);
    _buffer.write_4byte_number(_first_block_index);
    _first_key_value = read_4byte_number_from_buffer(block_buffer, 16);
    _buffer.write_4byte_number(_first_key_value);

    // ptr next block
    _buffer.jump_to_the_start();
    _buffer.jump_n_bytes_from_current_position(4092);
    _next_block = read_4byte_number_from_buffer(block_buffer, 4092);
    _buffer.write_4byte_number(_next_block);
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

unsigned int BPTreeLeafBlock::get_first_key() {
    _jump_header_bytes();
    _buffer.jump_n_bytes_from_current_position(4);

    return _buffer.read_4byte_number();
}

void BPTreeLeafBlock::transfer_data_and_pointers_to_split_node(BPTreeLeafBlock* split_node,
                                                               unsigned int split_node_block_index) {
    // write header
    split_node->_buffer.jump_to_the_start();
    split_node->_buffer.write_2byte_number(1);
    split_node->_buffer.write_4byte_number(0);
    split_node->_buffer.write_4byte_number(0);

    // deixar no index pra escrever o next
    split_node->_buffer.jump_n_bytes_from_current_position(4080);

    _buffer.jump_to_the_start();
    _buffer.jump_n_bytes_from_current_position(4092);

    // atualizar os ponteiros next
    unsigned int next = _buffer.read_4byte_number();
    split_node->_buffer.write_4byte_number(next);

    _buffer.jump_n_bytes_from_current_position(-4);
    _buffer.write_4byte_number(split_node_block_index);

    split_node->_buffer.jump_to_the_start();
    split_node->_buffer.jump_n_bytes_from_current_position(12);
    split_node->_buffer.write_bytes(_buffer.get_buffer_bytes() + 12 + 255 * 8, 255 * 8);

    // zerar o resto do buffer do bloco original
    char* zeros = new char[255 * 8];
    _buffer.jump_to_the_start();
    _buffer.jump_n_bytes_from_current_position(12 + 255 * 8);
    _buffer.write_bytes(zeros, 255 * 8);
    delete[] zeros;
}

// bool BPTreeLeafBlock::are_there_free_slots() {
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

bool BPTreeLeafBlock::are_there_free_slots() {
    _buffer.jump_to_the_start();
    _buffer.jump_n_bytes_from_current_position(2);

    return _buffer.read_2byte_number() < MAX_KEYS_PER_BLOCK;
}

void BPTreeLeafBlock::insert_key(unsigned int key, unsigned int data_file_block_index) {
    _buffer.jump_to_the_start();
    _buffer.jump_n_bytes_from_current_position(2);
    unsigned short int qt_keys = _buffer.read_2byte_number();
    int keys_read = 0;

    _buffer.jump_n_bytes_from_current_position(8);

    unsigned int current_block_index = _buffer.read_4byte_number();
    unsigned int current_key = _buffer.read_4byte_number();

    // encontra a posicao correta para inserir a chave
    while (keys_read < qt_keys && key > current_key) {
        current_block_index = _buffer.read_4byte_number();
        current_key = _buffer.read_4byte_number();

        keys_read++;
    }

    //insere a chave na posicao correta
    unsigned int aux_index = current_block_index;
    unsigned int aux_key = current_key;
    _buffer.write_4byte_number(data_file_block_index);
    _buffer.write_4byte_number(key);
    // da um shift no restante das chaves
    while(keys_read < qt_keys) {
        unsigned int current_position = _buffer.get_current_cursor_position();
        current_block_index = _buffer.read_4byte_number();
        current_key = _buffer.read_4byte_number();

        _buffer.jump_to_the_start();
        _buffer.jump_n_bytes_from_current_position(current_position);

        _buffer.write_4byte_number(aux_index);
        _buffer.write_4byte_number(aux_key);

        aux_index = current_block_index;
        current_key = current_key;

        keys_read++;
    }

    // atualiza a quantidade de chaves
    _buffer.jump_to_the_start();
    _buffer.jump_n_bytes_from_current_position(2);
    _buffer.write_2byte_number(qt_keys + 1);
}

unsigned int BPTreeLeafBlock::point_to_new_block(unsigned int block_index) {
    unsigned int old_next_block = read_4byte_number_from_buffer((unsigned char*) _buffer.get_buffer_bytes(), 4092);

    _buffer.jump_to_the_start();
    _buffer.jump_n_bytes_from_current_position(4092);
    _buffer.write_4byte_number(block_index);

    return old_next_block;
}

void BPTreeLeafBlock::transfer_first_half_of_keys_and_pointers(BPTreeLeafBlock* leaf) {
    _buffer.jump_to_the_start();
    _buffer.jump_n_bytes_from_current_position(2);
    unsigned short int qt_keys = _buffer.read_2byte_number();
    _buffer.jump_n_bytes_from_current_position(8);

    for (int i = 0; i < qt_keys / 2; i++) {
        unsigned int current_index = _buffer.read_4byte_number();
        unsigned int current_key = _buffer.read_4byte_number();

        write_4byte_number_to_buffer(leaf->get_block_buffer(), current_index, i + 12);
        write_4byte_number_to_buffer(leaf->get_block_buffer(), current_key, i + 12 + 4);
    }

    // atualiza a quantidade de chaves do header
    write_2byte_number_to_buffer(leaf->get_block_buffer(), qt_keys / 2, 2);
}

void BPTreeLeafBlock::transfer_second_half_of_keys_and_pointers(BPTreeLeafBlock* leaf) {
    _buffer.jump_to_the_start();
    _buffer.jump_n_bytes_from_current_position(2);
    unsigned short int qt_keys = _buffer.read_2byte_number();
    _buffer.jump_n_bytes_from_current_position(8);

    for (int i = qt_keys / 2; i < qt_keys; i++) {
        unsigned int current_index = _buffer.read_4byte_number();
        unsigned int current_key = _buffer.read_4byte_number();

        write_4byte_number_to_buffer(leaf->get_block_buffer(), current_index, i + 12);
        write_4byte_number_to_buffer(leaf->get_block_buffer(), current_key, i + 12 + 4);
    }

    // atualiza a quantidade de chaves do header
    write_2byte_number_to_buffer(leaf->get_block_buffer(), 1 + qt_keys / 2, 2);
}

// void BPTreeLeafBlock::insert_key(unsigned int key, unsigned int data_file_block_index) {
//     _jump_header_bytes();

//     unsigned int tree_key = _buffer.read_4byte_number();

//     // jump nos bytes do ponteiro de dados
//     _buffer.jump_n_bytes_from_current_position(4);

//     int keys_read = 0;

//     // procurar qual o index da nova chave dentro do bloco
//     while (tree_key < key && tree_key != 0) {
//         tree_key = _buffer.read_4byte_number();
//         _buffer.jump_n_bytes_from_current_position(4);
//         keys_read++;
//     }

//     Buffer temp(4096);

//     // vamos utilizar um novo buffer auxiliar para abrirmos um slot para os novos dados

//     // vamos copiar o header
//     char* original = _buffer.get_buffer_bytes();

//     int bytes_till_open_slot = keys_read * 8 + 12;
//     // vamos copiar o header e as chaves da esquerda
//     temp.write_bytes(original, bytes_till_open_slot);

//     // vamos colocar os novos dados
//     temp.write_4byte_number(key);
//     temp.write_4byte_number(data_file_block_index);

//     // vamos colocar os dados da parte direita e substituir o buffer orignal
    // temp.write_bytes(original + bytes_till_open_slot, 4096 - bytes_till_open_slot);
//     _buffer.free();
//     _buffer = temp;
// }

char* BPTreeLeafBlock::get_block_buffer() {
    return _buffer.get_buffer_bytes();
}

void BPTreeLeafBlock::free() {
    _buffer.free();
}
