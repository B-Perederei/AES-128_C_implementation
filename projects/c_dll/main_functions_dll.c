#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#define CHUNK_SIZE 64
#define TOTAL_LEN_LEN 8
// File with functions for decrypting and encrypting
#include "encr_decr_block.h"
#include "get_key.h"


int encrypt_file(char *input_file, char *result_file, char *password)
{
    FILE *fileptr_input;
    void(*encrypting_block_ptr)();
    encrypting_block_ptr = encrypting_block;
    uint8_t key[4][4];
    fileptr_input = fopen(input_file, "rb");
    if (fileptr_input == NULL)
    {
        // File doesn't exist
        return 1;        
    }
    fclose(fileptr_input);
    if (strlen(password) < 8)
    {
        // Checking password
        return 2;
    }
    // Encrypting process
    get_key(password, key);
    encrypt_decrypt_file(encrypting_block_ptr, key, input_file, result_file);
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            key[i][j] = 0;
        }
    }
    return 0;
}

int decrypt_file(char *input_file, char *result_file, char *password)
{
    FILE *fileptr_input;
    void(*decrypting_block_ptr)();
    decrypting_block_ptr = decrypting_block;
    uint8_t key[4][4];
    fileptr_input = fopen(input_file, "rb");
    if (fileptr_input == NULL)
    {
        // File doesn't exist
        return 1;        
    }
    fclose(fileptr_input);
    if (strlen(password) < 8)
    {
        // Checking password
        return 2;
    }
    // Encrypting process
    get_key(password, key);
    encrypt_decrypt_file(decrypting_block_ptr, key, input_file, result_file);
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            key[i][j] = 0;
        }
    }
    return 0;
}