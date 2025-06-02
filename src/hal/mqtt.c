#include "inc/mqtt.h"

static mqtt_client_t *client;
uint32_t last_timestamp = 0;
extern const uint8_t key[16];

/**
 * Função de callback para quando a conexão MQTT é estabelecida.
 *
 * @param client Ponteiro para o cliente MQTT.
 * @param arg Argumento adicional (não utilizado).
 * @param status Status da conexão.
 */
static void mqtt_connection_callback(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
    if (status == MQTT_CONNECT_ACCEPTED)
        printf("Conectado ao broker MQTT.\n");
    else
        printf("Falha ao conectar ao broker: %d\n", status);

    mqtt_set_inpub_callback(client, pub_cb, data_cb, NULL);
}

/**
 * Função para configurar o cliente MQTT.
 *
 * @param client_id ID do cliente MQTT.
 * @param broker_ip IP do broker MQTT.
 * @param user Nome de usuário (opcional).
 * @param pass Senha (opcional).
 */
void mqtt_setup(const char *client_id, const char *broker_ip, const char *user, const char *pass)
{
    ip_addr_t broker_addr;

    if (!ipaddr_aton(broker_ip, &broker_addr))
    {
        printf("Erro no IP.\n");
        return;
    }

    client = mqtt_client_new();
    if (!client)
    {
        printf("Falha ao criar cliente.\n");
        return;
    }

    struct mqtt_connect_client_info_t ci = {
        .client_id = client_id,
        .client_user = user,
        .client_pass = pass};

    mqtt_client_connect(client, &broker_addr, BROKER_PORT, mqtt_connection_callback, NULL, &ci);
}

/**
 * Callback de confirmação de publicação.
 *
 * @param arg Argumento adicional (não utilizado).
 * @param result Resultado da publicação.
 */
static void mqtt_pub_request_callback(void *arg, err_t result)
{
    if (result == ERR_OK)
    {
        printf("Mensagem publicada com sucesso.\n");
    }
    else
    {
        printf("Falha ao publicar mensagem: %d\n", result);
    }
}

/**
 * Função para publicar uma mensagem no tópico MQTT.
 *
 * @param topic Tópico onde a mensagem será publicada.
 * @param message Mensagem a ser publicada.
 * @param message_len Tamanho da mensagem.
 */
void mqtt_conn_publish(const char *topic, const char *message, size_t message_len, uint8_t qos, uint8_t retain)
{
    uint32_t now = time_us_32();
    char json_payload[256];

    int l = snprintf(json_payload, sizeof(json_payload),
             "{\"valor\":\"%s\", \"ts\": %lu}", message, now);

    if (l < 0)
    {
        printf("Erro ao formatar a mensagem com timestamp.\n");
        return;
    }

    err_t response = mqtt_publish(
        client,
        topic,
        json_payload,
        l,
        qos,    // QoS
        retain, // Retain
        mqtt_pub_request_callback,
        NULL);

    if (response != ERR_OK)
    {
        printf("Erro ao publicar mensagem: %d\n", response);
    }
}

static void pub_cb(void *arg, const char *topic, u32_t tot_len)
{
    printf("[MQTT] Mensagem recebida no tópico: %s\n", topic);
}

/* static size_t hex_to_bytes(const char *hex_string, uint8_t *byte_array, size_t max_len)
{
    size_t hex_len = strlen(hex_string);
    if (hex_len % 2 != 0)
    {
        printf("Erro: String hexadecimal com comprimento ímpar.\n");
        return 0;
    }

    size_t byte_len = hex_len / 2;
    if (byte_len > max_len)
    {
        printf("Erro: Buffer de bytes muito pequeno para a string hexadecimal.\n");
        return 0;
    }

    for (size_t i = 0; i < hex_len; i += 2)
    {
        sscanf(&hex_string[i], "%2hhX", &byte_array[i / 2]);
    }
    return byte_len;
} */

static void data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
    uint8_t decrypted[256];
    char received_value[100];
    uint32_t received_timestamp;

    aes_decrypt_message(data, decrypted, len, key);
    decrypted[len] = '\0';

    printf("[MQTT] Dados recebidos (%d bytes)\n", len);
    //printf("Encriptada: %s\n", data);
    //printf("Decriptada: %s\n", decrypted);

    if (sscanf((char *)decrypted, "{\"valor\":\"%99[^\"]\",\"ts\":%llu}", received_value, &received_timestamp) == 2)
    {
        if (received_timestamp > last_timestamp)
        {
            last_timestamp = received_timestamp;
            printf("Nova leitura válida: '%s' (ts: %llu)\n", received_value, received_timestamp);
        }
        else
        {
            printf("Replay detectado (timestamp recebido: %llu <= último timestamp válido: %llu)\n", received_timestamp, last_timestamp);
            // *** AQUI VOCÊ DEVE IGNORAR OU LIDAR COM O ATAQUE DE REPLAY ***
        }
    }
   /*  else if (1)
    {
        uint8_t received_bytes[256]; // Buffer para os bytes decodificados
        size_t decoded_len;

        printf("[MQTT] Dados recebidos (string HEX) (%d bytes): %s\n", len, data);

        // Converte a string hexadecimal recebida (data) para bytes reais
        decoded_len = hex_to_bytes((char *)data, received_bytes, sizeof(received_bytes));

        if (decoded_len == 0)
        {
            printf("Falha ao decodificar a string hexadecimal.\n");
            return;
        }

        aes_decrypt_message(received_bytes, decrypted, decoded_len, key);
        decrypted[decoded_len] = '\0'; // Garante null-termination se o texto original for menor

        printf("[MQTT] Dados decriptados: %s\n", decrypted);
    } */
    else
    {
        printf("Erro no parsing da mensagem decriptografada ou formato inválido.\n");
    }
}

void mqtt_conn_subscribe(const char *topic, uint8_t qos)
{
    err_t err = mqtt_subscribe(client, topic, qos, NULL, NULL);
    if (err != ERR_OK)
        printf("Erro ao se inscrever no tópico '%s': %d\n", topic, err);
    else
        printf("Inscrito no tópico '%s'.\n", topic);
}
