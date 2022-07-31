#include "../config.h"
#include "../db/hash-file.h"
#include "../db/primary-index.h"
#include "../parsing/paper-stream.h"

int main(int argc, char const *argv[]) {
    char const *string_id = argv[1];
    unsigned int actual_id = atoi(string_id);

    HashFile hash_file(BUCKETS, BLOCKS_PER_BUCKET);
    hash_file.open_file_for_reading(HASH_FILE_NAME);
    PrimaryIndex primary_index;
    primary_index.open_for_reading(PRIMARY_INDEX_NAME);

    Paper *result = primary_index.get_paper_by_id(&hash_file, actual_id);

    if (result == nullptr) {
        std::cout << "Artigo não encontrado." << std::endl;
        return 1;
    }

    std::cout << "Artigo encontrado" << std::endl;
    std::cout << "-------------------------------" << std::endl;
    result->print();

    hash_file.close();
    primary_index.close();

    return 0;
}