#include "paper-stream.h"

#include "parser.h"

void PaperStream::open_source_file(const char* filename) {
    file.open(filename, std::ios::in);
    file.seekg(0, std::ios::beg);
}

Paper* PaperStream::get_next_paper() {
    std::string line;

    if (!getline(file, line)) {
        return nullptr;
    }

    Paper* paper = read_csv_line(&file, &line);

    return paper;
}