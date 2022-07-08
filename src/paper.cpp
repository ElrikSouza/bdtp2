#include "paper.h"

#include <iostream>
#include <string>

#include "buffer-utils.h"

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
    return (unsigned short int)title.length() + snippet.length() + authors.length() + LENGTH_HEADERS + FIXED_LENGTH;
}

const char* Paper::serialize() {
    unsigned short int register_size = get_register_size();
    unsigned short int buffer_len = register_size + REGISTER_HEADER;
    char* buffer = new char[buffer_len];

    int bytes_written = 0;

    write_2byte_number_to_buffer(buffer, buffer_len, bytes_written);
    bytes_written += 2;

    write_4byte_number_to_buffer(buffer, id, bytes_written);
    bytes_written += 4;

    write_4byte_number_to_buffer(buffer, citations, bytes_written);
    bytes_written += 4;

    write_2byte_number_to_buffer(buffer, year, bytes_written);
    bytes_written += 2;

    write_fixed_length_string(buffer, updated_at, bytes_written);
    bytes_written += 20;

    bytes_written += write_varchar_to_buffer(buffer, title, bytes_written);

    bytes_written += write_varchar_to_buffer(buffer, authors, bytes_written);

    bytes_written += write_varchar_to_buffer(buffer, snippet, bytes_written);

    return buffer;
};

void Paper::deserialize(unsigned char* buffer, int buffer_offset) {
    int bytes_read = 2;

    id = read_4byte_number_from_buffer(buffer, buffer_offset + bytes_read);
    bytes_read += 4;

    citations = read_4byte_number_from_buffer(buffer, buffer_offset + bytes_read);
    bytes_read += 4;

    year = read_2byte_number_from_buffer(buffer, buffer_offset + bytes_read);
    bytes_read += 2;

    updated_at = read_fixed_length_string_from_buffer((char*)buffer, 20, buffer_offset + bytes_read);
    bytes_read += updated_at.length();

    title = read_varchar_from_buffer((char*)buffer, buffer_offset + bytes_read);
    bytes_read += title.length() + 2;

    authors = read_varchar_from_buffer((char*)buffer, buffer_offset + bytes_read);
    bytes_read += authors.length() + 2;

    snippet = read_varchar_from_buffer((char*)buffer, buffer_offset + bytes_read);
}