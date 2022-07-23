#include "../config.h"
#include "../db/hash-file.h"
#include "../parsing/paper-stream.h"

int main(int argc, char const *argv[]) {
    PaperStream stream;

    char const *filename = argv[1];

    stream.open_source_file(filename);

    HashFile hash_file(BUCKETS, BLOCKS_PER_BUCKET);
    hash_file.create_and_open_for_writing(HASH_FILE_NAME);
    Paper *current_paper;

    int total_papers = 0;
    while ((current_paper = stream.get_next_paper()) != nullptr) {
        hash_file.insert_paper(current_paper);
        total_papers++;
        delete current_paper;
    }

    std::cout << "Artigos inseridos: " << total_papers << std::endl;
    hash_file.close();

    return 0;
}