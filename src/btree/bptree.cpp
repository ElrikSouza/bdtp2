#include "bptree.h"

#include <fstream>
#include <stack>

#include "../binary-file.h"
#include "../buffer/buffer-utils.h"
#include "bptree-header-block.h"
#include "bptree-internal-block.h"
#include "bptree-leaf-block.h"
#include "bptree-root-block.h"
#include "auxiliar-block.h"

#define BLOCK_SIZE 4096

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
    // std::cout << root_index << std::endl;
    unsigned char* root_node_buffer = _tree_file.read_block(root_index);

    if (key == 130562) {
        std::cout << "rrot" << root_index << std::endl;
    }

    path.push(root_index);

    std::cout << ">>>>>>>>>>>>>>>>" << std::endl;

    if (is_leaf(root_node_buffer)) {
        std::cout << "A RAIZ EH FOLHA" << std::endl;
        return path;
    }

    BPTreeInternalBlock internal_block(root_node_buffer, 4096);

    // std::cout << "Antes" << std::endl;
    while (1) {
        // if (key == 130562) {
        //     std::cout << "b" << std::endl;
        // }
        unsigned int matching_pointer = internal_block.get_matching_pointer(key);

        // std::cout << "Dentro" << matching_pointer << std::endl;
        // break;
        path.push(matching_pointer);

        unsigned char* next_block_buffer = _tree_file.read_block(matching_pointer);

        if (key == 130512 && matching_pointer != 0) {
            std::cout << "pointer=" << matching_pointer << std::endl;

            for (int i = 12; i <= 4092; i += 4) {
                std::cout << ", " << read_4byte_number_from_buffer(next_block_buffer, i);
            }

            std::cout << std::endl;
        }

        // if (key > 130500 && matching_pointer != 516) {
        //     std::cout << "pointer=" << matching_pointer << std::endl;

        //     for (int i = 12; i <= 4092; i += 4) {
        //         std::cout << ", " << read_4byte_number_from_buffer(next_block_buffer, i);
        //     }

        //     std::cout << std::endl;
        // }

        // if (key == 130512 && matching_pointer == 514) {
        //     std::cout << read_4byte_number_from_buffer(next_block_buffer, 12) << std::endl;
        //     std::cout << read_4byte_number_from_buffer(next_block_buffer, 4092) << std::endl;
        // }

        // if (key == 130562 && matching_pointer == 512) {
        //     std::cout << read_4byte_number_from_buffer(next_block_buffer, 12) << std::endl;
        //     std::cout << read_4byte_number_from_buffer(next_block_buffer, 4092) << std::endl;
        // }

        if (is_leaf(next_block_buffer)) {
            delete next_block_buffer;
            return path;
        }

        internal_block.free();
        internal_block = BPTreeInternalBlock(next_block_buffer, 4096);
    }

    return path;
}

unsigned int BPTree::get_data_pointer(unsigned int key) {
    auto path = get_path_to_leaf(key);
    unsigned int leaf_index = path.top();
    Buffer b(_tree_file.read_block(leaf_index), 4096);
    b.jump_n_bytes_from_current_position(12);

    unsigned int current_key;
    unsigned int current_pointer;
    while (b.get_current_cursor_position() <= 4096 - 12) {
        current_pointer = b.read_4byte_number();
        current_key = b.read_4byte_number();
        // std::cout << current_key << std::endl;
        // std::cout << "read=(" << current_key << "," << current_pointer << std::endl;

        if (current_key == key) {
            // std::cout << "found" << std::endl;
            return current_pointer;
        }

        if (current_key == 0) {
            // std::cout << "not found" << std::endl;
            return 0;
        }
    }

    // std::cout << "not found" << std::endl;
    return 0;
}

// void BPTree::insert_key(unsigned int key, unsigned int data_file_index) {
//     std::stack<unsigned int> path_to_leaf = get_path_to_leaf(key);

//     unsigned int leaf_index = path_to_leaf.top();
//     path_to_leaf.pop();

//     unsigned char* leaf_block_buffer = _tree_file.read_block(leaf_index);

//     BPTreeLeafBlock leaf(leaf_block_buffer);

//     // happy path onde a folha tem espaço
//     if (leaf.are_there_free_slots()) {
//         leaf.insert_key(key, data_file_index);
//         _tree_file.write_block((char*)leaf.get_block_buffer(), leaf_index);
//         leaf.free();

//         return;
//     }

//     // criar nova folha para split
//     BPTreeLeafBlock new_leaf;
//     unsigned int new_leaf_index = _header_block.get_next_free_block_and_point_to_next();
//     leaf.transfer_data_and_pointers_to_split_node(&new_leaf, new_leaf_index);

//     // vamos decidir onde colocar a nova chave
//     unsigned int new_leaf_first_key = new_leaf.get_first_key();

//     if (key < new_leaf_first_key) {
//         leaf.insert_key(key, data_file_index);
//     } else {
//         new_leaf.insert_key(key, data_file_index);
//     }

//     _tree_file.write_block((char*)leaf.get_block_buffer(), leaf_index);
//     _tree_file.write_block((char*)new_leaf.get_block_buffer(), new_leaf_index);

//     leaf.free();
//     new_leaf.free();

//     // vamos pegar de novo a primeira chave da nova folha para promver (esse valor pode ter mudado)
//     unsigned int promoted_key = new_leaf.get_first_key();
//     // std::cout << "[debug]: promote" << new_leaf_first_key << std::endl;
//     unsigned int left_node_index = leaf_index;
//     unsigned int right_node_index = new_leaf_index;
//     unsigned int parent_index;

//     while (1) {
//         // chegamos na raiz e mesmo assim n tinha espaço
//         if (path_to_leaf.empty()) {
//             std::cout << "new root";
//             BPTreeRootBlock new_root;
//             unsigned int new_root_index = _header_block.get_next_free_block_and_point_to_next();
//             new_root.insert_key(promoted_key, left_node_index, right_node_index);
//             _header_block.set_new_root_index(new_root_index);

//             _tree_file.write_block(new_root.get_block_buffer(), new_root_index);
//             new_root.free();
//             break;
//         }

//         parent_index = path_to_leaf.top();
//         path_to_leaf.pop();

//         // std::cout << "[debug]: insert key in parent = " << parent_index << std::endl;
//         BPTreeInternalBlock parent(_tree_file.read_block(parent_index));
//         if (parent.are_there_free_slots()) {
//             parent.insert_key(promoted_key, new_leaf_index);
//             _tree_file.write_block(parent.get_block_buffer(), parent_index);
//             parent.free();
//             break;
//         }

//         std::cout << "PARENT IS FULL!!!!!!!!!!!!!!!!!!!!!!" << std::endl;

//         BPTreeInternalBlock split_internal;
//         left_node_index = parent_index;
//         right_node_index = _header_block.get_next_free_block_and_point_to_next();

//         promoted_key = parent.transfer_data_and_pointers_to_split_node(&split_internal, right_node_index, promoted_key);

//         _tree_file.write_block(split_internal.get_block_buffer(), right_node_index);
//         _tree_file.write_block(parent.get_block_buffer(), parent_index);

//         split_internal.free();
//         parent.free();

//         // std::cout << "stuck" << std::endl;
//     }

//     commit_header();
//     // std::cout << "\ncommit" << key << "\n";
// }

void BPTree::insert_key(unsigned int key, unsigned int data_file_block_index) {
    // encontra a folha
    std::stack<unsigned int> path_to_leaf = get_path_to_leaf(key);
    unsigned int leaf_block_index = path_to_leaf.top();
    std::cout << "folha" << leaf_block_index << std::endl;
    path_to_leaf.pop();
    unsigned char* leaf_block_buffer = _tree_file.read_block(leaf_block_index);
    BPTreeLeafBlock leaf_block(leaf_block_buffer, BLOCK_SIZE);

    for (int i = 12; i < 4092; i += 8) {
        std::cout << "index = " << read_4byte_number_from_buffer(leaf_block_buffer, i) << " key = " << read_4byte_number_from_buffer(leaf_block_buffer, i + 4) << std::endl;
    }

    // se a folha tem espaco, insere
    if (leaf_block.are_there_free_slots()) {
        std::cout << "TEM ESPACO" << std::endl;
        std::cout << "CHAVE = " << key << " BLOCK_INDEX = " << data_file_block_index << std::endl;

        leaf_block.insert_key(key, data_file_block_index);
        _tree_file.write_block(leaf_block.get_block_buffer(), leaf_block_index);
    } else { // se nao, faz o split
        std::cout << "NAO TEM ESPACO" << std::endl;
        // cria um novo folha
        BPTreeLeafBlock new_leaf(BLOCK_SIZE);
        unsigned int new_leaf_index = _header_block.get_next_free_block_and_point_to_next();

        // cria um bloco auxiliar que tem espaco para todas as chaves do no + o novo par (ptr,chave) a ser inserido
        // AuxiliarBlock aux_block(leaf_block.get_block_buffer(), BLOCK_SIZE + 8);
        // BPTreeLeafBlock* aux_leaf = aux_block.deserialize_into_leaf_block();
        AuxiliarBlock aux_block(BLOCK_SIZE - 12 + 4);

        aux_block.copy_all_key_pointer((char*) leaf_block.get_block_buffer(), BLOCK_SIZE);

        // insere a chave nesse bloco
        // aux_leaf->insert_key(key, data_file_block_index);
        aux_block.insert_key_pointer(key, data_file_block_index);

        // corrige os ponteiros da lista encadeada
        unsigned int old_next_block = leaf_block.point_to_new_block(new_leaf_index);
        new_leaf.point_to_new_block(old_next_block);
        // copia as chaves (inicio, meio) para o no atual ajustando o header de qtd chaves
        // aux_leaf->transfer_first_half_of_keys_and_pointers(&leaf_block);
        aux_block.copy_first_half_of_key_pointer_to_buffer(leaf_block.get_block_buffer());
        // copia as chaves (meio, fim) para o novo no ajustando o header de qtd chaves
        // aux_leaf->transfer_second_half_of_keys_and_pointers(&new_leaf);
        aux_block.copy_second_half_of_key_pointer_to_buffer(new_leaf.get_block_buffer());

        // pega o menor valor do novo folha
        unsigned int first_key = new_leaf.get_first_key();
        // insere esse valor no pai
        std::cout << "leaf index = " << leaf_block_index << std::endl;
        std::cout << ">>>ANTES" << std::endl;
        insert_in_parent(&path_to_leaf, (unsigned char*) leaf_block.get_block_buffer(), first_key, new_leaf_index);
        std::cout << ">>>DEPOIS" << std::endl;

        _tree_file.write_block(leaf_block.get_block_buffer(), leaf_block_index);
        _tree_file.write_block(new_leaf.get_block_buffer(), new_leaf_index);
    }
    delete[] leaf_block_buffer;
}

void BPTree::insert_in_leaf(unsigned char* leaf, unsigned int key, unsigned int data_file_block_index) {
    // insere ordenado
}

void BPTree::insert_in_parent(std::stack<unsigned int>* path_to_leaf, unsigned char* node, unsigned int key, unsigned int data_file_block_index) {
    // se for raiz
    if (path_to_leaf->empty()) {
        std::cout << "EH RAIZ" << std::endl;
        // std::stack<unsigned int> copy_of_path_to_leaf = get_path_to_leaf(key);
        unsigned int old_root_index = _header_block.get_root_node_index();
        std::cout << "old root index = " << old_root_index << std::endl;
        // cria uma nova raiz com parent, key e data_file_block_index
        std::cout << ">>>ANTES" << std::endl;
        BPTreeInternalBlock new_root(BLOCK_SIZE);
        std::cout << ">>>DEPOIS" << std::endl;
        unsigned int new_root_index = _header_block.get_next_free_block_and_point_to_next();
        std::cout << "new root index = " << new_root_index << std::endl;

        new_root.insert_key_for_root(old_root_index, key, data_file_block_index);

        // ajusta o header de nova raiz
        _header_block.set_new_root_index(new_root_index);
        _tree_file.write_block(new_root.get_block_buffer(), new_root_index);
    } else { // se nao, eh interno
        // pega o pai de node
        unsigned int parent_index = path_to_leaf->top();
        path_to_leaf->pop();
        unsigned char* parent_block = _tree_file.read_block(parent_index);
        BPTreeInternalBlock parent_node(parent_block, BLOCK_SIZE);

        // se o pai tiver espaco para inserir
        if (parent_node.are_there_free_slots()) {
            // insere ordenado
            parent_node.insert_key(key, data_file_block_index);
            _tree_file.write_block(parent_node.get_block_buffer(), parent_index);
        } else { // se nao, precisa fazer split
            // cria um novo no interno
            BPTreeInternalBlock new_internal(BLOCK_SIZE);
            unsigned int new_internal_index = _header_block.get_next_free_block_and_point_to_next();
            
            // cria um bloco auxiliar que tem espaco para todas as chaves do parent + o novo par (ptr,chave) a ser inserido
            // AuxiliarBlock aux_block(parent_node.get_block_buffer(), BLOCK_SIZE + 8);
            AuxiliarBlock aux_block(BLOCK_SIZE - 12 + 4);
            // BPTreeInternalBlock* aux_internal = aux_block.deserialize_into_internal_block();
            aux_block.copy_all_key_pointer((char*) node, BLOCK_SIZE);

            // insere key nesse bloco
            // aux_internal->insert_key(key, data_file_block_index);
            aux_block.insert_key_pointer(key, data_file_block_index);
            // copia as chaves (inicio,meio) para o parent
            // aux_internal->transfer_first_half_of_keys_and_pointers(&parent_node);
            aux_block.copy_first_half_of_key_pointer_to_buffer(parent_node.get_block_buffer());
            // copia as chaves (meio,fim) para o novo no interno
            // aux_internal->transfer_second_half_of_keys_and_pointers(&new_internal);
            aux_block.copy_second_half_of_key_pointer_to_buffer(new_internal.get_block_buffer());
            // pega a menor chave do novo no interno
            unsigned int first_key = new_internal.get_first_key();
            // insere a menor chave no pai recursivamente
            insert_in_parent(path_to_leaf, (unsigned char *) parent_node.get_block_buffer(), key, new_internal_index);

            // delete aux_internal;
            // delete aux_block;

            _tree_file.write_block(parent_node.get_block_buffer(), parent_index);
            _tree_file.write_block(new_internal.get_block_buffer(), new_internal_index);
        }
    }
}

void BPTree::commit_header() {
    _tree_file.write_block(_header_block.get_block_buffer(), 0);
}

// void BPTree::insert_key(unsigned int key, unsigned int data_file_block_index) {
//     std::stack<unsigned int> path_to_leaf = get_path_to_leaf(key);

//     unsigned int leaf_index = path_to_leaf.top();
//     path_to_leaf.pop();

//     unsigned char* leaf_block_buffer = _tree_file.read_block(leaf_index);

//     BPTreeLeafBlock leaf(leaf_block_buffer);

//     if (leaf.are_there_free_slots()) {
//         leaf.insert_key(key, data_file_block_index);
//         _tree_file.write_block((char*)leaf.get_block_buffer(), leaf_index);

//         return;
//     }

//     unsigned int middle_key = leaf.get_middle_key();
//     unsigned int next_free_block = _header_block.get_next_free_block_and_point_to_next();
//     BPTreeLeafBlock new_split_leaf;
//     leaf.transfer_data_and_pointers_to_split_node(&new_split_leaf, next_free_block);
//     _tree_file.write_block((char*)leaf.get_block_buffer(), leaf_index);
//     _tree_file.write_block((char*)new_split_leaf.get_block_buffer(), next_free_block);

//     unsigned int parent_index = leaf_index;

//     while (1 == 1) {
//         if (path_to_leaf.empty()) {
//             BPTreeRootBlock new_root;
//             unsigned int new_root_index = _header_block.get_next_free_block_and_point_to_next();
//             new_root.insert_key(key, parent_index, next_free_block);

//             _tree_file.write_block((char*)new_root.get_block_buffer(), new_root_index);
//             _header_block.set_new_root_index(new_root_index);

//             _tree_file.write_block((char*)_header_block.get_block_buffer(), 0);

//             return;
//         }

//         parent_index = path_to_leaf.top();
//         path_to_leaf.pop();

//         unsigned char* parent_block_buffer = _tree_file.read_block(parent_index);
//         BPTreeInternalBlock parent(parent_block_buffer);

//         if (parent.are_there_free_slots()) {
//             parent.insert_key(key, next_free_block);
//             _tree_file.write_block((char*)parent.get_block_buffer(), parent_index);
//             return;
//         }

//         middle_key = parent.get_middle_key();
//         next_free_block = _header_block.get_next_free_block_and_point_to_next();
//         BPTreeInternalBlock new_split_internal_block;
//         parent.transfer_data_and_pointers_to_split_node(&new_split_internal_block, next_free_block);
//     }
// }

bool BPTree::is_leaf(unsigned char* block_buffer) {
    unsigned short int is_leaf = read_2byte_number_from_buffer(block_buffer, 0);

    if (is_leaf == 1) return true;

    return false;
}

void BPTree::create_tree_file(const char* filename) {
    // criar o arquivo da arvore
    _tree_file.create_or_rewrite(filename);
    _tree_file.zero_the_file_out();

    char* root = new char[4096];
    write_2byte_number_to_buffer(root, 1, 0); // a raiz eh uma folha no inicio

    _tree_file.write_block(_header_block.get_block_buffer(), 0);
    _tree_file.write_block(root, 1);
    delete[] root;
}

void BPTree::read_tree_file(const char* filename) {
    _tree_file.open_as_readonly(filename);
    _header_block = BPTreeHeaderBlock(_tree_file.read_block(0));
}