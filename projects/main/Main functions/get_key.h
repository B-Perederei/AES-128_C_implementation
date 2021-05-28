#ifndef GET_KEY_H
#define GET_KEY_H

void calc_sha_256(uint8_t hash[32], const void *input, size_t len);
void get_key(char *password, uint8_t key[4][4]);

#endif