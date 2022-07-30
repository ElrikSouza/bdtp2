#include "../btree/bptree.h"
#include "../paper.h"
#include "hash-file.h"

class PrimaryIndex {
    private:
        BPTree _bptree;

    public:
        void create_and_open_for_writing(const char* filename);
        void open_for_reading(const char* filename);
        void close();
        void insert_id(unsigned int id, unsigned int data_file_block_index);
        Paper* get_paper_by_id(HashFile* hash_file, unsigned int id);
        unsigned int qt_blocks_in_index();
};