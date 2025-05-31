# IoT_security

1. Para rodar o broker e permitir conexões externas, utilize o comando:

```bash
mosquitto -c /etc/mosquitto/mosquitto.conf -v
```
> O arquivo de configuração padrão do mosquitto (`/etc/mosquitto/mosquitto.conf`) deve estar configurado:

```bash
listener 1883
allow_anonymous true
```