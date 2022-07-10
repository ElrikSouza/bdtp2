#include <cstdio>
#include <fstream>
#include <iostream>

#include "paper-block.h"
#include "paper.h"

#pragma once

/**
 * Esta é uma classe para abstrair a leitura e escrita de blocos de um tamanho X em um arquivo binario qualquer.
 *
 * A ideia é não usar `fstream` e afins no código.
 */
class BinaryFile {
   private:
    std::fstream _file_stream;
    int _num_of_blocks;
    int _block_size;

   public:
    BinaryFile();
    BinaryFile(int num_of_blocks, int block_size);

    void create_or_rewrite(const char* filename);

    void open_as_readonly(const char* filename);

    void close();

    // pré-aloca espaço no arquivo preenchendo com zeros
    void zero_the_file_out();

    /**
     * Escreve um buffer em um bloco na posição indicada.
     * O offset em bytes é calculado a partir do block_index multiplicado pelo tamanho do bloco.
     * */
    void write_block(const char* block_buffer, int block_index);

    /*
     Método que lê um bloco em uma posicao x e aloca um buffer para esse bloco.
     Importante notar que a posição do bloco é calculada a partir do index multiplicando pelo tamanho do bloco. Esse
     parametro **NÃO** é o offset em bytes */
    unsigned char* read_block(int block_index);
};
