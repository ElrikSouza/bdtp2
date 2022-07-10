#include "buffer/buffer-writer.h"
#include "paper.h"

#pragma once

class PaperBlock {
   private:
    unsigned char* _block_bytes;
    unsigned short int _bytes_used;
    Buffer _buffer{nullptr, 0};

   public:
    PaperBlock(unsigned char* paper_block_bytes);

    Paper* get_paper_if_it_is_inside(unsigned int id);
    bool insert_paper(Paper* paper);
    char* get_block_buffer();
};