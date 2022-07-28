g++ -g test/bptree-file.test.cpp src/btree/bptree-header-block.cpp src/btree/bptree-internal-block.cpp \
src/btree/bptree-leaf-block.cpp src/btree/bptree.cpp src/buffer/buffer-utils.cpp \
src/buffer/buffer.cpp src/binary-file.cpp -o bptree-test src/btree/auxiliar-block.cpp

./bptree-test

rm ./bptree-test