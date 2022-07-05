#define BLOCK_SIZE 4096
#define BLOCK_METADATA_OVERHEAD 2

/**
 * Os blocos tem 2 bytes de overhead para marcar a quantidade de bytes ocupados.
 * Como os blocos nao tem registros espalhados, vai ter algum desperdicio de espaco.
 * Entao podemos usar esses espacos para colocar metadados que vao ajudar a acelerar a aplicacao.
 * */
class BlockBuilder {
    unsigned char _internal_block_buffer[BLOCK_SIZE];
    unsigned short int _current_internal_buffer_length = 0;

    void append_to_internal_buffer(unsigned char* buffer, int buffer_length) {
        for (int i = 0; i < buffer_length; i++) {
            _internal_block_buffer[BLOCK_METADATA_OVERHEAD + _current_internal_buffer_length + i] = buffer[i];
        }

        _current_internal_buffer_length += buffer_length;
    }

    // coloca o tamanho ocupado no bloco nos dois primeiros bytes do buffer interno do bloco
    void update_block_header() {
        _internal_block_buffer[0] = _current_internal_buffer_length & 0xff;
        _internal_block_buffer[1] = (_current_internal_buffer_length >> 8) & 0xff;
    }

   public:
    // coloca esse buffer no final do buffer interno, nao deixando passar do tamanho do bloco, contando o overhead
    bool append_buffer_to_block(unsigned char* buffer, int buffer_length) {
        if (buffer_length + _current_internal_buffer_length > BLOCK_SIZE - BLOCK_METADATA_OVERHEAD) {
            return false;
        }

        append_to_internal_buffer(buffer, buffer_length);

        return true;
    }

    // retorna uma referencia constante para nao deixar corromper esse buffer, e coloca o tamanho alocado em dois bytes
    // no inicio do buffer
    const unsigned char* buildBlock() {
        update_block_header();

        return _internal_block_buffer;
    }
};