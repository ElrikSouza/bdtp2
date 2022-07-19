#include "bptree-internal-block.h"
#include "buffer/buffer-utils.h"

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

// assumindo que o bloco em questao nao precisa de split, insere o par <key,split_node_block_index> de forma ordenada
void BPTreeInternalBlock::insert_key(unsigned int key, unsigned int split_node_block_index) {
    unsigned int current_key_value = _first_key_value;
    unsigned int current_subtree_block_index = _first_subtree_block_index;

    _buffer.jump_to_the_start();
    int offset = _buffer.get_current_cursor_position();
    // insere ordenado no bloco
    while (!_buffer.has_ended() && (key >= current_key_value)) {
        current_key_value = _buffer.read_4byte_number();
        current_subtree_block_index = _buffer.read_4byte_number();
    }

    unsigned int aux_current_key_value = current_key_value;
    unsigned int aux_current_subtree_block_index = current_subtree_block_index;

    // escreve o novo par na posicao correta
    _buffer.write_4byte_number(key);
    _buffer.write_4byte_number(split_node_block_index);

    // da shift no restante das chaves
    while(!_buffer.has_ended()) {
        current_key_value = read_4byte_number_from_buffer((unsigned char*) _buffer.get_buffer_bytes(), _buffer.get_current_cursor_position());
        current_subtree_block_index = read_4byte_number_from_buffer((unsigned char*) _buffer.get_buffer_bytes(), _buffer.get_current_cursor_position() + 4);

        _buffer.write_4byte_number(aux_current_key_value);
        _buffer.write_4byte_number(aux_current_subtree_block_index);

        aux_current_key_value = current_key_value;
        aux_current_subtree_block_index = current_subtree_block_index;
    }
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

void BPTreeInternalBlock::transfer_data_and_pointers_to_split_node(BPTreeInternalBlock* split_node, unsigned int split_node_block_index) {
    // TODO
}

// dada a chave `key`, retorna o ponteiro para o bloco que indica a menor subarvore da sequencia
// se a `key` for maior que todos as chaves do bloco, retorna o ponteiro que indica a maior subarvore da sequencia
unsigned int BPTreeInternalBlock::get_matching_pointer(unsigned int key) {
    unsigned int current_key = _first_key_value;
    unsigned int current_subtree_block_index = _first_subtree_block_index;
    
    _buffer.jump_to_the_start();
    _buffer.jump_n_bytes_from_current_position(8);

    while(!_buffer.has_ended() && key >= current_key) {
        current_key = _buffer.read_4byte_number();
        current_subtree_block_index = _buffer.read_4byte_number();
    }

    // se acabou os pares <chave,apontador>, entao deve seve seguir para a subarvore
    // mais a direita, ou seja, o ultimo apontador do bloco
    if (_buffer.has_ended()) return _buffer.read_4byte_number();

    // retorna o caminho correspondente da subarvore
    return current_subtree_block_index;
}

// verifica se o bloco possui espaco para alocar novas chaves
bool BPTreeInternalBlock::are_there_free_slots() {
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

char* BPTreeInternalBlock::get_block_buffer() {
    return _buffer.get_buffer_bytes();
}
