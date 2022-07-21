#include "../src/btree/bptree.h"
#include "../src/buffer/buffer.h"

bool is_key_inside_block(char* buffer, unsigned int key) {
    Buffer b((unsigned char*)buffer, 4096);

    unsigned int current_key;
    while (b.get_current_cursor_position() <= 4096 - 12) {
        current_key = b.read_4byte_number();

        if (current_key == key) {
            return true;
        }
    }

    return false;
}

int main() {
    int keys = 255 * 4 + 1;
    BPTree bptree;

    bptree.create_tree_file("index.bin");
    std::cout << "Arquivo criado" << std::endl;

    for (int i = 0; i < keys; i++) {
        bptree.insert_key(i + 1, i * 2);
    }

    std::cout << "key2=" << bptree.get_data_pointer(2) << std::endl;

    for (int i = 0; i < keys; i++) {
        if (bptree.get_data_pointer(i + 1) != i * 2) {
            std::cout << std::endl
                      << i + 1 << " didnt match " << bptree.get_data_pointer(i + 1) << "!=" << i * 2 << std::endl;
            return 0;
        }
    }

    return 0;
}