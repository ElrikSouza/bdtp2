#include "btree.h"

#include <fstream>
#include <stack>

#include "../binary-file.h"

std::stack<unsigned int> BPTree::get_path_to_leaf(unsigned int key) {
    std::stack<unsigned int> path;

    unsigned int root_index = get_root_node();
    unsigned char* root_node_buffer = _tree_file.read_block(root_index);

    path.push(root_index);

    if (is_leaf(root_node_buffer)) {
        return path;
    }

    BPTreeInternalBlock internal_block(root_node_buffer);

    while (1 == 1) {
        unsigned int matching_pointer = internal_block.get_matching_pointer(key);
        path.push(matching_pointer);

        unsigned char* next_block_buffer = _tree_file.read_block(matching_pointer);

        if (is_leaf(next_block_buffer)) {
            return path;
        }

        internal_block = BPTreeInternalBlock(next_block_buffer);
    }
}

bool BPTree::insert_key(unsigned int key, unsigned int data_file_block_index) {
    std::stack<unsigned int> path_to_leaf = get_path_to_leaf(key);

    unsigned int leaf_index = path_to_leaf.top();
    path_to_leaf.pop();

    unsigned char* leaf_block_buffer = _tree_file.read_block(leaf_index);

    BPTreeLeafBlock leaf(leaf_block_buffer);

    if (leaf.are_there_free_slots()) {
        leaf.insert_key(key, data_file_block_index);
        _tree_file.write_block((char*)leaf.get_block_buffer(), leaf_index);

        return true;
    }

    //

    unsigned int middle_key = leaf.get_middle_key();
    unsigned int next_free_block = get_next_free_block_and_point_to_next();
    BPTreeLeafBlock new_split_leaf;
    leaf.transfer_data_and_pointers_to_split_node(&new_split_leaf, next_free_block);
    _tree_file.write_block((char*)leaf.get_block_buffer(), leaf_index);
    _tree_file.write_block((char*)new_split_leaf.get_block_buffer(), next_free_block);

    unsigned int parent_index = leaf_index;

    while (1 == 1) {
        if (path_to_leaf.empty()) {
            BPTreeRootBlock new_root;
            unsigned int new_root_index = get_next_free_block_and_point_to_next();
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
        next_free_block = get_next_free_block_and_point_to_next();
        BPTreeInternalBlock new_split_internal_block;
        parent.transfer_data_and_pointers_to_split_node(&new_split_internal_block, next_free_block);
    }
}

// 32 bits 16tb

// & 0x80 1000 0000 == 0x80
// 1 interno, 0 = 0

void BPTree::create_tree_file(const char* filename) {
    _tree_file.create_or_rewrite(filename);
}

void BPTree::read_tree_file(const char* filename) {
    _tree_file.open_as_readonly(filename);
    _header_block = BPTreeHeaderBlock(_tree_file.read_block(0));
}