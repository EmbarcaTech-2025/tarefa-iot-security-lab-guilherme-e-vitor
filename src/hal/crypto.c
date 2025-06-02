#include "inc/crypto.h"

void xor_encrypt_message(const uint8_t *message, uint8_t *encrypted_message, size_t message_len, uint8_t key)
{
    for (size_t i = 0; i < message_len; i++)
        encrypted_message[i] = message[i] ^ key;
}

size_t aes_encrypt_message(const uint8_t *message, uint8_t *encrypted_message, size_t message_len, const uint8_t *key)
{
    struct AES_ctx ctx;
    AES_init_ctx(&ctx, key);

    size_t padded_len = ((message_len + 16 - 1) / 16) * 16;

    uint8_t buffer[padded_len];
    memset(buffer, 0, padded_len);
    memcpy(buffer, message, message_len);

    memcpy(encrypted_message, buffer, padded_len);

    for (size_t i = 0; i < message_len; i += 16)
    {
        AES_ECB_encrypt(&ctx, encrypted_message + i);
    }

    return padded_len;
}

void aes_decrypt_message(const uint8_t *encrypted, uint8_t *decrypted, size_t encrypted_len, const uint8_t *key)
{
    struct AES_ctx ctx;
    AES_init_ctx(&ctx, key);

    memcpy(decrypted, encrypted, encrypted_len);

    for (size_t i = 0; i < encrypted_len; i += 16)
        AES_ECB_decrypt(&ctx, decrypted + i);
}