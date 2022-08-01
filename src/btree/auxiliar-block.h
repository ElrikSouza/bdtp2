#include "../buffer/buffer.h"
#include "../config.h"

#pragma once

/**
 * Essa classe representa uma abstração de um bloco de tamanho arbitrário para auxiliar
 * no split da árvore B+.
 */
class AuxiliarBlock {
   private:
    unsigned int _block_size;
    Buffer _buffer{nullptr, 0};

   public:
    AuxiliarBlock(int block_size);
    AuxiliarBlock(char* bytes, int block_size);

    /**
     * Copia todas as chaves do buffer 'node_buffer' de tamanho 'num_of_bytes' para o buffer interno.
     */
    void copy_all_key_pointer(char* node_buffer, int num_of_bytes);

    /**
     * Utilizado quando o buffer interno representa a estrutura de um nó folha.
     * 
     * Copia a primeira metade dos pares <index,key> do buffer interno para
     * o buffer 'leaf_node_buffer'.
     */
    void leaf_copy_first_half_of_key_pointer_to_buffer(char* leaf_node_buffer);

    /**
     * Utilizado quando o buffer interno representa a estrutura de um nó folha.
     * 
     * Copia a segunda metade dos pares <index,key> do buffer interno para
     * o buffer 'leaf_node_buffer'.
     *
     * Não altera o último apontador de 'leaf_node_buffer' que representa a lista encadeada.
     */
    void leaf_copy_second_half_of_key_pointer_to_buffer(char* leaf_node_buffer);

    /**
     * Utilizado quando o buffer interno representa a estrutura de um nó folha.
     * 
     * Insere a chave 'key' com endereço de bloco 'block_index' no buffer interno de forma ordenada.
     *
     * Não altera o último apontador do buffer interno.
     */
    void insert_leaf_key_pointer(unsigned int key, unsigned int block_index);

    /**
     * Utilizado quando o buffer interno representa a estrutura de um nó interno.
     * 
     * Copia a primeira metade dos pares <index,key> do buffer interno para
     * o buffer 'internal_node_buffer'.
     */
    void internal_copy_first_half_of_key_pointer_to_buffer(char* internal_node_buffer);

    /**
     * Utilizado quando o buffer interno representa a estrutura de um nó interno.
     * 
     * Copia a segunda metade dos pares <index,key> do buffer interno para
     * o buffer 'internal_node_buffer' corrigindo o último apontador de
     * 'internal_node_buffer'.
     */
    void internal_copy_second_half_of_key_pointer_to_buffer(char* internal_node_buffer);

    /**
     * Utilizado quando o buffer interno representa a estrutura de um nó interno.
     * 
     * Insere a chave 'key' com endereço de bloco 'block_index' no buffer interno de forma ordenada
     * corrigindo o último apontador do buffer interno.
     */
    void insert_internal_key_pointer(unsigned int key, unsigned int block_index);

    char* get_block_buffer();

    void free();
};