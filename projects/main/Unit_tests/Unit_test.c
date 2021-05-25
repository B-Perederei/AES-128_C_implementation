#include <stdio.h>
#include <stdint.h>
#include "encr_decr_block.h"
#include "get_key.h"

size_t str_len_in_bytes(const char *s)
{
    // Function for getting length of text in bytes
    // Needed for getting length for SHA-256 algortihm 
    size_t length = 0;
    while (*s++ != '\0')
    {
        length++;
    }
    return length;
}

int main()
{
    // Testing encrypting and decrypting
    int encrypt_test = 1;
    int decrypt_test = 1; 
    // Testing encrypring 
    uint8_t testing_example[4][4] = {{0x32, 0x88, 0x31, 0xe0},
        {0x43, 0x5a, 0x31, 0x37},
        {0xf6, 0x30, 0x98, 0x07},
        {0xa8, 0x8d, 0xa2, 0x34}
    };

    uint8_t state[4][4] = {{0x32, 0x88, 0x31, 0xe0},
        {0x43, 0x5a, 0x31, 0x37},
        {0xf6, 0x30, 0x98, 0x07},
        {0xa8, 0x8d, 0xa2, 0x34}
    };

    uint8_t key[4][4] = {{0x2b, 0x28, 0xab, 0x09},
        {0x7e, 0xae, 0xf7, 0xcf},
        {0x15, 0xd2, 0x15, 0x4f},
        {0x16, 0xa6, 0x88, 0x3c}
    };

    uint8_t encrypted_message[4][4] = {{0x39, 0x02, 0xdc, 0x19},
        {0x25, 0xdc, 0x11, 0x6a},
        {0x84, 0x09, 0x85, 0x0b},
        {0x1d, 0xfb, 0x97, 0x32}
    };

    encrypting_block(testing_example, key);
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (testing_example[i][j] != encrypted_message[i][j])
            {
                encrypt_test = 0;
                break;
            }
        }
        if (encrypt_test == 0)
        {
            printf("Encrypted test failed!\n");
            printf("Result of the encryption:\n");
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    printf("%x ", testing_example[i][j]);
                }
                printf("\n");
            }
            printf("Needed result:\n");
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    printf("%x ", encrypted_message[i][j]);
                }
                printf("\n");
            }
            return 0;
        }
    }
    printf("Encryption test passed!\n\n");
    
    // Testing decrypring 
    decrypting_block(testing_example, key);
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (testing_example[i][j] != state[i][j])
            {
                decrypt_test = 0;
                break;
            }
        }
        if (decrypt_test == 0)
        {
            printf("Decrypted test failed!\n");
            printf("Result of the decryption:\n");
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    printf("%x ", testing_example[i][j]);
                }
                printf("\n");
            }
            printf("Needed result:\n");
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    printf("%x ", state[i][j]);
                }
                printf("\n");
            }
            return 0;
        }
    }
    printf("Decryption test passed!\n\n");
    
    // Testing SHA-256
    // Testing phrase "test_to_check"
    int hash_test = 1;
    uint8_t hash[32];
    uint8_t hash_needed[32] = {0xcf, 0xd9, 0x9a, 0x51, 0x1e, 0xbd, 0x8b, 0xf1, 0x3f, 0x81, 0x9d, 0x01, 0x72, 0x52, 0x6e, 0x7b, 
                               0xfc, 0x1b, 0x4c, 0x44, 0xf7, 0x1c, 0xe4, 0x81, 0xd9, 0x9d, 0x05, 0x6a, 0x47, 0xb3, 0x8a, 0x02
                              };
    calc_sha_256(hash, "test_to_check", str_len_in_bytes("test_to_check"));
    for (int i = 0; i < 32; i++)
    {
        if (hash[i] != hash_needed[i])
        {
            hash_test = 0;
            break;
        }
    }
    if (hash_test == 0)
    {
        printf("Hash test failed!\n");
        printf("Result:        ");
        for (int i = 0; i < 32; i++)
        {
            printf("%x ", hash[i]);
        }
        printf("\n");
        printf("Needed result: ");
        for (int i = 0; i < 32; i++)
        {
            printf("%x ", hash_needed[i]);
        }
        return 0;
    }
    printf("Hash test passed!\n");
    getchar();
    return 0;
}