g++ src/binary-file.cpp src/buffer/buffer.cpp src/buffer/buffer-utils.cpp src/db/hash-file.cpp \
src/paper-block.cpp src/paper.cpp src/parsing/paper-stream.cpp src/parsing/parser.cpp \
test/hash-file.test.cpp -o hash.test

./hash.test

rm hash.test
rm test.hex
