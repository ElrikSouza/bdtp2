#include "bptree-header-block.h"

#include <stack>

#include "../binary-file.h"
#include "../buffer/buffer.h"
#include "../config.h"

#pragma once

class BPTree {
   private:
    BPTreeHeaderBlock _header_block;
    BinaryFile _tree_file;

    unsigned int get_root_node();

    /**
     * Dado o caminho até um nó armazenado em 'path_to_leaf', insere o par <key,data_file_block_index> no nó contido no topo da pilha.
     * Caso o nó não possua espaço, insere recursivamente até encontrar um nó que possua espaço.
     */
    void insert_in_parent(std::stack<unsigned int>* path_to_leaf, unsigned int key, unsigned int data_file_block_index);

   public:
    BPTree();

    /**
     * Retorna a pilha de nós que devem ser percorridos até chegar no bloco que contém
     * a chave 'key'.
     */
    std::stack<unsigned int> get_path_to_leaf(unsigned int key);

    /**
     * Procura pela chave 'key' na árvore. 
     * Se existir, retorna o endereço do bloco correspondente no arquivo de dados.
     * Caso contrario, retorna 0.
     */
    unsigned int get_data_pointer(unsigned int key);

    /**
     * Insere a chave 'key' com endereço de bloco 'data_file_block_index' na árvore.
     */
    void insert_key(unsigned int key, unsigned int data_file_block_index);

    bool is_leaf(unsigned char* block_buffer);

    void commit_header();

    /**
     * Cria ou reescreve uma arvore B+ no caminho especificado.
     */
    void create_tree_file(const char* filename);

    /**
     * Apenas lê uma arvore B+ no caminho especificado.
     */
    void read_tree_file(const char* filename);
};
