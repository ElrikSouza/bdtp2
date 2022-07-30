#include "primary-index.h"
#include "../paper-block.h"

void PrimaryIndex::create_and_open_for_writing(const char* filename) {
    _bptree = BPTree();
    
    _bptree.create_tree_file(filename);
}

void PrimaryIndex::open_for_reading(const char* filename) {
    _bptree.read_tree_file(filename);
}
void PrimaryIndex::close() {
    _bptree.close_tree_file();
}

void PrimaryIndex::insert_id(unsigned int id, unsigned int data_file_block_index) {
    _bptree.insert_key(id, data_file_block_index);
}

Paper* PrimaryIndex::get_paper_by_id(HashFile* hash_file, unsigned int id) {
    std::stack<unsigned int> path_to_leaf = _bptree.get_path_to_leaf(id);
    unsigned int leaf_index = path_to_leaf.top();
    _bptree.read_tree_file(PRIMARY_INDEX_NAME);

    std::cout << "LEAF INDEX = " << leaf_index << std::endl;

    unsigned char *block_buffer = hash_file->read_block(leaf_index);
    PaperBlock paper_block(block_buffer);
    Paper* paper = paper_block.get_paper_if_it_is_inside(id);

    delete block_buffer;

    return paper;
}

unsigned int PrimaryIndex::qt_blocks_in_index() {
    return _bptree.get_qt_blocks();
}