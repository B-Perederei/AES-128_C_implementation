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

void encrypt_decrypt_file(void (*encr_decr_function)(), uint8_t key[4][4], char *input_file, char *result_file)
{
    FILE *fileptr_input, *fileptr_result;
    uint8_t current_block[4][4];
    unsigned long long filelen;
    unsigned long long number_of_matrixes;
    int not_full_matrix_elements;
    fileptr_input = fopen(input_file, "rb");
    fileptr_result = fopen(result_file, "wb");
    // Getting length of file
    fseek(fileptr_input, 0, SEEK_END);
    filelen = ftell(fileptr_input);
    rewind(fileptr_input);
    // Getting number of whole 2D arrays and number of elements of not whole 2D array
    number_of_matrixes = filelen / 16;
    not_full_matrix_elements = filelen % 16;
    for (unsigned long long i_file = 0; i_file < number_of_matrixes; i_file++)
    {
        // During fread elements are putting row by row, not column by column
        fread(current_block, sizeof(uint8_t), 16, fileptr_input);
        encr_decr_function(current_block, key);
        fwrite(current_block, sizeof(uint8_t), 16, fileptr_result);
    }
    if (not_full_matrix_elements != 0)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                // Making zero matrix
                current_block[i][j] = 0;
            }
        }
        fread(current_block, sizeof(uint8_t), not_full_matrix_elements, fileptr_input);
        encr_decr_function(current_block, key);
        fwrite(current_block, sizeof(uint8_t), 16, fileptr_result);
    }
    fclose(fileptr_input);
    fclose(fileptr_result);
}

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