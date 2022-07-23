#include "../binary-file.h"
#include "../paper.h"

#pragma once

/**
 * Classe que abstrai a criação, escrita e leitura do arquivo organizado por hash.
 *
 * Ela permite inserir e recuperar um artigo.
 * */
class HashFile {
   private:
    BinaryFile _bin_file;
    int _blocks_per_bucket;
    int _number_of_buckets;

    int hash_paper(unsigned int paper_id);

   public:
    HashFile(int number_of_buckets, int blocks_per_bucket);

    void close();
    void open_file_for_reading(const char* filename);
    void create_and_open_for_writing(const char* filename);

    // insere um artigo e retorna o bloco em que ele foi inserido
    unsigned int insert_paper(Paper* paper);

    // tenta achar um artigo pela id. se não achar, retorna nullptr
    Paper* get_paper_by_id(unsigned int id);
};
