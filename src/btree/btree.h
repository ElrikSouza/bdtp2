#include <stack>

#include "../binary-file.h"
#include "../buffer/buffer.h"

class BPTreeHeaderBlock {
   public:
    BPTreeHeaderBlock();
    BPTreeHeaderBlock(unsigned char* block_buffer);

    unsigned int get_root_node_index();

    unsigned int get_next_free_block_and_point_to_next();

    void set_new_root_index(unsigned int new_root_index);

    unsigned char* get_block_buffer();
};

class BPTreeInternalBlock {
   private:
    Buffer _buffer;

   public:
    BPTreeInternalBlock();
    BPTreeInternalBlock(unsigned char* block_buffer);

    void insert_key(unsigned int key, unsigned int split_node_block_index);
    unsigned int get_middle_key();
    void transfer_data_and_pointers_to_split_node(BPTreeInternalBlock* split_node, unsigned int split_node_block_index);

    unsigned int get_matching_pointer(unsigned int key);
    bool are_there_free_slots();

    unsigned char* get_block_buffer();
};

class BPTreeRootBlock {
   public:
    BPTreeRootBlock();

    void insert_key(unsigned int key, unsigned int lesser_than, unsigned int greater_than);
    unsigned char* get_block_buffer();
};

class BPTreeLeafBlock {
   private:
    Buffer _buffer;

   public:
    BPTreeLeafBlock();
    BPTreeLeafBlock(unsigned char* block_buffer);
    unsigned int get_middle_key();
    void transfer_data_and_pointers_to_split_node(BPTreeLeafBlock* split_node, unsigned int split_node_block_index);
    bool are_there_free_slots();
    bool insert_key(unsigned int key, unsigned int data_file_block_index);

    unsigned char* get_block_buffer();
};

class BPTree {
   private:
    BPTreeHeaderBlock _header_block;
    BinaryFile _tree_file;
    void update_header();
    unsigned int get_next_free_block_and_point_to_next();
    unsigned int get_root_node();
    std::stack<unsigned int> get_path_to_leaf(unsigned int key);

   public:
    BPTree();

    bool insert_key(unsigned int key, unsigned int data_file_block_index);

    // procura por uma chave na arvore, se exister, retorna um ponteiro para o bloco
    // correspondente no arquivo de dados, caso contrario, retorna 0
    unsigned int get_data_pointer(unsigned int key);

    // cria ou reescreve uma arvore b+ no caminho especificado
    void create_tree_file(const char* filename);

    // apenas le uma arvore b+ no caminho especificado sem fazer alteracoes
    void read_tree_file(const char* filename);

    bool is_leaf(unsigned char* block_buffer);
};
