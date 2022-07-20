#include "bptree.h"
#include "bptree-header-block.h"
#include "bptree-internal-block.h"
#include "bptree-leaf-block.h"
#include "bptree-root-block.h"

#include <fstream>
#include <stack>

#include "../binary-file.h"
#include "../buffer/buffer-utils.h"

BPTree::BPTree(){
    _tree_file = BinaryFile(10000, 4096);
    _header_block = BPTreeHeaderBlock();
    BPTreeLeafBlock root;
    unsigned int root_block = _header_block.get_next_free_block_and_point_to_next();
    _header_block.set_new_root_index(root_block);
    // std::cout << "root = " << root_block << std::endl;

    _tree_file.write_block(_header_block.get_block_buffer(), _header_block.get_next_free_block_and_point_to_next());
    _tree_file.write_block(root.get_block_buffer(), root_block);

    // unsigned char* aa = _tree_file.read_block(root_block);
    // unsigned int bb = read_2byte_number_from_buffer(aa, 0);
    // std::cout << "bb = " << bb << std::endl;
}

unsigned int BPTree::get_root_node() {
    return _header_block.get_root_node_index();
}

std::stack<unsigned int> BPTree::get_path_to_leaf(unsigned int key) {
    std::stack<unsigned int> path;

    unsigned int root_index = get_root_node();
    unsigned char* root_node_buffer = _tree_file.read_block(root_index);

    // bool a = is_leaf(root_node_buffer);
    // unsigned short int aaa = read_2byte_number_from_buffer(root_node_buffer, 0);
    // std::cout << "root = " << root_index << std::endl;
    // std::cout << a << std::endl;
    // std::cout << aaa << std::endl;

    path.push(root_index);

    if (is_leaf(root_node_buffer)) {
        return path;
    }

    BPTreeInternalBlock internal_block(root_node_buffer);

    // std::cout << "Antes" << std::endl;
    while (1 == 1) {
        // std::cout << "Dentro" << std::endl;
        unsigned int matching_pointer = internal_block.get_matching_pointer(key);
        path.push(matching_pointer);

        unsigned char* next_block_buffer = _tree_file.read_block(matching_pointer);

        if (is_leaf(next_block_buffer)) {
            return path;
        }

        internal_block = BPTreeInternalBlock(next_block_buffer);
    }
}

void BPTree::insert_key(unsigned int key, unsigned int data_file_block_index) {
    std::stack<unsigned int> path_to_leaf = get_path_to_leaf(key);

    unsigned int leaf_index = path_to_leaf.top();
    path_to_leaf.pop();

    unsigned char* leaf_block_buffer = _tree_file.read_block(leaf_index);

    BPTreeLeafBlock leaf(leaf_block_buffer);

    if (leaf.are_there_free_slots()) {
        leaf.insert_key(key, data_file_block_index);
        _tree_file.write_block((char*)leaf.get_block_buffer(), leaf_index);

        return;
    }

    unsigned int middle_key = leaf.get_middle_key();
    unsigned int next_free_block = _header_block.get_next_free_block_and_point_to_next();
    BPTreeLeafBlock new_split_leaf;
    leaf.transfer_data_and_pointers_to_split_node(&new_split_leaf, next_free_block);
    _tree_file.write_block((char*)leaf.get_block_buffer(), leaf_index);
    _tree_file.write_block((char*)new_split_leaf.get_block_buffer(), next_free_block);

    unsigned int parent_index = leaf_index;

    while (1 == 1) {
        if (path_to_leaf.empty()) {
            BPTreeRootBlock new_root;
            unsigned int new_root_index = _header_block.get_next_free_block_and_point_to_next();
            new_root.insert_key(key, parent_index, next_free_block);

            _tree_file.write_block((char*)new_root.get_block_buffer(), new_root_index);
            _header_block.set_new_root_index(new_root_index);

            _tree_file.write_block((char*)_header_block.get_block_buffer(), 0);

            return;
        }

        parent_index = path_to_leaf.top();
        path_to_leaf.pop();

        unsigned char* parent_block_buffer = _tree_file.read_block(parent_index);
        BPTreeInternalBlock parent(parent_block_buffer);

        if (parent.are_there_free_slots()) {
            parent.insert_key(key, next_free_block);
            _tree_file.write_block((char*)parent.get_block_buffer(), parent_index);
            return;
        }

        middle_key = parent.get_middle_key();
        next_free_block = _header_block.get_next_free_block_and_point_to_next();
        BPTreeInternalBlock new_split_internal_block;
        parent.transfer_data_and_pointers_to_split_node(&new_split_internal_block, next_free_block);
    }
}

bool BPTree::is_leaf(unsigned char* block_buffer) {
    unsigned short int is_leaf = read_2byte_number_from_buffer(block_buffer, 0);

    if (is_leaf == 1) return true;

    return false;
}

// 32 bits 16tb

// & 0x80 1000 0000 == 0x80
// 1 interno, 0 = 0

void BPTree::create_tree_file(const char* filename) {
    _tree_file.create_or_rewrite(filename);
    _tree_file.zero_the_file_out();
}

void BPTree::read_tree_file(const char* filename) {
    _tree_file.open_as_readonly(filename);
    _header_block = BPTreeHeaderBlock(_tree_file.read_block(0));
}