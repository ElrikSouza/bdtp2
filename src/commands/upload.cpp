#include <vector>

#include "../db/hash-file.h"
#include "../parsing/paper-stream.h"

// #define BUCKETS 1000000
#define BUCKETS 400000
#define BLOCKS_PER_BUCKET 1

int main() {
    std::vector<unsigned int> ids;
    PaperStream stream;

    Paper* current_paper;
    stream.open_source_file("artigo.csv");

    HashFile f(BUCKETS, BLOCKS_PER_BUCKET);
    f.create_and_open_for_writing("result.hex");

    while ((current_paper = stream.get_next_paper()) != nullptr) {
        f.insert_paper(current_paper);
        ids.emplace_back(current_paper->id);
        delete current_paper;
    }

    f.close();

    f.open_file_for_reading("result.hex");

    for (int i : ids) {
        current_paper = f.get_paper_by_id(i);
        if (current_paper == nullptr) {
            std::cout << "not found" << i << std::endl;
        } else if (current_paper->id != i) {
            std::cout << current_paper->id << " != " << i << std::endl;
        } else {
            // std::cout << i << std::endl;
        }
    }

    return 0;
}