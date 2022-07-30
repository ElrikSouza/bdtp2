#include "hash-file.h"

#include <fstream>
#include <iostream>

#include "../binary-file.h"
#include "../config.h"
#include "../paper-block.h"
#include "../paper.h"

HashFile::HashFile(int number_of_buckets, int blocks_per_bucket) {
    _blocks_per_bucket = blocks_per_bucket;
    _number_of_buckets = number_of_buckets;

    int total_num_of_blocks = number_of_buckets * blocks_per_bucket;
    _bin_file = BinaryFile(total_num_of_blocks, BLOCK_SIZE);
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

unsigned int HashFile::insert_paper(Paper* paper) {
    int bucket_number = hash_paper(paper->id);
    unsigned char* block_buffer;
    int current_block;

    // vamos tentar inserir o artigo no primeiro bloco livre do bucket
    for (int i = 0; i < _blocks_per_bucket; i++) {
        current_block = bucket_number * _blocks_per_bucket + i;
        block_buffer = _bin_file.read_block(current_block);
        PaperBlock block(block_buffer);
        bool did_insertion_succeed = block.insert_paper(paper);

        if (did_insertion_succeed) {
            // conseguimos inserir, então vamos gravar no disco e retornar o bloco onde
            // foi gravado
            _bin_file.write_block(block.get_block_buffer(), current_block);
            delete block_buffer;
            return current_block;
        }

        delete block_buffer;
    }

    std::cout << "Não foi possível inserir o artigo. Desligando..." << std::endl;
    exit(1);

    return 0;
}

Paper* HashFile::get_paper_by_id(unsigned int paper_id) {
    int bucket_number = hash_paper(paper_id);
    int current_block;
    unsigned char* block_buffer;
    Paper* paper = nullptr;

    // sabemos em qual bucket o artigo pode estar, mas não sabemos em que bloco ele está
    // então vamos iterar por todos os blocos do bucket até achar
    for (int i = 0; i < _blocks_per_bucket; i++) {
        current_block = bucket_number * _blocks_per_bucket + i;
        block_buffer = _bin_file.read_block(current_block);
        PaperBlock block(block_buffer);

        paper = block.get_paper_if_it_is_inside(paper_id);

        delete block_buffer;

        if (paper != nullptr) {
            std::cout << "Blocos lidos = " << i + 1 << std::endl;
            break;
        }
    }

    // se não encontramos, então paper vai continuar sendo um nullptr
    // caso contrario, ele vai ter o artigo correto.
    return paper;
}

unsigned char* HashFile::read_block(unsigned int block_index) {
    return _bin_file.read_block((int) block_index);
}