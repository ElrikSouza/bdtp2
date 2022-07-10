#include "buffer/buffer.h"
#include "paper.h"

#pragma once

/**
 * Esta classe abstrai a inserção e busca de um artigo em um bloco.
 *
 * Ela evelopa um buffer já existente, provendo abstrações para manipular registros de artigos.
 * */
class PaperBlock {
   private:
    unsigned char* _block_bytes;
    unsigned short int _bytes_used;
    Buffer _buffer{nullptr, 0};

   public:
    PaperBlock(unsigned char* paper_block_bytes);

    // Procura um artigo por id dentro de um bloco, se não estiver lá, retorna nullptr
    Paper* get_paper_if_it_is_inside(unsigned int id);

    // Tenta inserir um artigo se tiver espaço, o bool retornado indica se a inserção deu certo
    bool insert_paper(Paper* paper);

    // Retorna o buffer interno da classe. Use esse método antes de inserir denovo no arquivo, mas não manipule esse
    // buffer diretamente.
    char* get_block_buffer();
};