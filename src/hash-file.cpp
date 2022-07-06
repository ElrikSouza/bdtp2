#include "paper.cpp"

class HashFile {
    void open_file_for_reading();
    void create_and_open_for_writing();
    void insert_paper(Paper* paper);
    Paper* get_paper_by_id(unsigned int id);
};

void HashFile::insert_paper(Paper* paper) {
    // hash
    // paper => buffer

    //
}