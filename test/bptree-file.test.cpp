#include "../src/btree/bptree.h"

int main() {
    BPTree bptree;

    bptree.create_tree_file("index.bin");
    std::cout << "Arquivo criado" << std::endl;

    for (int i = 0; i < 255 * 4 + 1; i++) {
        bptree.insert_key(i + 1, i * 2);
    }

    auto aa = bptree.get_path_to_leaf(765);
    std::cout << "loop" << std::endl;
    while (!aa.empty()) {
        std::cout << aa.top() << std::endl;
        aa.pop();
    }
    // bptree.insert_key(10, 11);
    // bptree.insert_key(3, 6);
    // bptree.insert_key(2, 3);
    // bptree.insert_key(15, 9);

    std::cout << "Hello World" << std::endl;

    return 0;
}