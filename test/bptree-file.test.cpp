#include "../src/btree/bptree.h"

int main() {
    BPTree bptree;

    bptree.create_tree_file("index.bin");
    std::cout << "Arquivo criado" << std::endl;

    bptree.insert_key(10, 11);

    std::cout << "Hello World" << std::endl;

    return 0;
}