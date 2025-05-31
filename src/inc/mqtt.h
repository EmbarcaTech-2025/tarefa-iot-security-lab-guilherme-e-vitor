#ifndef MQTT_H
#define MQTT_H

#include "lwip/apps/mqtt.h"
#include "lwipopts.h"
#include "aes.h"
#include "string.h"

#define BROKER_PORT 1883
#define CLIENT_ID "pico_client"
#define BROKER_IP "192.168.1.113"
#define MQTT_USER "user1"
#define MQTT_PASS "trt567"

void mqtt_setup(const char *client_id, const char *broker_ip, const char *user, const char *pass);
void mqtt_conn_publish(const char *topic, const char *message, size_t message_len, uint8_t qos, uint8_t retain);
void mqtt_conn_subscribe(const char *topic, uint8_t qos);
static void pub_cb(void *arg, const char *topic, u32_t tot_len);
static void data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags);
void xor_encrypt_message(const uint8_t *message, uint8_t *encrypted_message, size_t message_len, uint8_t key);
size_t aes_encrypt_message(const uint8_t *message, uint8_t *encrypted_message, size_t message_len, const uint8_t *key);
void aes_decrypt_message(const uint8_t *encrypted, uint8_t *decrypted, size_t encrypted_len, const uint8_t *key);

#endif