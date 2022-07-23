#include <fstream>

#include "../paper.h"

// Classe que permite ler o arquivo de entrada
// como uma stream de objetos Paper, assim economizando memória
class PaperStream {
   private:
    std::fstream file;

   public:
    void open_source_file(const char* filename);

    Paper* get_next_paper();
};