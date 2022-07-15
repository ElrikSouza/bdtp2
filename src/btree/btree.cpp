#include "btree.h"

#include <fstream>
#include <stack>

#include "../binary-file.h"

std::stack<unsigned int> BPTree::get_path_to_leaf(unsigned int key) {
    // get the root node index
    // load the root node

    // if it is a leaf, return the root index

    // else
    // create a internal block object
    // get the matching pointer from the internal block object
    // repeat until a leaf is found
    // return the leaf
}

void BPTree::create_tree_file(const char* filename) {
    _tree_file.create_or_rewrite(filename);
}

void BPTree::read_tree_file(const char* filename) {
    _tree_file.open_as_readonly(filename);
    _header_block = BPTreeHeaderBlock(_tree_file.read_block(0));
}