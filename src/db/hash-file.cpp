#include "hash-file.h"

#include <fstream>
#include <iostream>

#include "../binary-file.h"
#include "../paper-block.h"
#include "../paper.h"

HashFile::HashFile(int number_of_buckets, int blocks_per_bucket) {
    _blocks_per_bucket = blocks_per_bucket;
    _number_of_buckets = number_of_buckets;

    int total_num_of_blocks = number_of_buckets * blocks_per_bucket;
    _bin_file = BinaryFile(total_num_of_blocks, 4096);
}

int HashFile::hash_paper(unsigned int paper_id) {
    return paper_id % _number_of_buckets;
}

void HashFile::close() {
    _bin_file.close();
}

void HashFile::create_and_open_for_writing(const char* filename) {
    _bin_file.create_or_rewrite(filename);
    _bin_file.zero_the_file_out();
}

void HashFile::open_file_for_reading(const char* filename) {
    _bin_file.open_as_readonly(filename);
}

void HashFile::insert_paper(Paper* paper) {
    int bucket_number = hash_paper(paper->id);
    int current_block;
    unsigned char* block_buffer;

    for (int i = 0; i < _blocks_per_bucket; i++) {
        current_block = bucket_number * _blocks_per_bucket + i;
        block_buffer = _bin_file.read_block(current_block);
        PaperBlock block(block_buffer);
        bool didTheInsertionSucceed = block.insert_paper(paper);

        if (didTheInsertionSucceed) {
            _bin_file.write_block(block.get_block_buffer(), current_block);
            delete block_buffer;
            return;
        }

        delete block_buffer;
    }

    std::cout << "sem espaco" << std::endl;
}

Paper* HashFile::get_paper_by_id(unsigned int paper_id) {
    int bucket_number = hash_paper(paper_id);
    int current_block;
    unsigned char* block_buffer;
    Paper* paper = nullptr;

    for (int i = 0; i < _blocks_per_bucket; i++) {
        current_block = bucket_number * _blocks_per_bucket + i;
        block_buffer = _bin_file.read_block(current_block);
        PaperBlock block(block_buffer);

        paper = block.get_paper_if_it_is_inside(paper_id);

        if (paper != nullptr) {
            delete block_buffer;
            break;
        }

        delete block_buffer;
        // std::cout << "crash" << std::endl;
    }

    return paper;
}