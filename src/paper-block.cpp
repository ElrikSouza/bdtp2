#include "paper.cpp"

class PaperBlock {
   private:
    unsigned char* _block_bytes;
    unsigned short int _bytes_used;

   public:
    PaperBlock(unsigned char* paper_block_bytes) {
        _block_bytes = paper_block_bytes;
        _bytes_used = read_2byte_number_from_buffer(paper_block_bytes, 0);
    }

    Paper* get_paper_if_it_is_inside(unsigned int id);
    bool insert_paper(Paper* paper);
    char* get_block_buffer();
};