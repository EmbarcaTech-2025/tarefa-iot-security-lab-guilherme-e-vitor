#ifndef WIFI_H
#define WIFI_H

#include <stdio.h>
#include "pico/cyw43_arch.h"

#define WIFI_SSID "AP-ACCESS BLH" 
#define WIFI_PASSWORD "Fin@ApointBlH"

int connect_wifi(const char *ssid, const char *password);

#endif