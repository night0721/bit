#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

/* You can use another sha256 implementation if you want */
#include "sha256.h"

#define DEFAULT_BLOCKS 1024
#define MAX_BLOCK_DATA 1024
#define DIFFICULTY 2
#define FILENAME "log.bit"

typedef struct {
    int index;
    char previous_hash[65];
    char diff[MAX_BLOCK_DATA];
    char timestamp[20];
	int nonce;
    char hash[65];
} block_t;

void print_block(block_t *b)
{
    printf("Block %d\n", b->index);
    printf("Previous Hash: %s\n", b->previous_hash);
    printf("Diff         : %s\n", b->diff);
    printf("Timestamp    : %s\n", b->timestamp);
    printf("Nonce        : %d\n", b->nonce);
    printf("Hash         : %s\n\n", b->hash);
}

void calculate_hash(block_t *b, char *output)
{
    unsigned char hash_bytes[32];
    char input[MAX_BLOCK_DATA + 88];
    
    snprintf(input, sizeof(input), "%d%s%s%s%d", b->index, b->previous_hash,
			b->diff, b->timestamp, b->nonce);
    
    sha256(input, strlen(input), hash_bytes);

	/* Convert to hex */
    for (int i = 0; i < 32; i++) {
        sprintf(output + (i * 2), "%02x", hash_bytes[i]);
    }
    output[64] = '\0';
}

/*
 * Stimulate mining block to find valid hash
 */
void mine_block(block_t *b)
{
    char target[DIFFICULTY + 1];
	/* Adjust DIFFICULTY for the hash to start with 0s */
    memset(target, '0', DIFFICULTY);
    target[DIFFICULTY] = '\0';

    while (1) {
        calculate_hash(b, b->hash);
        if (strncmp(b->hash, target, DIFFICULTY) == 0)
            break;
        b->nonce++;
    }
}

block_t *create_genesis_block(void)
{
    block_t *genesis = malloc(sizeof(block_t));
    genesis->index = 1;
	/* No previous hash */
    strcpy(genesis->previous_hash, "0");
	/* Example initial commit diff */
    strcpy(genesis->diff, "+ print(\"Hello World\")");
    time_t now = time(NULL);
    strftime(genesis->timestamp, sizeof(genesis->timestamp),
			"%Y-%m-%d %H:%M:%S", localtime(&now));
	genesis->nonce = 0;
    mine_block(genesis);
    return genesis;
}

block_t *create_block(block_t *previous, const char *diff)
{
    block_t *new_block = malloc(sizeof(block_t));
    new_block->index = previous->index + 1;
    strcpy(new_block->previous_hash, previous->hash);
    strncpy(new_block->diff, diff, MAX_BLOCK_DATA);
    time_t now = time(NULL);
    strftime(new_block->timestamp, sizeof(new_block->timestamp),
			"%Y-%m-%d %H:%M:%S", localtime(&now));
    new_block->nonce = 0;
    mine_block(new_block);
    return new_block;
}

void save_blockchain(block_t *block)
{
    FILE *file = fopen(FILENAME, "a");
    fprintf(file, "%d,%s,%s,%s,%d,%s\n", block->index, block->previous_hash,
			block->diff, block->timestamp, block->nonce, block->hash);
    fclose(file);
}

block_t *get_blocks(int *num)
{
	FILE *file = fopen(FILENAME, "r");
    if (!file) {
		perror("Blockchain file not found");
        return NULL;
    }

	block_t *blocks = malloc(sizeof(block_t) * DEFAULT_BLOCKS);
	int block_num = 0;
    char line[MAX_BLOCK_DATA + 153];
    while (fgets(line, sizeof(line), file)) {
		block_t current_block;
        char *token;
        token = strtok(line, ",");
        current_block.index = atoi(token);

        token = strtok(NULL, ",");
        strcpy(current_block.previous_hash, token);

        token = strtok(NULL, ",");
        strcpy(current_block.diff, token);

        token = strtok(NULL, ",");
        strcpy(current_block.timestamp, token);

		token = strtok(NULL, ",");
        current_block.nonce = atoi(token);

        token = strtok(NULL, ",");
        strcpy(current_block.hash, token);
        
		current_block.hash[strcspn(current_block.hash, "\n")] = 0;
		blocks[block_num++] = current_block;
    }
	*num = block_num;
	fclose(file);
	return blocks;
}

block_t *create_blockchain(void)
{
    block_t *genesis = create_genesis_block();
    print_block(genesis);
    save_blockchain(genesis);
	return genesis;
}

void verify_blockchain(block_t *blocks, int block_num)
{
	for (int i = 0; i < block_num; i++) {
		char calculated_hash[65];
        calculate_hash(&blocks[i], calculated_hash);
		if (strncmp(blocks[i].hash, calculated_hash, 65) != 0) {
            printf("Hash mismatch in block %d\n", blocks[i].index);
			printf("Blockchain is invalid!\n");
			return;
        }
	}
    printf("Blockchain is valid!\n");
}

block_t *add_block(block_t *last_block)
{
    char diff[MAX_BLOCK_DATA];
    
    if (!fgets(diff, MAX_BLOCK_DATA, stdin)) {
        fprintf(stderr, "Error reading diff from stdin\n");
        return NULL;
    }

    diff[strcspn(diff, "\n")] = '\0';

    block_t *new_block = create_block(last_block, diff);
    print_block(new_block);
    save_blockchain(new_block);
    return new_block;
}

void help(int code)
{
	fprintf(stderr, "bit create|verify|add\n");
	exit(code);
}
int main(int argc, char **argv)
{
	if (argc != 2) {
		help(1);
	}
	if (!strncmp(argv[1], "create", 6)) {
		create_blockchain();
	} else if (!strncmp(argv[1], "verify", 6)) {
		int block_num;
		block_t *blocks = get_blocks(&block_num);
		verify_blockchain(blocks, block_num);
	} else if (!strncmp(argv[1], "add", 3)) {
		int block_num;
		block_t *blocks = get_blocks(&block_num);
		add_block(&blocks[block_num - 1]);
	} else {
		help(0);
	}
    return 0;
}
