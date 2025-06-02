#ifndef CRYPTO_H
#define CRYPTO_H

#include "pico/stdlib.h"
#include "inc/aes.h"
#include "string.h"

void xor_encrypt_message(const uint8_t *message, uint8_t *encrypted_message, size_t message_len, uint8_t key);
size_t aes_encrypt_message(const uint8_t *message, uint8_t *encrypted_message, size_t message_len, const uint8_t *key);
void aes_decrypt_message(const uint8_t *encrypted, uint8_t *decrypted, size_t encrypted_len, const uint8_t *key);

#endif