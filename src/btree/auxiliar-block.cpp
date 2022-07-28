#include "auxiliar-block.h"
#include "../buffer/buffer-utils.h"

AuxiliarBlock::AuxiliarBlock(int block_size) {
    _block_size = block_size;
    _buffer = Buffer(block_size);
}

AuxiliarBlock::AuxiliarBlock(char* bytes, int block_size) {
    _buffer = Buffer(block_size);
    _block_size = block_size;
    _buffer.write_bytes(bytes, BLOCK_SIZE);
}

void AuxiliarBlock::copy_all_key_pointer(char* bytes, int num_of_bytes) {
    _buffer.jump_to_the_start();

    _buffer.write_bytes(bytes + BPTREE_HEADER_SIZE, num_of_bytes - BPTREE_HEADER_SIZE);
    _buffer.jump_to_the_start();

    // std::cout << ">>>> DEPOIS DE COPIAR TODOS OS PARES" << std::endl;
    // for (int i = 0; i < 510; i++) {
    //     std::cout << "index = " << read_4byte_number_from_buffer((unsigned char*) _buffer.get_buffer_bytes(), i * 8) << std::endl;
    //     std::cout << "key = " << read_4byte_number_from_buffer((unsigned char*) _buffer.get_buffer_bytes(), i * 8 + 4) << std::endl;
    // }
    // std::cout << "last ptr = " << read_4byte_number_from_buffer((unsigned char*) _buffer.get_buffer_bytes(), 4088) << std::endl;
}

void AuxiliarBlock::leaf_copy_first_half_of_key_pointer_to_buffer(char* bytes) {
    char* buffer_bytes = _buffer.get_buffer_bytes();

    // std::cout << "\n>>>>>>>>>>>>>>>>>INSERTING FIRST HALF\n" << std::endl;
    for (int i = 0; i < BPTREE_MIN_KEYS_PER_BLOCK; i++) {
        unsigned int current_index = read_4byte_number_from_buffer((unsigned char*) buffer_bytes, i * BPTREE_INDEX_VALUE_PAIR_SIZE);
        unsigned int current_key = read_4byte_number_from_buffer((unsigned char*) buffer_bytes, i * BPTREE_INDEX_VALUE_PAIR_SIZE + 4);
        // std::cout << "current index = " << current_index << " current key = " << current_key << " i = " << i << std::endl;

        write_4byte_number_to_buffer(bytes, current_index, (i * BPTREE_INDEX_VALUE_PAIR_SIZE) + BPTREE_HEADER_SIZE);
        write_4byte_number_to_buffer(bytes, current_key, (i * BPTREE_INDEX_VALUE_PAIR_SIZE) + BPTREE_HEADER_SIZE + 4);
    }
    // std::cout << "\n>>>>>>>>>>>>>>>>>FIRST HALF\n" << std::endl;
    // for (int i = 0; i < 255; i++) {
    //     std::cout << "current index = " << read_4byte_number_from_buffer((unsigned char*) bytes, i * 8 + 12) << " current key = " << read_4byte_number_from_buffer((unsigned char*) bytes, i * 8 + 12 + 4) << " i = " << i << std::endl;
    // }

    write_2byte_number_to_buffer(bytes, BPTREE_MIN_KEYS_PER_BLOCK, 2);
}

void AuxiliarBlock::leaf_copy_second_half_of_key_pointer_to_buffer(char* bytes) {
    char* buffer_bytes = _buffer.get_buffer_bytes();

    // std::cout << "\n>>>>>>>>>>>>>>>>>INSERTING SECOND HALF\n" << std::endl;
    for (int i = 0; i <= BPTREE_MIN_KEYS_PER_BLOCK; i++) {
        unsigned int current_index = read_4byte_number_from_buffer((unsigned char*) buffer_bytes, (i + BPTREE_MIN_KEYS_PER_BLOCK) * BPTREE_INDEX_VALUE_PAIR_SIZE);
        unsigned int current_key = read_4byte_number_from_buffer((unsigned char*) buffer_bytes, (i + BPTREE_MIN_KEYS_PER_BLOCK) * BPTREE_INDEX_VALUE_PAIR_SIZE + 4);

        // std::cout << "current index = " << current_index << " current key = " << current_key << " i = " << i << std::endl;

        write_4byte_number_to_buffer(bytes, current_index, (i * BPTREE_INDEX_VALUE_PAIR_SIZE) + BPTREE_HEADER_SIZE);
        write_4byte_number_to_buffer(bytes, current_key, (i * BPTREE_INDEX_VALUE_PAIR_SIZE) + BPTREE_HEADER_SIZE + 4);
    }
    // unsigned int last_block_index = read_4byte_number_from_buffer((unsigned char*) buffer_bytes, 4088);
    // write_4byte_number_to_buffer(bytes, last_block_index, 4092);

    // std::cout << "\n>>>>>>>>>>>>>>>>>SECOND HALF\n" << std::endl;
    // for (int i = 0; i <= 255; i++) {
    //     std::cout << "current index = " << read_4byte_number_from_buffer((unsigned char*) bytes, i * 8 + 12) << " current key = " << read_4byte_number_from_buffer((unsigned char*) bytes, i * 8 + 12 + 4) << " i = " << i << std::endl;
    // }
    // std::cout << "last ptr = " << read_4byte_number_from_buffer((unsigned char*) buffer_bytes, 4088) << std::endl;

    write_2byte_number_to_buffer(bytes, BPTREE_MIN_KEYS_PER_BLOCK + 1, 2);
}

void AuxiliarBlock::insert_leaf_key_pointer(unsigned int key, unsigned int block_index) {
    unsigned int current_index = read_4byte_number_from_buffer((unsigned char*) _buffer.get_buffer_bytes(), 0);
    unsigned int current_key = read_4byte_number_from_buffer((unsigned char*) _buffer.get_buffer_bytes(), 4);

    int i = BPTREE_INDEX_VALUE_PAIR_SIZE;
    while (key > current_key && (i + BPTREE_INDEX_VALUE_PAIR_SIZE < _block_size)) {
        current_index = read_4byte_number_from_buffer((unsigned char*) _buffer.get_buffer_bytes(), i);
        current_key = read_4byte_number_from_buffer((unsigned char*) _buffer.get_buffer_bytes(), i + 4);

        i += BPTREE_INDEX_VALUE_PAIR_SIZE;
    }
    i = i - BPTREE_INDEX_VALUE_PAIR_SIZE;
    // std::cout << ">>>>> ACHOU ONDE INSERIR CURRENT INDEX = " << current_index << " CURRENT KEY = " << current_key << " i = " << i << " block size = " << _block_size << std::endl;
    
    unsigned int aux_index = current_index;
    unsigned int aux_key = current_key;

    write_4byte_number_to_buffer(_buffer.get_buffer_bytes(), block_index, i);
    write_4byte_number_to_buffer(_buffer.get_buffer_bytes(), key, i + 4);

    i += BPTREE_INDEX_VALUE_PAIR_SIZE;

    // nao eh necessario alterar o ptr da lista encadeada
    while (i < _block_size - 4) {
        current_index = read_4byte_number_from_buffer((unsigned char*) _buffer.get_buffer_bytes(), i);
        current_key = read_4byte_number_from_buffer((unsigned char*) _buffer.get_buffer_bytes(), i + 4);

        write_4byte_number_to_buffer(_buffer.get_buffer_bytes(), aux_index, i);
        write_4byte_number_to_buffer(_buffer.get_buffer_bytes(), aux_key, i + 4);
        // std::cout << "DEU O SHIFT" << std::endl;

        aux_index = current_index;
        aux_key = current_key;
        
        i += BPTREE_INDEX_VALUE_PAIR_SIZE;
    }

    // std::cout << "\n>>>>>>>>>>>>>>>>>DEPOIS DE INSERIR O NOVO VALOR\n" << std::endl;
    // for (int i = 0; i < _block_size - 4; i += 8) {
    //     std::cout << "ptr = " << read_4byte_number_from_buffer((unsigned char*) _buffer.get_buffer_bytes(), i) << std::endl;
    //     std::cout << "key = " << read_4byte_number_from_buffer((unsigned char*) _buffer.get_buffer_bytes(), i + 4) << std::endl;
    //     std::cout << "i = " << i << std::endl;
    // }

    // std::cout << "i = " << i << std::endl;
    // std::cout << "last ptr = " << read_4byte_number_from_buffer((unsigned char*) _buffer.get_buffer_bytes(), 4088) << std::endl;
}

void AuxiliarBlock::internal_copy_first_half_of_key_pointer_to_buffer(char* bytes) {
    char* buffer_bytes = _buffer.get_buffer_bytes();
    int a = 0;

    // std::cout << "\n>>>>>>>>>>>>>>>>>INSERTING FIRST HALF\n" << std::endl;
    for (int i = 0; i < BPTREE_MIN_KEYS_PER_BLOCK; i++) {
        unsigned int current_index = read_4byte_number_from_buffer((unsigned char*) buffer_bytes, i * BPTREE_INDEX_VALUE_PAIR_SIZE);
        unsigned int current_key = read_4byte_number_from_buffer((unsigned char*) buffer_bytes, i * BPTREE_INDEX_VALUE_PAIR_SIZE + 4);
        // std::cout << "current index = " << current_index << " current key = " << current_key << " i = " << i << std::endl;

        write_4byte_number_to_buffer(bytes, current_index, (i * BPTREE_INDEX_VALUE_PAIR_SIZE) + BPTREE_HEADER_SIZE);
        write_4byte_number_to_buffer(bytes, current_key, (i * BPTREE_INDEX_VALUE_PAIR_SIZE) + BPTREE_HEADER_SIZE + 4);
        a++;
    }
    // std::cout << "\na = " << a << std::endl;
    // std::cout << "\n>>>>>>>>>>>>>>>>>FIRST HALF\n" << std::endl;
    // for (int i = 0; i < 255; i++) {
    //     std::cout << "current index = " << read_4byte_number_from_buffer((unsigned char*) bytes, i * 8 + 12) << " current key = " << read_4byte_number_from_buffer((unsigned char*) bytes, i * 8 + 12 + 4) << " i = " << i << std::endl;
    // }

    write_2byte_number_to_buffer(bytes, BPTREE_MIN_KEYS_PER_BLOCK, 2);
}

void AuxiliarBlock::internal_copy_second_half_of_key_pointer_to_buffer(char* bytes) {
    char* buffer_bytes = _buffer.get_buffer_bytes();
    int a = 0;

    // std::cout << "\n>>>>>>>>>>>>>>>>>INSERTING SECOND HALF\n" << std::endl;
    for (int i = 0; i <= BPTREE_MIN_KEYS_PER_BLOCK; i++) {
        unsigned int current_index = read_4byte_number_from_buffer((unsigned char*) buffer_bytes, (i + BPTREE_MIN_KEYS_PER_BLOCK) * BPTREE_INDEX_VALUE_PAIR_SIZE);
        unsigned int current_key = read_4byte_number_from_buffer((unsigned char*) buffer_bytes, (i + BPTREE_MIN_KEYS_PER_BLOCK) * BPTREE_INDEX_VALUE_PAIR_SIZE + 4);

        // std::cout << "current index = " << current_index << " current key = " << current_key << " i = " << i << std::endl;

        write_4byte_number_to_buffer(bytes, current_index, (i * BPTREE_INDEX_VALUE_PAIR_SIZE) + BPTREE_HEADER_SIZE);
        write_4byte_number_to_buffer(bytes, current_key, (i * BPTREE_INDEX_VALUE_PAIR_SIZE) + BPTREE_HEADER_SIZE + 4);
        a++;
    }
    // std::cout << "\na = " << a << std::endl;
    // unsigned int last_block_index = read_4byte_number_from_buffer((unsigned char*) buffer_bytes, 4088);
    // write_4byte_number_to_buffer(bytes, last_block_index, 4092);

    // std::cout << "\n>>>>>>>>>>>>>>>>>SECOND HALF\n" << std::endl;
    // for (int i = 0; i <= 255; i++) {
    //     std::cout << "current index = " << read_4byte_number_from_buffer((unsigned char*) bytes, i * 8 + 12) << " current key = " << read_4byte_number_from_buffer((unsigned char*) bytes, i * 8 + 12 + 4) << " i = " << i << std::endl;
    // }
    // std::cout << "last ptr = " << read_4byte_number_from_buffer((unsigned char*) buffer_bytes, 4088) << std::endl;
    unsigned int last_subtree_index = read_4byte_number_from_buffer((unsigned char*) buffer_bytes, 4088);
    write_4byte_number_to_buffer(bytes, last_subtree_index, BPTREE_LAST_INDEX);
    write_2byte_number_to_buffer(bytes, BPTREE_MIN_KEYS_PER_BLOCK + 1, 2);
}

void AuxiliarBlock::insert_internal_key_pointer(unsigned int key, unsigned int block_index) {
    unsigned int current_index = read_4byte_number_from_buffer((unsigned char*) _buffer.get_buffer_bytes(), 0);
    unsigned int current_key = read_4byte_number_from_buffer((unsigned char*) _buffer.get_buffer_bytes(), 4);
    unsigned int last_subtree_index = read_4byte_number_from_buffer((unsigned char*) _buffer.get_buffer_bytes(), _block_size - 4);

    int i = BPTREE_INDEX_VALUE_PAIR_SIZE;
    while (key > current_key && (i + BPTREE_INDEX_VALUE_PAIR_SIZE < _block_size)) {
        current_index = read_4byte_number_from_buffer((unsigned char*) _buffer.get_buffer_bytes(), i);
        current_key = read_4byte_number_from_buffer((unsigned char*) _buffer.get_buffer_bytes(), i + 4);

        i += BPTREE_INDEX_VALUE_PAIR_SIZE;
    }
    i = i - BPTREE_INDEX_VALUE_PAIR_SIZE;
    std::cout << ">>>>> ACHOU ONDE INSERIR CURRENT INDEX = " << current_index << " CURRENT KEY = " << current_key << " i = " << i << " block size = " << _block_size << std::endl;

    unsigned int aux_index = current_index;
    unsigned int aux_key = current_key;

    write_4byte_number_to_buffer(_buffer.get_buffer_bytes(), last_subtree_index, i);
    write_4byte_number_to_buffer(_buffer.get_buffer_bytes(), key, i + 4);
    write_4byte_number_to_buffer(_buffer.get_buffer_bytes(), block_index, _block_size - 4);

    i += BPTREE_INDEX_VALUE_PAIR_SIZE;

    while (i < _block_size - 4) {
        current_index = read_4byte_number_from_buffer((unsigned char*) _buffer.get_buffer_bytes(), i);
        current_key = read_4byte_number_from_buffer((unsigned char*) _buffer.get_buffer_bytes(), i + 4);

        write_4byte_number_to_buffer(_buffer.get_buffer_bytes(), aux_index, i);
        write_4byte_number_to_buffer(_buffer.get_buffer_bytes(), aux_key, i + 4);
        // std::cout << "DEU O SHIFT" << std::endl;

        aux_index = current_index;
        aux_key = current_key;
        
        i += BPTREE_INDEX_VALUE_PAIR_SIZE;
    }
}

char* AuxiliarBlock::get_block_buffer() {
    return _buffer.get_buffer_bytes();
}