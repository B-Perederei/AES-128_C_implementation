#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <getopt.h>
// File with functions for decrypting and encrypting
#include "Main functions/encr_decr_block.h"
#include "Main functions/get_key.h"

void encrypt_file(char *input_file, char *result_file, uint8_t key[4][4])
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
    fclose(fileptr_input);
    fclose(fileptr_result);
}

void decrypt_file(char *input_file, char *result_file, uint8_t key[4][4])
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
    // Getting number of whole 2D arrays and number of elements of not whole 2D array
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


void output_main_commands(void)
{
    printf("Welcome to the program! Here you can encrypt and decrypt your text files ");
    printf("to keep it save in your PC!\n\n");
    printf("Commands:\n/encrypt to encrypt file;\n/decrypt to decrypt file;\n");
    printf("/help to get information about the program;\n/exit to exit program.\n\n");
}

void output_help_information(void)
{
    printf("This program encrypts and decrypts text files. ");
    printf("It uses AES-128 algorithm to keep your text data in safety.\n");
    printf("First, you need to save file to the folder where program is.\n");
    printf("Then you can choose one from two operations (encrypt/decrypt), enter name of files ");
    printf("which you want to encrypt/decrypt and your password (better memorize it).\n");
    printf("In the end of the process you will get encrypted or decrypted file.\n");
}

void enter_password(uint8_t key[4][4])
{
    char *password;
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
}

void enter_data_and_encrypting_file(void)
{
    char *input_file;
    char *result_file;
    uint8_t key[4][4];
    clock_t start_of_action = 0;
    clock_t end_of_action = 0;
    float time_for_operation = 0;
    FILE *fileptr_input;
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
    enter_password(key);
    printf("\n\nEncryption started, it could take some time...\n");
    start_of_action = clock();
    encrypt_file(input_file, result_file, key); 
    end_of_action = clock();
    time_for_operation = (end_of_action - start_of_action) / (CLOCKS_PER_SEC / (1000));
    printf("\n\nFile was encrypted! Result is in %s file. ", result_file);
    printf("Time for encrypting file: %0.3lf seconds\n", time_for_operation / 1000);
    free(result_file);
    free(input_file);
    // Deleting key for not storing it in memory
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            key[i][j] = 0;
        }
    }
}

void entering_data_and_decrypting_file(void)
{
    char *input_file;
    char *result_file;
    uint8_t key[4][4];
    clock_t start_of_action = 0;
    clock_t end_of_action = 0;
    float time_for_operation = 0;
    FILE *fileptr_input;
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
        printf("Attention! If file, where result will be written, contains information, it will be deleted!\n");
        printf("If file output file doesn't exist, it will be created.\n");
        printf("Enter name of file which will contain the result of decryption: ");
        result_file = inputString(stdin, 10);
        break;
    }
    enter_password(key);
    printf("\n\nDecrypting started, it could take some time...\n");
    start_of_action = clock();
    decrypt_file(input_file, result_file, key); 
    end_of_action = clock();
    time_for_operation = (end_of_action - start_of_action) / (CLOCKS_PER_SEC / (1000));
    printf("\n\nFile was decrypted! Result is in %s file. ", result_file);
    printf("Time for decrypting file: %0.3lf seconds\n", time_for_operation / 1000);
    free(result_file);
    free(input_file);
    // Deleting key for not storing it in memory
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            key[i][j] = 0;
        }
    }
}


int main(void)
{
    // Srting variables needed in console
    char *current_input;
    output_main_commands();
    while (1)
    {
        while (1)
        {
            printf("Enter operation: ");
            current_input = inputString(stdin, 10);
            if (check_text_similarity(current_input, "/encrypt"))
            {
                free(current_input);
                enter_data_and_encrypting_file();
            }
            else if (check_text_similarity(current_input, "/decrypt"))
            {
                free(current_input);
                entering_data_and_decrypting_file();
            }
            else if (check_text_similarity(current_input, "/help"))
            {
                free(current_input);
                // Giving main inforamtion about the program
                output_help_information();
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
