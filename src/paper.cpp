#include "paper.h"

#include <iostream>
#include <string>

#include "buffer/buffer.h"

void Paper::print() {
    std::cout << "id: " << id << "\n"
              << "titulo: " << title << "\n"
              << "ano: " << year << "\n"
              << "snippet: " << snippet << "\n"
              << "autores: " << authors << "\n"
              << "citacoes: " << citations << "\n"
              << "atualizacao: " << updated_at << "\n"
              << "\n";
}

unsigned short int Paper::get_register_size() {
    return (unsigned short int)title.length() + snippet.length() + authors.length() + LENGTH_HEADERS + FIXED_LENGTH +
           REGISTER_HEADER;
}

const char* Paper::serialize() {
    unsigned short int register_size = get_register_size();
    Buffer buffer(register_size);

    buffer.write_2byte_number(register_size);
    buffer.write_4byte_number(id);
    buffer.write_4byte_number(citations);
    buffer.write_2byte_number(year);
    buffer.write_fixed_length_string(updated_at, 20);
    buffer.write_varchar(title);
    buffer.write_varchar(authors);
    buffer.write_varchar(snippet);

    return buffer.get_buffer_bytes();
};

void Paper::deserialize(unsigned char* buffer, int buffer_offset) {
    Buffer buffer_reader(buffer, 4096);
    buffer_reader.jump_n_bytes_from_current_position(2 + buffer_offset);

    id = buffer_reader.read_4byte_number();
    citations = buffer_reader.read_4byte_number();
    year = buffer_reader.read_2byte_number();
    updated_at = buffer_reader.read_fixed_length_string(20);
    title = buffer_reader.read_varchar();
    authors = buffer_reader.read_varchar();
    snippet = buffer_reader.read_varchar();
}