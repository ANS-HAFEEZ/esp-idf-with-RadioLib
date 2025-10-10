/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#include <RadioLib.h>
#include "hal/ESP32S3Hal/Esp32S3Hal.hpp"   // include your HAL implementation
#include <oled_display.h>
#include "driver/i2c_master.h"
#include "esp_log.h"


#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"


#define RADIO_NSS   (8)   // LoRa CS
#define RADIO_IRQ   (14)  // DIO1
#define RADIO_RST   (12)  // Reset
#define RADIO_GPIO  (13)  // Busy

#define RADIO_SCK   (9)
#define RADIO_MISO  (11)
#define RADIO_MOSI  (10)








extern "C" void app_main(void)
{
  //  // Create HAL and module
  // Esp32S3Hal hal(RADIO_SCK, RADIO_MISO, RADIO_MOSI);
  // Module mod(&hal, RADIO_NSS, RADIO_IRQ, RADIO_RST, RADIO_GPIO);
  // SX1262 radio(&mod);

  // printf("\n[RadioLib] SX1262 LoRa test starting...\n");

  // // Initialize radio (returns RADIOLIB_ERR_NONE if OK)
  // int16_t state = radio.begin();
  // if (state == RADIOLIB_ERR_NONE) {
  //   printf("Radio initialized successfully!\n");
  // } else {
  //   printf("Radio init failed, code = %d\n", state);
  //   return;   // stop here
  // }

  // // Configure LoRa parameters (optional — default is 434 MHz, 125 kHz BW)
  // radio.setFrequency(868.0);       // adjust for your region (868 MHz EU / 915 MHz US)
  // radio.setBandwidth(125.0);
  // radio.setSpreadingFactor(9);
  // radio.setCodingRate(7);          // 4/7
  // radio.setOutputPower(14);        // dBm

  // // // Send a test packet once every 3 s
  // // for (;;) {
  // //   state = radio.transmit("Hello from ESP32-S3!");
  // //   if (state == RADIOLIB_ERR_NONE) {
  // //     printf("Packet sent successfully!\n");
  // //   } else {
  // //     printf("Transmit failed, code = %d\n", state);
  // //   }

  // //   vTaskDelay(pdMS_TO_TICKS(3000));  // wait 3 s
  // // }



  //   uint8_t buffer[256];   // enough for typical LoRa packets

  //   while (true) {
  //       memset(buffer, 0, sizeof(buffer));
  //       state = radio.receive(buffer, sizeof(buffer), 5000);  // timeout 5s

  //       if (state == RADIOLIB_ERR_NONE) {
  //           printf("Received packet: %s\n", buffer);
  //           printf("RSSI: %.1f dBm, SNR: %.2f dB\n",
  //                   radio.getRSSI(), radio.getSNR());
  //       } else if (state == RADIOLIB_ERR_RX_TIMEOUT) {
  //           printf("No packet received (timeout)\n");
  //       } else {
  //           printf("Receive failed, code = %d\n", state);
  //       }

  //       vTaskDelay(pdMS_TO_TICKS(1000));
  //   }



    // ESP_ERROR_CHECK(oled_init());
    // oled_clear();
    // oled_draw_text(0, 0, "Hello ESP-IDF!", 10);
    // oled_display();

    // while (1) {
    //     vTaskDelay(pdMS_TO_TICKS(5000));
    // }
    // ESP_ERROR_CHECK(oled_init());
    // oled_i2c_scan();     // Optional — debug only
    // oled_clear();

    // while (1) {
    //     vTaskDelay(pdMS_TO_TICKS(5000));
    // }

    ESP_ERROR_CHECK(oled_init());
    oled_clear();

    // Draw text: page = vertical block of 8 pixels
    oled_draw_text(0, 0, "Hello ESP-IDF!");
    oled_draw_text(0, 2, "LoRa Ready");

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(5000));
    }

}
