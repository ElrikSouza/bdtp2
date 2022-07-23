#include "../config.h"
#include "../db/hash-file.h"
#include "../parsing/paper-stream.h"

int main(int argc, char const *argv[]) {
    PaperStream stream;

    char const *string_id = argv[1];
    unsigned int actual_id = atoi(string_id);

    stream.open_source_file("hash-data-file.bin");

    HashFile hash_file(BUCKETS, BLOCKS_PER_BUCKET);
    hash_file.open_file_for_reading(HASH_FILE_NAME);

    Paper *result = hash_file.get_paper_by_id(actual_id);
    hash_file.close();

    if (result == nullptr) {
        std::cout << "Artigo não encontrado." << std::endl;
        return 1;
    }

    std::cout << "Artigo encontrado" << std::endl;
    std::cout << "-------------------------------" << std::endl;
    result->print();

    return 0;
}