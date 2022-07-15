#include <stack>

#include "../binary-file.h"

class BPTreeHeaderBlock {
   public:
    BPTreeHeaderBlock();
    BPTreeHeaderBlock(unsigned char* block_buffer);

    unsigned int get_root_node_index();

    unsigned int get_next_free_block();
};

class BPTreeInternalBlock {
   public:
    BPTreeInternalBlock();
    BPTreeInternalBlock(char* block_buffer);
};

class BPTreeLeafBlock {
   public:
    BPTreeLeafBlock();
    BPTreeLeafBlock(char* block_buffer);

    unsigned int get_middle_key();
    void transfer_data_to_split_node(BPTreeLeafBlock* split_node);
};

class BPTree {
   private:
    BPTreeHeaderBlock _header_block;
    BinaryFile _tree_file;
    void update_header();
    unsigned int get_next_free_block();
    unsigned int get_root_node();
    std::stack<unsigned int> get_path_to_leaf(unsigned int key);

   public:
    BPTree();

    bool insert_key(unsigned int key);

    // procura por uma chave na arvore, se exister, retorna um ponteiro para o bloco
    // correspondente no arquivo de dados, caso contrario, retorna 0
    unsigned int get_data_pointer(unsigned int key);

    // cria ou reescreve uma arvore b+ no caminho especificado
    void create_tree_file(const char* filename);

    // apenas le uma arvore b+ no caminho especificado sem fazer alteracoes
    void read_tree_file(const char* filename);
};
