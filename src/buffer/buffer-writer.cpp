#include <iostream>

class BufferWriter {
   private:
    char* _internal_buffer;
    int _buffer_capacity;

   public:
    BufferWriter(int buffer_capacity);
    ~BufferWriter();

    void write_2byte_number(unsigned short int number);
    void write_4byte_number(unsigned int number);
    void write_fixed_length_string(std::string word);
    int write_varchar(std::string varchar);
};