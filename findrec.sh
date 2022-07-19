echo "Compilando..."
g++ src/binary-file.cpp  \
src/buffer/buffer.cpp src/buffer/buffer-utils.cpp \
src/db/hash-file.cpp src/paper-block.cpp src/paper.cpp src/parsing/paper-stream.cpp \
src/parsing/parser.cpp \
src/commands/findrec.cpp -o findrec.o 

echo "Iniciando a busca por $1"
./findrec.o $1

rm findrec.o