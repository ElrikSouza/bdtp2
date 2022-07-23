#include "../buffer/buffer.h"
#include "bptree-leaf-block.h"
#include "bptree-internal-block.h"

#pragma once

/**
 * Essa classe representa uma abstração de um bloco de tamanho arbitrário para auxiliar
 * no split da árvore B+.
 */
class AuxiliarBlock {
    private: 
        Buffer _buffer{nullptr, 0};

    public:
        AuxiliarBlock(char* bytes, int block_size);
        BPTreeLeafBlock* deserialize_into_leaf_block();
        BPTreeInternalBlock* deserialize_into_internal_block();
};