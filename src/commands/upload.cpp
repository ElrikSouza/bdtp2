#include "../config.h"
#include "../db/hash-file.h"
#include "../parsing/paper-stream.h"
#include "../db/primary-index.h"

int main(int argc, char const *argv[]) {
    PaperStream stream;

    char const *filename = argv[1];

    stream.open_source_file(filename);

    HashFile hash_file(BUCKETS, BLOCKS_PER_BUCKET);
    hash_file.create_and_open_for_writing(HASH_FILE_NAME);
    PrimaryIndex primary_index;
    primary_index.create_and_open_for_writing(PRIMARY_INDEX_NAME);
    Paper *current_paper;

    unsigned int total_papers = 0;
    while ((current_paper = stream.get_next_paper()) != nullptr) {
        unsigned int data_file_block_index = hash_file.insert_paper(current_paper);
        primary_index.insert_id(current_paper->id, data_file_block_index);

        total_papers++;
        delete current_paper;
    }

    std::cout << "Artigos inseridos: " << total_papers << std::endl;
    hash_file.close();
    primary_index.close();

    return 0;
}