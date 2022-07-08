#include <fstream>
#include <iostream>

#include "binary-file.cpp"
#include "paper-block.cpp"
#include "paper.cpp"

class HashFile {
   private:
    BinaryFile _bin_file;
    int _blocks_per_bucket;
    int _number_of_buckets;

    int hash_paper(unsigned int paper_id);

   public:
    HashFile(int number_of_buckets, int blocks_per_bucket);

    void open_file_for_reading(const char* filename);
    void create_and_open_for_writing(const char* filename);
    void insert_paper(Paper* paper);
    Paper* get_paper_by_id(unsigned int id);
};

HashFile::HashFile(int number_of_buckets, int blocks_per_bucket) {
    _blocks_per_bucket = blocks_per_bucket;
    _number_of_buckets = number_of_buckets;

    int total_num_of_blocks = number_of_buckets * blocks_per_bucket;
    _bin_file = BinaryFile(total_num_of_blocks, 4096);
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
            break;
        }

        delete block_buffer;
    }
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
    }

    return paper;
}