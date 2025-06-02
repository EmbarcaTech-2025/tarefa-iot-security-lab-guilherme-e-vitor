#include <stdio.h>
#include "pico/stdlib.h"
#include "lwipopts.h"
#include "inc/wifi.h"
#include "inc/mqtt.h"
#include "inc/crypto.h"

#define G_LED 11
#define R_LED 13
#define A 5
#define B 6

static bool last_a_state = true;
static bool last_b_state = true;
const uint8_t key[16] = "minha_chave_1234";

void print_hex(const uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        printf("%02X", data[i]);
    }
    printf("\n");
}

int main()
{
    stdio_init_all();

    // LEDs
    gpio_init(R_LED);
    gpio_set_dir(R_LED, GPIO_OUT);
    gpio_init(G_LED);
    gpio_set_dir(G_LED, GPIO_OUT);

    // Buttons
    gpio_init(A);
    gpio_set_dir(A, GPIO_IN);
    gpio_pull_up(A);

    gpio_init(B);
    gpio_set_dir(B, GPIO_IN);
    gpio_pull_up(B);

    sleep_ms(3000);

    int r = connect_wifi(WIFI_SSID, WIFI_PASSWORD);
    sleep_ms(500);

    if (r > 0)
    {
        gpio_put(G_LED, 1);
        sleep_ms(1000);
        gpio_put(G_LED, 0);
        mqtt_setup(CLIENT_ID, BROKER_IP, MQTT_USER, MQTT_PASS);
    }
    else
    {
        gpio_put(R_LED, 1);
        sleep_ms(1000);
        gpio_put(R_LED, 0);
    }

    sleep_ms(1000);

    const char *message = "Hello MQTT!";

    while (true)
    {
        bool cur_a = gpio_get(A);
        bool cur_b = gpio_get(B);

        if (!cur_a && last_a_state)
        {
            // send message
            uint8_t encrypted[64];
            // uint8_t decrypted[64];

            size_t len = aes_encrypt_message(message, encrypted, strlen(message), key);

            printf("Mensagem encriptada (HEX): \n");
            print_hex(encrypted, len);

            mqtt_conn_publish("test/topic/west", encrypted, len, 2, 1);
        }
        else if (!cur_b && last_b_state)
        {
            mqtt_conn_subscribe("test/topic/vitor", 0);
            printf("Esperando mensagem no tópico 'test/topic/vitor'...\n");
        }
        last_a_state = cur_a;
        last_b_state = cur_b;
        sleep_ms(50);
    }
}
