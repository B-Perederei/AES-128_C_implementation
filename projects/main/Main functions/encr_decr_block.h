#ifndef ENCR_DECR_BLOCK_H
#define ENCR_DECR_BLOCK_H

void encrypting_block(uint8_t state[4][4], uint8_t key[4][4]);
void decrypting_block(uint8_t encrypted_message[4][4], uint8_t key[4][4]);

#endif