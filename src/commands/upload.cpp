#include "../parser.h"

int main() {
    PaperStream stream;
    stream.open_source_file("artigo.csv");
    (stream.get_next_paper())->print();
    (stream.get_next_paper())->print();
    (stream.get_next_paper())->print();
    return 0;
}