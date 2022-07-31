echo "Compilando..."
g++ src/binary-file.cpp  \
src/buffer/buffer.cpp src/buffer/buffer-utils.cpp \
src/db/hash-file.cpp src/paper-block.cpp src/paper.cpp src/parsing/paper-stream.cpp \
src/parsing/parser.cpp \
src/btree/bptree-header-block.cpp src/btree/bptree-internal-block.cpp \
src/btree/bptree-leaf-block.cpp src/btree/bptree.cpp src/btree/auxiliar-block.cpp \
src/db/primary-index.cpp \
src/commands/upload.cpp -o upload.o 

echo "Iniciando o upload"
./upload.o $1

echo "Arquivos criados."
rm upload.o