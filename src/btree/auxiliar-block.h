#include "../buffer/buffer.h"
#include "../config.h"

#pragma once

/**
 * Essa classe representa uma abstração de um bloco de tamanho arbitrário para auxiliar
 * no split da árvore B+.
 */
class AuxiliarBlock {
    private:
        unsigned int _block_size;
        Buffer _buffer{nullptr, 0};

    public:
        AuxiliarBlock(int block_size);
        AuxiliarBlock(char* bytes, int block_size);

        void copy_all_key_pointer(char* bytes, int num_of_bytes);

        void leaf_copy_first_half_of_key_pointer_to_buffer(char* bytes);
        void leaf_copy_second_half_of_key_pointer_to_buffer(char* bytes);
        void insert_leaf_key_pointer(unsigned int key, unsigned int block_index);

        void internal_copy_first_half_of_key_pointer_to_buffer(char* bytes);
        void internal_copy_second_half_of_key_pointer_to_buffer(char* bytes);
        void insert_internal_key_pointer(unsigned int key, unsigned int block_index);

        char* get_block_buffer();
};