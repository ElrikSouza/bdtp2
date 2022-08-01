#include "../buffer/buffer.h"
#include "../config.h"

#pragma once

class BPTreeHeaderBlock {
   private:
    unsigned int _root_index;
    unsigned int _next_free_block_index;
    unsigned int _qt_blocks;
    Buffer _buffer{nullptr, 0};

   public:
    BPTreeHeaderBlock();
    BPTreeHeaderBlock(unsigned char* block_buffer);

    /**
     * Retorna o endereço do bloco raiz da árvore.
     */
    unsigned int get_root_node_index();

    /**
     * Retorna o endereço do próximo bloco livre no arquivo e aponta para um novo livre.
     */
    unsigned int get_next_free_block_and_point_to_next();

    /**
     * Retorna a quantidade de blocos ocupada pela árvore.
     */
    unsigned int get_qt_blocks();

    /**
     * Referencia o apontador de raiz da árvore para o valor de 'new_root_index'.
     */
    void set_new_root_index(unsigned int new_root_index);

    char* get_block_buffer();
};