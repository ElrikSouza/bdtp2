#include "../src/db/hash-file.h"

#include <vector>

#include "../src/parsing/paper-stream.h"

#define BUCKETS 400000
#define BLOCKS_PER_BUCKET 1

bool are_papers_identical(Paper* paper_a, Paper* paper_b) {
    return (paper_a->id == paper_b->id) && (paper_a->authors == paper_b->authors) && (paper_a->year == paper_b->year) &&
           (paper_a->citations == paper_b->citations) && (paper_a->snippet == paper_b->snippet) &&
           (paper_a->title == paper_b->title) && (paper_a->updated_at.compare(0, 19, paper_b->updated_at, 0, 19) == 0);
}

int main() {
    std::vector<Paper*> papers;
    PaperStream stream;

    Paper* current_paper;
    stream.open_source_file("artigo.csv");

    HashFile f(BUCKETS, BLOCKS_PER_BUCKET);
    f.create_and_open_for_writing("test.hex");

    std::cout << "INSERINDO ARTIGOS" << std::endl;

    int papers_inserted = 0;
    while ((current_paper = stream.get_next_paper()) != nullptr) {
        f.insert_paper(current_paper);
        papers.push_back(current_paper);
        papers_inserted++;
    }

    f.close();

    f.open_file_for_reading("test.hex");

    std::cout << "VERIFICANDO INTEGRIDADE" << std::endl;

    int papers_checked = 0;
    for (auto reference_paper : papers) {
        current_paper = f.get_paper_by_id(reference_paper->id);
        if (current_paper == nullptr) {
            std::cout << "[TESTE FALHOU] Um artigo com id = " << reference_paper->id << " foi perdido" << std::endl;
            return 0;
        } else if (!are_papers_identical(reference_paper, current_paper)) {
            std::cout << "[TESTE FALHOU] artigo recuperado eh diferente do artigo original" << std::endl;
            std::cout << "original:\n";
            reference_paper->print();
            std::cout << "recuperado:\n";
            current_paper->print();
            return 0;
        }

        papers_checked++;
    }

    if (papers_checked == papers_inserted) {
        std::cout << "O teste do hash file passou" << std::endl;
    } else {
        std::cout << "[TESTE FALHOU] numero de artigos lidos nao bate com o numero inserido" << std::endl;
    }

    std::cout << "lidos=" << papers_checked << ";inseridos=" << papers_inserted << std::endl;

    return 1;
}