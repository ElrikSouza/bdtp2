#include <iostream>

#pragma once

/**
 * Essa classe envelopa e abstrai operações comuns em buffers.
 *
 * Ela possui um cursor interno com incremento automático na escrita, que permite escrita de
 * dados em sequencia sem colisões, bem como pular para qualquer posição do buffer para leitura ou escrita.
 * */
class Buffer {
   private:
    char* _internal_buffer;
    int _buffer_capacity;
    int _current_position;
    bool already_free = false;

   public:
    Buffer(int buffer_capacity);
    Buffer(unsigned char* buffer, int buffer_length);

    int get_current_cursor_position();

    // verifica se o cursor está na posição igual à capacidade do buffer
    bool has_ended();

    void jump_n_bytes_from_current_position(int bytes);
    void jump_to_absolute_position(int position_in_bytes);
    void jump_to_the_start();

    // escreve um unsigned short in e pula 2 bytes
    void write_2byte_number(unsigned short int number);

    // escreve um unsigned int e pula 4 bytes
    void write_4byte_number(unsigned int number);

    // escreve uma string de tamanho X e pula X bytes
    void write_fixed_length_string(std::string word, int length);

    // escreve 2 bytes com o tamanho da string, a string, e pula 2 bytes + tamanho da string
    void write_varchar(std::string varchar);

    // escreve n bytes e pula n bytes
    void write_bytes(char* bytes, int num_of_bytes);

    unsigned short int read_2byte_number();
    unsigned int read_4byte_number();
    std::string read_varchar();
    std::string read_fixed_length_string(int length);

    // expoe o buffer interno
    char* get_buffer_bytes();

    // desaloca o conteudo do buffer interno se ele já não foi desalocado
    void free();
};