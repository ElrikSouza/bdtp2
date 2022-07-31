#include "bptree.h"

#include <fstream>
#include <stack>

#include "../binary-file.h"
#include "../buffer/buffer-utils.h"
#include "auxiliar-block.h"
#include "bptree-header-block.h"
#include "bptree-internal-block.h"
#include "bptree-leaf-block.h"

BPTree::BPTree() {
    _tree_file = BinaryFile(100, BLOCK_SIZE);
    _header_block = BPTreeHeaderBlock();
}

unsigned int BPTree::get_root_node() {
    return _header_block.get_root_node_index();
}

std::stack<unsigned int> BPTree::get_path_to_leaf(unsigned int key) {
    std::stack<unsigned int> path;

    unsigned int root_index = get_root_node();
    unsigned char* root_node_buffer = _tree_file.read_block(root_index);

    path.push(root_index);

    if (is_leaf(root_node_buffer)) {
        return path;
    }

    BPTreeInternalBlock internal_block(root_node_buffer);

    while (1) {
        unsigned int matching_pointer = internal_block.get_matching_pointer(key);

        path.push(matching_pointer);

        unsigned char* next_block_buffer = _tree_file.read_block(matching_pointer);

        if (is_leaf(next_block_buffer)) {
            delete next_block_buffer;
            return path;
        }

        internal_block.free();
        internal_block = BPTreeInternalBlock(next_block_buffer);
    }

    return path;
}

unsigned int BPTree::get_data_pointer(unsigned int key) {
    auto path = get_path_to_leaf(key);
    unsigned int leaf_index = path.top();
    Buffer b(_tree_file.read_block(leaf_index), BLOCK_SIZE);
    b.jump_to_absolute_position(BPTREE_HEADER_SIZE);

    unsigned int current_pointer;
    unsigned int current_key;

    b.jump_to_absolute_position(BPTREE_HEADER_SIZE);
    while (b.get_current_cursor_position() <= BLOCK_SIZE - BPTREE_HEADER_SIZE) {
        current_pointer = b.read_4byte_number();
        current_key = b.read_4byte_number();

        if (current_key == key) {
            b.free();
            return current_pointer;
        }

        if (current_key == 0) {
            b.free();
            return 0;
        }
    }

    b.free();
    return 0;
}

void BPTree::insert_key(unsigned int key, unsigned int data_file_block_index) {
    // encontra a folha
    std::stack<unsigned int> path_to_leaf = get_path_to_leaf(key);
    unsigned int leaf_index = path_to_leaf.top();
    path_to_leaf.pop();
    unsigned char* leaf_buffer = _tree_file.read_block(leaf_index);
    BPTreeLeafBlock leaf_block(leaf_buffer);

    // se a folha tem espaco, insere
    if (leaf_block.are_there_free_slots()) {
        leaf_block.insert_key(key, data_file_block_index);
        _tree_file.write_block(leaf_block.get_block_buffer(), leaf_index);
        leaf_block.free();
    } else {  // se nao, faz o split
        // cria um novo folha
        BPTreeLeafBlock new_leaf;
        unsigned int new_leaf_index = _header_block.get_next_free_block_and_point_to_next();

        // cria um bloco auxiliar que tem espaco para todas as chaves do no + o novo par (ptr,chave) a ser inserido
        AuxiliarBlock aux_block(BLOCK_SIZE - BPTREE_HEADER_SIZE +
                                BPTREE_INDEX_VALUE_PAIR_SIZE);  // - cabecalho + novo par (prt,key)

        aux_block.copy_all_key_pointer((char*)leaf_block.get_block_buffer(), BLOCK_SIZE);

        // insere a chave nesse bloco
        aux_block.insert_leaf_key_pointer(key, data_file_block_index);

        // corrige os ponteiros da lista encadeada
        unsigned int old_next_block = leaf_block.point_to_new_block(new_leaf_index);
        new_leaf.point_to_new_block(old_next_block);
        // copia as chaves (inicio, meio) para o no atual ajustando o header de qtd chaves

        aux_block.leaf_copy_first_half_of_key_pointer_to_buffer(leaf_block.get_block_buffer());
        // copia as chaves (meio, fim) para o novo no ajustando o header de qtd chaves
        aux_block.leaf_copy_second_half_of_key_pointer_to_buffer(new_leaf.get_block_buffer());

        aux_block.free();

        // pega o menor valor do novo folha
        unsigned int first_key = new_leaf.get_first_key();
        // insere esse valor no pai
        insert_in_parent(&path_to_leaf, first_key, new_leaf_index);

        _tree_file.write_block(leaf_block.get_block_buffer(), leaf_index);
        _tree_file.write_block(new_leaf.get_block_buffer(), new_leaf_index);
        _tree_file.write_block(_header_block.get_block_buffer(), 0);
    }
}

void BPTree::insert_in_parent(std::stack<unsigned int>* path_to_leaf, unsigned int key,
                              unsigned int data_file_block_index) {
    // se for raiz
    if (path_to_leaf->empty()) {
        unsigned int old_root_index = _header_block.get_root_node_index();
        // cria uma nova raiz com parent, key e data_file_block_index
        BPTreeInternalBlock new_root;
        unsigned int new_root_index = _header_block.get_next_free_block_and_point_to_next();

        new_root.insert_key_for_root(old_root_index, key, data_file_block_index);

        // ajusta o header de nova raiz
        _header_block.set_new_root_index(new_root_index);
        _tree_file.write_block(new_root.get_block_buffer(), new_root_index);
        new_root.free();
    } else {  // se nao, eh interno
        // pega o pai de node
        unsigned int parent_index = path_to_leaf->top();
        path_to_leaf->pop();
        unsigned char* parent_block = _tree_file.read_block(parent_index);
        BPTreeInternalBlock parent_node(parent_block);

        // se o pai tiver espaco para inserir
        if (parent_node.are_there_free_slots()) {
            // insere ordenado
            parent_node.insert_key(key, data_file_block_index);
            _tree_file.write_block(parent_node.get_block_buffer(), parent_index);
        } else {  // se nao, precisa fazer split
            // cria um novo no interno
            BPTreeInternalBlock new_internal;
            unsigned int new_internal_index = _header_block.get_next_free_block_and_point_to_next();

            // cria um bloco auxiliar que tem espaco para todas as chaves do parent + o novo par (ptr,chave) a ser
            // inserido
            AuxiliarBlock aux_block(BLOCK_SIZE - BPTREE_HEADER_SIZE + BPTREE_INDEX_VALUE_PAIR_SIZE);
            aux_block.copy_all_key_pointer((char*)parent_block, BLOCK_SIZE);

            // insere key nesse bloco
            aux_block.insert_internal_key_pointer(key, data_file_block_index);  // fazer o pra interno

            // copia as chaves (inicio,meio) para o parent
            aux_block.internal_copy_first_half_of_key_pointer_to_buffer(parent_node.get_block_buffer());
            // copia as chaves (meio,fim) para o novo no interno
            aux_block.internal_copy_second_half_of_key_pointer_to_buffer(new_internal.get_block_buffer());

            // pega a menor chave do novo nodo interno
            unsigned int first_key = new_internal.get_first_key();

            // insere a menor chave no pai recursivamente
            insert_in_parent(path_to_leaf, key, new_internal_index);

            _tree_file.write_block(parent_node.get_block_buffer(), parent_index);
            _tree_file.write_block(new_internal.get_block_buffer(), new_internal_index);
            parent_node.free();
        }
    }

    _tree_file.write_block(_header_block.get_block_buffer(), 0);
}

bool BPTree::is_leaf(unsigned char* block_buffer) {
    unsigned short int is_leaf = read_2byte_number_from_buffer(block_buffer, 0);

    if (is_leaf == 1) return true;

    return false;
}

void BPTree::create_tree_file(const char* filename) {
    // criar o arquivo da arvore
    _tree_file.create_or_rewrite(filename);
    _tree_file.zero_the_file_out();

    char* root = new char[BLOCK_SIZE];
    write_2byte_number_to_buffer(root, 1, 0);  // a raiz eh uma folha no inicio

    _tree_file.write_block(_header_block.get_block_buffer(), 0);
    _tree_file.write_block(root, 1);
    delete[] root;
}

void BPTree::read_tree_file(const char* filename) {
    _tree_file.open_as_readonly(filename);
    _header_block = BPTreeHeaderBlock(_tree_file.read_block(0));
}

void BPTree::close_tree_file() {
    _tree_file.close();
}

unsigned int BPTree::get_qt_blocks() {
    return _header_block.get_qt_blocks();
}