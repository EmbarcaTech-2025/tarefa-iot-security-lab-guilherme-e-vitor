#ifndef WIFI_H
#define WIFI_H

#include <stdio.h>
#include "pico/cyw43_arch.h"

#define WIFI_SSID "Boboy_2.4GHz" 
#define WIFI_PASSWORD "13zb0276"

int connect_wifi(const char *ssid, const char *password);

#endif