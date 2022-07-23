#include <stack>

#include "../binary-file.h"
#include "../buffer/buffer.h"
#include "bptree-header-block.h"

#pragma once

class BPTree {
   private:
    BPTreeHeaderBlock _header_block;
    BinaryFile _tree_file;
    unsigned int get_root_node();

    void insert_in_leaf(unsigned char* leaf, unsigned int key, unsigned int data_file_block_index);
    void insert_in_parent(std::stack<unsigned int>* path_to_leaf, unsigned char* node, unsigned int key, unsigned int data_file_block_index);

   public:
    BPTree();

    std::stack<unsigned int> get_path_to_leaf(unsigned int key);
    void insert_key(unsigned int key, unsigned int data_file_block_index);

    // procura por uma chave na arvore, se existir, retorna um ponteiro para o bloco
    // correspondente no arquivo de dados, caso contrario, retorna 0
    unsigned int get_data_pointer(unsigned int key);

    // cria ou reescreve uma arvore b+ no caminho especificado
    void create_tree_file(const char* filename);

    // apenas le uma arvore b+ no caminho especificado sem fazer alteracoes
    void read_tree_file(const char* filename);

    bool is_leaf(unsigned char* block_buffer);

    void commit_header();
};
