#pragma once

// arquivo que seria um .env com as configurações do código

#define BLOCK_SIZE 4096  // 4kb
#define BUCKETS 400000   // 400k
#define BLOCKS_PER_BUCKET 1
#define HASH_FILE_NAME "hash-data-file.bin"
#define PRIMARY_INDEX_NAME "primary-index.bin"

#define BPTREE_MAX_KEYS_PER_BLOCK 510
#define BPTREE_MIN_KEYS_PER_BLOCK 255
#define BPTREE_HEADER_SIZE 12
#define BPTREE_INDEX_VALUE_PAIR_SIZE 8
#define BPTREE_LAST_INDEX 4092