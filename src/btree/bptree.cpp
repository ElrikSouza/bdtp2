#include "bptree.h"

#include <fstream>
#include <stack>

#include "../binary-file.h"
#include "../buffer/buffer-utils.h"
#include "bptree-header-block.h"
#include "bptree-internal-block.h"
#include "bptree-leaf-block.h"
#include "bptree-root-block.h"

BPTree::BPTree() {
    _tree_file = BinaryFile(100, 4096);
    _header_block = BPTreeHeaderBlock();
}

unsigned int BPTree::get_root_node() {
    return _header_block.get_root_node_index();
}

std::stack<unsigned int> BPTree::get_path_to_leaf(unsigned int key) {
    std::stack<unsigned int> path;

    unsigned int root_index = get_root_node();
    // std::cout << root_index << std::endl;
    // std::cout << root_index << std::endl;
    unsigned char* root_node_buffer = _tree_file.read_block(root_index);

    path.push(root_index);

    if (is_leaf(root_node_buffer)) {
        // std::cout << "leaf" << std::endl;
        return path;
    }

    BPTreeInternalBlock internal_block(root_node_buffer);

    // std::cout << "Antes" << std::endl;
    while (1) {
        unsigned int matching_pointer = internal_block.get_matching_pointer(key);
        // std::cout << "Dentro" << matching_pointer << std::endl;
        // break;
        path.push(matching_pointer);

        unsigned char* next_block_buffer = _tree_file.read_block(matching_pointer);

        if (is_leaf(next_block_buffer)) {
            return path;
        }

        internal_block = BPTreeInternalBlock(next_block_buffer);
    }

    return path;
}

void BPTree::insert_key(unsigned int key, unsigned int data_file_index) {
    if (key == 511) {
        std::cout << "hmm";
    }
    std::stack<unsigned int> path_to_leaf = get_path_to_leaf(key);

    unsigned int leaf_index = path_to_leaf.top();
    path_to_leaf.pop();

    unsigned char* leaf_block_buffer = _tree_file.read_block(leaf_index);

    BPTreeLeafBlock leaf(leaf_block_buffer);

    // happy path onde a folha tem espaço
    if (leaf.are_there_free_slots()) {
        leaf.insert_key(key, data_file_index);
        _tree_file.write_block((char*)leaf.get_block_buffer(), leaf_index);

        return;
    }

    // criar nova folha para split
    BPTreeLeafBlock new_leaf;
    unsigned int new_leaf_index = _header_block.get_next_free_block_and_point_to_next();
    leaf.transfer_data_and_pointers_to_split_node(&new_leaf, new_leaf_index);

    // vamos decidir onde colocar a nova chave
    unsigned int new_leaf_first_key = new_leaf.get_first_key();

    if (key < new_leaf_first_key) {
        leaf.insert_key(key, data_file_index);
    } else {
        new_leaf.insert_key(key, data_file_index);
    }

    _tree_file.write_block((char*)leaf.get_block_buffer(), leaf_index);
    _tree_file.write_block((char*)new_leaf.get_block_buffer(), new_leaf_index);

    // vamos pegar de novo a primeira chave da nova folha para promver (esse valor pode ter mudado)
    unsigned int promoted_key = new_leaf.get_first_key();
    std::cout << "[debug]: promote" << new_leaf_first_key << std::endl;
    unsigned int left_node_index = leaf_index;
    unsigned int right_node_index = new_leaf_index;
    unsigned int parent_index;

    while (1) {
        // chegamos na raiz e mesmo assim n tinha espaço
        if (path_to_leaf.empty()) {
            BPTreeRootBlock new_root;
            unsigned int new_root_index = _header_block.get_next_free_block_and_point_to_next();
            new_root.insert_key(promoted_key, left_node_index, right_node_index);
            _header_block.set_new_root_index(new_root_index);

            _tree_file.write_block(new_root.get_block_buffer(), new_root_index);
            break;
        }

        parent_index = path_to_leaf.top();
        path_to_leaf.pop();

        std::cout << "[debug]: insert key in parent = " << parent_index << std::endl;
        BPTreeInternalBlock parent(_tree_file.read_block(parent_index));
        parent.insert_key(promoted_key, new_leaf_index);
        _tree_file.write_block(parent.get_block_buffer(), parent_index);

        break;
    }

    commit_header();
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

    char* first_root = new char[4096];
    write_2byte_number_to_buffer(first_root, 1, 0);

    _tree_file.zero_the_file_out();

    _tree_file.write_block(_header_block.get_block_buffer(), 0);
    _tree_file.write_block(first_root, 1);
}

void BPTree::read_tree_file(const char* filename) {
    _tree_file.open_as_readonly(filename);
    _header_block = BPTreeHeaderBlock(_tree_file.read_block(0));
}