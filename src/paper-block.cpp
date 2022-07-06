#include "paper.cpp"

class PaperBlock {
    const unsigned char* _block_bytes;

   public:
    PaperBlock(const unsigned char* block_bytes) {
        _block_bytes = block_bytes;
    }

    ~PaperBlock() {
        delete _block_bytes;
    }

    Paper* get_paper_if_it_is_inside_this_block(unsigned int id) {}
};