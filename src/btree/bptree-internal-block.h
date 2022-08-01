#include "../buffer/buffer.h"
#include "../config.h"

#pragma once

class BPTreeInternalBlock {
   private:
    unsigned short int _is_leaf;
    unsigned short int _qt_keys;
    unsigned int _first_key_value;
    unsigned int _first_subtree_block_index;
    unsigned int _last_subtree_block_index;
    Buffer _buffer{nullptr, 0};

   public:
    BPTreeInternalBlock();
    BPTreeInternalBlock(unsigned char* block_buffer);

    /**
     * Retorna a primeira chave do bloco.
     */
    unsigned int get_first_key();

    /**
     * Utilizado após o split na raiz quando a mesma estiver cheia. Insere a chave 'key'
     * e seus apontadores 'old_root_index' e 'new_node'.
     */
    void insert_key_for_root(unsigned int old_root_index, unsigned int key, unsigned int new_node);

    /**
     * Insere o par <node_block_index,key> no nó.
     */
    void insert_key(unsigned int key, unsigned int node_block_index);

    /**
     * Retorna o apontador correspondente à chave 'key', i.e., o apontador que referencia
     * a subárvore à esquerda da chave.
     */
    unsigned int get_matching_pointer(unsigned int key);

    /**
     * Verifica se há espaço no nó para inserir um novo par <index,key>.
     */
    bool are_there_free_slots();

    char* get_block_buffer();

    void free();
};