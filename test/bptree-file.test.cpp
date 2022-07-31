#include "../src/btree/bptree-internal-block.h"
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
    // int keys = 255 * 4 + 1;
    // int keys = 1500000;
    int keys = 1000000;
    // int keys = 1000;
    BPTree bptree;
    int qtd_keys = 0;

    bptree.create_tree_file("index.bin");
    std::cout << "Arquivo criado" << std::endl;

    for (unsigned int i = 0; i < keys; i++) {
        bptree.insert_key(i + 1, i * 2);
        qtd_keys++;
    }

    std::cout << "checking" << std::endl;

    unsigned aaa;
    float error = 0;
    for (unsigned int i = 0; i < keys; i++) {
        aaa = bptree.get_data_pointer(i + 1);
        std::cout << std::endl << i + 1 << " match " << aaa << "==" << i * 2 << std::endl;

        if (aaa != i * 2) {
            std::cout << std::endl << i + 1 << " didnt match " << aaa << "!=" << i * 2 << std::endl;
            error++;
        }
    }

    std::cout << "error rate=" << error / keys << std::endl;
    std::cout << "error rate=" << error << std::endl;

    return 0;
}