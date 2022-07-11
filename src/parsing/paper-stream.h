#include <fstream>

#include "../paper.h"

class PaperStream {
   private:
    std::fstream file;

   public:
    void open_source_file(const char* filename);

    Paper* get_next_paper();
};