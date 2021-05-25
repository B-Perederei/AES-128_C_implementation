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

char *inputString(FILE *fp, size_t size)
{
    // Function for reading string of any len
    // It uses dynamic memory
    char *str;
    int ch;
    size_t len = 0;
    str = realloc(NULL, sizeof(*str) * size);
    if (!str)
    {
        return str;
    }

    while (EOF != (ch = fgetc(fp)) && ch != '\n')
    {
        str[len++] = ch;
        if (len == size)
        {
            str = realloc(str, sizeof(*str) * (size += 16));
            if (!str)
            {
                return str;
            }
        }
    }
    str[len++] = '\0';
    return realloc(str, sizeof(*str) * len);
}

int check_text_similarity(char *text_1, char *text_2)
{
    // Code for comparing two lines
    if ((strlen(text_1) == strlen(text_2)) && (strcmp(text_1, text_2) == 0))
    {
        return 1;
    }
    return 0;
}


int main(void)
{
    clock_t start_of_action = 0;
    clock_t end_of_action = 0;
    float time_for_operation = 0;
    void(*encrypting_block_ptr)();
    void(*decrypting_block_ptr)();
    // Pointers to functions of encrypting or decrypting
    encrypting_block_ptr = encrypting_block;
    decrypting_block_ptr = decrypting_block;
    // Srting variables needed in console
    char *current_input;
    char *input_file;
    char *result_file;
    char *password;
    uint8_t key[4][4];
    FILE *fileptr_input;
    // Function for console output and input
    printf("Welcome to the program! Here you can encrypt and decrypt your text files ");
    printf("to keep it save in your PC!\n\n");
    printf("Commands:\n/encrypt to encrypt file;\n/decrypt to decrypt file;\n");
    printf("/help to get information about the program;\n/exit to exit program.\n\n");
    while (1)
    {
        while (1)
        {
            printf("Enter operation: ");
            current_input = inputString(stdin, 10);
            if (check_text_similarity(current_input, "/encrypt"))
            {
                free(current_input);
                while (1)
                {
                    printf("Enter name of file which you want to encrypt: ");
                    input_file = inputString(stdin, 10);
                    fileptr_input = fopen(input_file, "rb");
                    if (fileptr_input == NULL)
                    {
                        // File doesn't exist
                        printf("Could not open file %s, check if file is in folder ", input_file);
                        printf("where program is containing and try again!\n");
                        free(input_file);
                        continue;
                    }
                    fclose(fileptr_input);
                    printf("Attention! If file, where result will be written, contains information, it will be deleted!\n");
                    printf("If file output file doesn't exist, it will be created.\n");
                    printf("Enter name of file which will contain the result of encryption: ");
                    result_file = inputString(stdin, 10);
                    break;
                }
                while (1)
                {
                    printf("Enter your password: ");
                    password = inputString(stdin, 16);
                    if (strlen(password) < 8)
                    {
                        printf("Password must be at least 8 characters long!\n");
                        free(password);
                        continue;
                    }
                    get_key(password, key);
                    free(password);
                    break;
                }
                printf("\n\nEncrypting started, it could take some time...\n\n");
                start_of_action = clock();
                encrypt_decrypt_file(encrypting_block_ptr, key, input_file, result_file);
                end_of_action = clock();
                time_for_operation = (end_of_action - start_of_action) / (CLOCKS_PER_SEC / (1000));
                printf("\n\nFile was encrypted! Result is in %s file. ", result_file);
                printf("Time for encrypting file: %0.3lf seconds.\n", time_for_operation / 1000);
                // Deleting key for not storing it in memory
                for (int i = 0; i < 4; i++)
                {
                    for (int j = 0; j < 4; j++)
                    {
                        key[i][j] = 0;
                    }
                }
                free(result_file);
                free(input_file);
            }
            else if (check_text_similarity(current_input, "/decrypt"))
            {
                free(current_input);
                while (1)
                {
                    printf("Enter name of file which you want to decrypt: ");
                    input_file = inputString(stdin, 10);
                    fileptr_input = fopen(input_file, "rb");
                    if (fileptr_input == NULL)
                    {
                        // File doesn't exist
                        printf("Could not open file %s, check if file is in folder ", input_file);
                        printf("where program is containing and try again!\n");
                        free(input_file);
                        continue;
                    }
                    fclose(fileptr_input);
                    printf("Attention! If file, where result will be written, ");
                    printf("contains information, it will be deleted!\n");
                    printf("If file output file doesn't exist, it will be created.\n");
                    printf("Enter name of file which will contain the result of decryption: ");
                    result_file = inputString(stdin, 10);
                    break;
                }
                while (1)
                {
                    printf("Enter your password: ");
                    password = inputString(stdin, 16);
                    if (strlen(password) < 8)
                    {
                        printf("Password must be at least 8 characters long!\n");
                        free(password);
                        continue;
                    }
                    // Getting hash of password
                    get_key(password, key);
                    free(password);
                    break;
                }
                printf("\n\nDecrypting started, it could take some time...\n");
                start_of_action = clock();
                encrypt_decrypt_file(decrypting_block_ptr, key, input_file, result_file);
                end_of_action = clock();
                time_for_operation = (end_of_action - start_of_action) / (CLOCKS_PER_SEC / (1000));
                printf("\n\nFile was decrypted! Result is in %s file. ", result_file);
                printf("Time for decrypting file: %0.3lf seconds\n", time_for_operation / 1000);
                // Deleting key for not storing it in memory
                for (int i = 0; i < 4; i++)
                {
                    for (int j = 0; j < 4; j++)
                    {
                        key[i][j] = 0;
                    }
                }
                free(result_file);
                free(input_file);
            }
            else if (check_text_similarity(current_input, "/help"))
            {
                free(current_input);
                // Giving main inforamtion about the program
                printf("This program encrypts and decrypts text files. ");
                printf("It uses AES-128 algorithm to keep your text data in safety.\n");
                printf("First, you need to save file to the folder where program is.\n");
                printf("Then you can choose one from two operations (encrypt/decrypt), enter name of files ");
                printf("which you want to encrypt/decrypt and your password (better memorize it).\n");
                printf("In the end of the process you will get encrypted or decrypted file.\n");
            }
            else if (check_text_similarity(current_input, "/exit"))
            {
                free(current_input);
                return 0;
            }
            else
            {
                // Entered not valid command
                free(current_input);
                printf("Don't understand that...\n");
            }
        }
    }
    return 0;
}
