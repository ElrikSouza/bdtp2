echo "Compilando..."
g++ src/binary-file.cpp  \
src/buffer/buffer.cpp src/buffer/buffer-utils.cpp \
src/db/hash-file.cpp src/paper-block.cpp src/paper.cpp \
src/db/primary-index.cpp src/btree/bptree.cpp src/btree/bptree-internal-block.cpp \
src/btree/bptree-leaf-block.cpp src/btree/bptree-header-block.cpp src/btree/auxiliar-block.cpp \
src/commands/seek1.cpp -o seek1.o 

echo "Iniciando a busca por $1"
./seek1.o $1

rm seek1.o