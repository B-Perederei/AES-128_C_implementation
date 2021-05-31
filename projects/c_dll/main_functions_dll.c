#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#define CHUNK_SIZE 64
#define TOTAL_LEN_LEN 8
// File with functions for decrypting and encrypting
#include "../main/Main functions/encr_decr_block.h"
#include "../main/Main functions/get_key.h"

void encrypt_process(char *input_file, char *result_file, uint8_t key[4][4])
{
    FILE *fileptr_input, *fileptr_result;
    uint8_t current_block[4][4];
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            // Making zero matrix
            current_block[i][j] = 0;
        }
    }
    unsigned long long filelen;
    unsigned long long number_of_matrixes;
    int not_full_matrix_elements;
    int read_bytes_from_file = 16;
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
        encrypting_block(current_block, key);
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
        read_bytes_from_file = fread(current_block, sizeof(uint8_t), not_full_matrix_elements, fileptr_input);
        encrypting_block(current_block, key);
        fwrite(current_block, sizeof(uint8_t), 16, fileptr_result);
    }
    if (filelen == 0)
    {
        read_bytes_from_file = 0;
    }
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            // Making zero matrix
            current_block[i][j] = 0;
        }
    }
    // Last block contains number of bytes, which don't fit
    current_block[0][0] = read_bytes_from_file;
    encrypting_block(current_block, key);
    fwrite(current_block, sizeof(uint8_t), 16, fileptr_result);
    // Closing files
    fclose(fileptr_input);
    fclose(fileptr_result);
}

void decrypt_process(char *input_file, char *result_file, uint8_t key[4][4])
{
    FILE *fileptr_input, *fileptr_result;
    uint8_t current_block[4][4];
    uint8_t buffer_block[4][4];
    unsigned long long filelen;
    int read_bytes_from_file;
    unsigned long long number_of_matrixes;
    fileptr_input = fopen(input_file, "rb");
    fileptr_result = fopen(result_file, "wb");
    // Getting length of file
    fseek(fileptr_input, 0, SEEK_END);
    filelen = ftell(fileptr_input);
    rewind(fileptr_input);
    // Getting number of whole 2D arrays
    number_of_matrixes = filelen / 16;
    if (number_of_matrixes > 2)
    {
        for (unsigned long long i_file = 0; i_file < number_of_matrixes - 2; i_file++)
        {
            // During fread elements are putting row by row, not column by column
            fread(current_block, sizeof(uint8_t), 16, fileptr_input);
            decrypting_block(current_block, key);
            fwrite(current_block, sizeof(uint8_t), 16, fileptr_result);
        }
    }
    // Reading last block
    fread(current_block, sizeof(uint8_t), 16, fileptr_input);
    decrypting_block(current_block, key);
    // Block which contains numbers of bytes of not full matrix
    read_bytes_from_file = fread(buffer_block, sizeof(uint8_t), 16, fileptr_input);
    if (read_bytes_from_file == 0)
    {
        fwrite(current_block, sizeof(uint8_t), 16, fileptr_result);
    }
    else
    {
        decrypting_block(buffer_block, key);
        // Writing needed bytes to file
        fwrite(current_block, sizeof(uint8_t), buffer_block[0][0], fileptr_result);
    }
    fclose(fileptr_input);
    fclose(fileptr_result);
}

int encrypt_file(char *input_file, char *result_file, char *password)
{
    FILE *fileptr_input;
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
    encrypt_process(input_file, result_file, key);
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
    decrypt_process(input_file, result_file, key);
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            key[i][j] = 0;
        }
    }
    return 0;
}