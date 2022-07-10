#include "paper-block.h"

#include "buffer/buffer-utils.h"
#include "buffer/buffer.h"
#include "paper.h"

PaperBlock::PaperBlock(unsigned char* paper_block_bytes) {
    _block_bytes = paper_block_bytes;
    _bytes_used = read_2byte_number_from_buffer(paper_block_bytes, 0);

    if (_bytes_used == 0) {
        _bytes_used = 2;
    }

    _buffer = Buffer(paper_block_bytes, 4096);
}

Paper* PaperBlock::get_paper_if_it_is_inside(unsigned int target_paper_id) {
    if (_bytes_used == 0) {
        return nullptr;
    }

    // skip the block ocupation header
    _buffer.jump_to_the_start();
    _buffer.jump_n_bytes_from_current_position(2);

    int paper_register_size;
    unsigned int paper_id;

    while (!_buffer.has_ended()) {
        paper_register_size = _buffer.read_2byte_number();

        if (paper_register_size == 0) {
            return nullptr;
        }

        paper_id = _buffer.read_4byte_number();

        // go back to the start of the register
        _buffer.jump_n_bytes_from_current_position(-6);

        // std::cout << _buffer.get_current_cursor_position() << std::endl;

        if (paper_id == target_paper_id) {
            Paper* paper = new Paper;
            unsigned char* bytes = (unsigned char*)_buffer.get_buffer_bytes();
            paper->deserialize(bytes, _buffer.get_current_cursor_position());

            return paper;
        }

        // go next
        _buffer.jump_n_bytes_from_current_position(paper_register_size);
        // std::cout << _buffer.get_current_cursor_position() << std::endl;
    }

    return nullptr;
}

bool PaperBlock::insert_paper(Paper* paper) {
    _buffer.jump_to_the_start();

    int buffer_length = _buffer.read_2byte_number();

    if (buffer_length == 0) {
        buffer_length = 2;
    }

    _buffer.jump_to_the_start();

    int paper_register_size = paper->get_register_size();

    if (buffer_length + paper_register_size > 4096) {
        // nao cabe
        return false;
    }

    _buffer.jump_n_bytes_from_current_position(buffer_length);
    _buffer.write_bytes((char*)paper->serialize(), paper_register_size);

    // update length header
    unsigned short int new_buffer_len = buffer_length + paper_register_size;
    _buffer.jump_to_the_start();
    _buffer.write_2byte_number(new_buffer_len);
    _bytes_used = new_buffer_len;

    return true;
}

char* PaperBlock::get_block_buffer() {
    return _buffer.get_buffer_bytes();
}