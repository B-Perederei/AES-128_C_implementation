#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
// File with functions for decrypting and encrypting and getting key
#include "Main functions/encr_decr_block.h"
#include "Main functions/get_key.h"
// Logging files
#include "Main functions/logging.h"
#include "Main functions/global.h"
#include "Main functions/ini.h"

// Function for logging file infromation
#define INI_FILE_NAME "main.ini"

// Functions needed for logging
static int handler(void *user, const char *section, const char *name,
                   const char *value)
{
    configuration *pconfig = (configuration *)user;

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("logging", "loglevel"))
    {
        pconfig->log_level = atoi(value);
    }
    else if (MATCH("logging", "logfilepath"))
    {
        if (!(pconfig->log_file = fopen(value, "a")))
        {
            pconfig->log_file = stderr;
            LOG(LOGLEVEL_ERROR, "Failed to open log file '%s' for write. Log output redirested to stderr", value);
        }
    }
    else
    {
        return false;  /* unknown section/name, error */
    }
    return true;
}

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
    LOG(LOGLEVEL_DEBUG, "Opend input file '%s'", input_file);
    fileptr_result = fopen(result_file, "wb");
    if (fileptr_result == NULL)
    {
        LOG(LOGLEVEL_ERROR, "Can't create and open output file for encryption with name '%s'", result_file);
    }
    LOG(LOGLEVEL_DEBUG, "Created and open output file '%s'", result_file);
    // Getting length of file
    fseek(fileptr_input, 0, SEEK_END);
    filelen = ftell(fileptr_input);
    rewind(fileptr_input);
    // Getting number of whole 2D arrays and number of elements of not whole 2D array
    number_of_matrixes = filelen / 16;
    not_full_matrix_elements = filelen % 16;
    LOG(LOGLEVEL_DEBUG, "Start of encryption of main body of the file");
    for (unsigned long long i_file = 0; i_file < number_of_matrixes; i_file++)
    {
        // During fread elements are putting row by row, not column by column
        fread(current_block, sizeof(uint8_t), 16, fileptr_input);
        encrypting_block(current_block, key);
        fwrite(current_block, sizeof(uint8_t), 16, fileptr_result);
    }
    LOG(LOGLEVEL_DEBUG, "End of encryption of main body of the file");
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
        LOG(LOGLEVEL_DEBUG, "Encrypted last not full block");
    }
    if (filelen == 0)
    {
        LOG(LOGLEVEL_WARN, "Entered file for encryption is empty");
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
    LOG(LOGLEVEL_DEBUG, "Size of the last block is %d bytes", read_bytes_from_file);
    // Last block contains number of bytes, which don't fit
    current_block[0][0] = read_bytes_from_file;
    encrypting_block(current_block, key);
    fwrite(current_block, sizeof(uint8_t), 16, fileptr_result);
    LOG(LOGLEVEL_DEBUG, "Last block with %d bytes encrypted and written", read_bytes_from_file);
    // Closing files
    fclose(fileptr_input);
    LOG(LOGLEVEL_DEBUG, "Close input file '%s'", input_file);
    fclose(fileptr_result);
    LOG(LOGLEVEL_DEBUG, "Close output file '%s'", result_file);
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
    if (fileptr_result == NULL)
    {
        LOG(LOGLEVEL_ERROR, "Can't create and open output file for decryption with name '%s'", result_file);
    }
    LOG(LOGLEVEL_DEBUG, "Created and open output file '%s'", result_file);
    // Getting length of file
    fseek(fileptr_input, 0, SEEK_END);
    filelen = ftell(fileptr_input);
    rewind(fileptr_input);
    // Getting number of whole 2D arrays
    number_of_matrixes = filelen / 16;
    if (number_of_matrixes > 2)
    {
        LOG(LOGLEVEL_DEBUG, "Started decrypting main body of the file");
        for (unsigned long long i_file = 0; i_file < number_of_matrixes - 2; i_file++)
        {
            // During fread elements are putting row by row, not column by column
            fread(current_block, sizeof(uint8_t), 16, fileptr_input);
            decrypting_block(current_block, key);
            fwrite(current_block, sizeof(uint8_t), 16, fileptr_result);
        }
        LOG(LOGLEVEL_DEBUG, "Ended decrypting main body of the file");
    }
    // Reading last block
    fread(current_block, sizeof(uint8_t), 16, fileptr_input);
    decrypting_block(current_block, key);
    // Block which contains numbers of bytes of not full matrix
    read_bytes_from_file = fread(buffer_block, sizeof(uint8_t), 16, fileptr_input);
    if (read_bytes_from_file == 0)
    {
        LOG(LOGLEVEL_WARN, "Decrypted file is empty");
    }
    else
    {
        decrypting_block(buffer_block, key);
        LOG(LOGLEVEL_DEBUG, "Got %d bytes which needed to put in file", buffer_block[0][0]);
        // Writing needed bytes to file
        fwrite(current_block, sizeof(uint8_t), buffer_block[0][0], fileptr_result);
    }
    LOG(LOGLEVEL_DEBUG, "Close input file '%s'", input_file);
    fclose(fileptr_input);
    LOG(LOGLEVEL_DEBUG, "Close output file '%s'", result_file);
    fclose(fileptr_result);
}

int check_text_similarity(char *text_1, char *text_2)
{
    // Code for comparing two lines
    if ((strlen(text_1) == strlen(text_2)) && (strcmp(text_1, text_2) == 0))
    {
        return 1;
    }
    LOG(LOGLEVEL_DEBUG, "Enetered command '%s' doesn't match command '%s'", text_1, text_2);
    return 0;
}

void output_help_information(void)
{
    // Giving info about the application
    printf("This program encrypts and decrypts text files. ");
    printf("It uses AES-128 algorithm to keep your text data in safety.\n");
    printf("First, you need to save file to the folder where program is.\n");
    printf("Result of the operation will be in the same folder.\n");
    // Giving main information about commands avaible
    printf("Commands:\n");
    printf("/encrypt input_file.txt output_file.txt qwerty123 - encrypt file with name ");
    printf("'input_file.txt' and save result in 'output_file.txt'm using password 'qwerty123'. ");
    printf("Password should be at least 8 characters long.\n"); 
    printf("/decrypt input_file.txt output_file.txt qwerty123 - decrypt file with name ");
    printf("'input_file.txt' and save result in 'output_file.txt'm using password 'qwerty123'. ");
    printf("Password should be at least 8 characters long.\n"); 
    printf("/help - get help");
}

void preparation_to_ecnrypting_and_encrypting(char *input_file, char *result_file, char *password)
{
    uint8_t key[4][4];
    clock_t start_of_action = 0;
    clock_t end_of_action = 0;
    float time_for_operation = 0;
    get_key(password, key);
    printf("\n\nEncryption started, it could take some time...\n");
    LOG(LOGLEVEL_INFO, "Start of encryption file %s, result in %s", input_file, result_file);
    // Start encrypting and measure time
    start_of_action = clock();
    encrypt_file(input_file, result_file, key);
    end_of_action = clock();
    LOG(LOGLEVEL_INFO, "End of encryption file %s, result in %s", input_file, result_file);
    // Getting time for encrypting operation
    time_for_operation = (end_of_action - start_of_action) / (CLOCKS_PER_SEC / (1000));
    LOG(LOGLEVEL_INFO, "Time for encryption: %0.3lf seconds", time_for_operation / 1000);
    printf("\n\nFile was encrypted! Result is in %s file. ", result_file);
    printf("Time for encrypting file: %0.3lf seconds\n", time_for_operation / 1000);
    // Deleting key for not storing it in memory
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            key[i][j] = 0;
        }
    }
}

void preparation_to_decrypting_and_decrypting(char *input_file, char *result_file, char *password)
{
    uint8_t key[4][4];
    clock_t start_of_action = 0;
    clock_t end_of_action = 0;
    float time_for_operation = 0;
    get_key(password, key);
    printf("\n\nDecryption started, it could take some time...\n");
    LOG(LOGLEVEL_INFO, "Start of decryption file %s, result in %s", input_file, result_file);
    // Start decrypting and measure time
    start_of_action = clock();
    decrypt_file(input_file, result_file, key);
    end_of_action = clock();
    LOG(LOGLEVEL_INFO, "End of decryption file %s, result in %s", input_file, result_file);
    // Getting time for encrypting operation
    time_for_operation = (end_of_action - start_of_action) / (CLOCKS_PER_SEC / (1000));
    LOG(LOGLEVEL_INFO, "Time for decryption: %0.3lf seconds", time_for_operation / 1000);
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
}


int main(int argc, char *argv[])
{
    FILE *fileptr_input;
    if (ini_parse(INI_FILE_NAME, handler, &globalConfig) < 0)
    {
        printf("Can't load '%s'\n", INI_FILE_NAME);
        return 1;
    }
    LOG(LOGLEVEL_INFO, "Start program");
    if (argc < 2)
    {
        // Not enough elemetns from cmd line for operations
        printf("Please, enter valid command");
        LOG(LOGLEVEL_WARN, "Not enough elements in cmd line");
        // Logging end of the program
        LOG(LOGLEVEL_INFO, "End program");
        if (globalConfig.log_file)
        {
            fclose(globalConfig.log_file);
        }
        return 0;
    }
    if (check_text_similarity(argv[1], "/encrypt") && argc == 5)
    {
        LOG(LOGLEVEL_INFO, "Entered /ecnrypt command");
        fileptr_input = fopen(argv[2], "rb");
        if (fileptr_input == NULL)
        {
            // File doesn't exist
            LOG(LOGLEVEL_ERROR, "Tried to ecnrypt file which doesn't exist");
            printf("Could not open file %s, check if file is in folder ", argv[2]);
            printf("where program is containing and try again!\n");
        }
        else if (strlen(argv[4]) < 8)
        {
            // Too short password
            LOG(LOGLEVEL_INFO, "Entered password smaller than 8 characters long");
            printf("Password must be at least 8 characters long!\n");
        }
        else
        {
            preparation_to_ecnrypting_and_encrypting(argv[2], argv[3], argv[4]);
        }
    }
    else if (check_text_similarity(argv[1], "/decrypt") && argc == 5)
    {
        LOG(LOGLEVEL_INFO, "Entered /decrypt command");
        fileptr_input = fopen(argv[2], "rb");
        if (fileptr_input == NULL)
        {
            LOG(LOGLEVEL_ERROR, "Tried to decrypt file which doesn't exist");
            // File doesn't exist
            printf("Could not open file %s, check if file is in folder ", argv[2]);
            printf("where program is containing and try again!\n");
        }
        else if (strlen(argv[4]) < 8)
        {
            // Too short password
            LOG(LOGLEVEL_INFO, "Entered password smaller than 8 characters long");
            printf("Password must be at least 8 characters long!\n");
        }
        else
        {
            preparation_to_decrypting_and_decrypting(argv[2], argv[3], argv[4]);
        }
    }
    else if (check_text_similarity(argv[1], "/help") && argc == 2)
    {
        // Giving help info
        LOG(LOGLEVEL_INFO, "Entered /help command");
        output_help_information();
    }
    else
    {
        // Not valid input from cmd
        LOG(LOGLEVEL_WARN, "Entered not valid command or given too many/not enough arguments.\n");
        printf("Entered not valid operation or given too many/not enough arguments, enter /help to check possible operation.");
    }
    // Free entered command
    for (int i = 0; i < argc; i++)
    {
        free(argv[i]);
    }
    free(argv);
    // Logging end of the program
    LOG(LOGLEVEL_INFO, "End program");
    if (globalConfig.log_file)
    {
        fclose(globalConfig.log_file);
    }
    return 0;
}
