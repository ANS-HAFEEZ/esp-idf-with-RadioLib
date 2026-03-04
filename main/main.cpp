/*
    * LoRaWAN OTAA example for ESP32-S3 + SX1262 using RadioLib
    * Fill in the credential macros below before flashing.
    */
#include <RadioLib.h>
#include "hal/ESP32S3Hal/Esp32S3Hal.hpp"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// ==== LoRaWAN credentials — fill these in ====
// TODO: replace with your actual credentials from TTN / ChirpStack / etc.
#define APPEUI     0x0000000000000000ULL  // 8 bytes MSB
#define DEVEUI     0x0000000000000000ULL  // 8 bytes MSB
#define APPKEY     { 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, \
                                    0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00 }
// For LoRaWAN 1.0.x NwkKey == AppKey; for 1.1 set separately
#define NWKKEY     APPKEY

// ==== SPI / radio pins ====
#define RADIO_NSS   (8)
#define RADIO_IRQ   (14)
#define RADIO_RST   (12)
#define RADIO_GPIO  (13)
#define RADIO_SCK   (9)
#define RADIO_MISO  (11)
#define RADIO_MOSI  (10)

// ==== NVS storage keys ====
#define NVS_NAMESPACE    "lorawan"
#define NVS_KEY_SESSION  "session"
#define NVS_KEY_NONCES   "nonces"

// ==== Uplink interval ====
#define UPLINK_INTERVAL_MS  (30000)

static const char *TAG = "LoRaWAN";

// ---------------------------------------------------------------------------
// NVS helpers
// ---------------------------------------------------------------------------

static bool nvs_load_blob(const char *key, uint8_t *buf, size_t size) {
    ESP_LOGI(TAG, "%s", __func__);
    nvs_handle_t h;
    if (nvs_open(NVS_NAMESPACE, NVS_READONLY, &h) != ESP_OK) return false;
    size_t len = size;
    bool ok = (nvs_get_blob(h, key, buf, &len) == ESP_OK && len == size);
    nvs_close(h);
    return ok;
}

static bool nvs_save_blob(const char *key, const uint8_t *buf, size_t size) {
    ESP_LOGI(TAG, "%s", __func__);
    nvs_handle_t h;
    if (nvs_open(NVS_NAMESPACE, NVS_READWRITE, &h) != ESP_OK) return false;
    bool ok = (nvs_set_blob(h, key, buf, size) == ESP_OK &&
                        nvs_commit(h) == ESP_OK);
    nvs_close(h);
    return ok;
}

// ---------------------------------------------------------------------------
// app_main
// ---------------------------------------------------------------------------

extern "C" void app_main(void) {
    ESP_LOGI(TAG, "%s", __func__);

    // ---- NVS init ----
    esp_err_t nvsErr = nvs_flash_init();
    if (nvsErr == ESP_ERR_NVS_NO_FREE_PAGES || nvsErr == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    // ---- Radio init ----
    Esp32S3Hal hal(RADIO_SCK, RADIO_MISO, RADIO_MOSI);
    Module mod(&hal, RADIO_NSS, RADIO_IRQ, RADIO_RST, RADIO_GPIO);
    SX1262 radio(&mod);

    int16_t state = radio.begin();
    if (state == RADIOLIB_ERR_NONE) {
        ESP_LOGI(TAG, "Radio initialized OK");
    } else {
        ESP_LOGE(TAG, "Radio init failed: %d", state);
        while (true) vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // ---- LoRaWAN node ----
    LoRaWANNode node(&radio, &EU868);

    uint8_t appKey[] = APPKEY;
    uint8_t nwkKey[] = NWKKEY;

    state = node.beginOTAA(APPEUI, DEVEUI, nwkKey, appKey);
    if (state != RADIOLIB_ERR_NONE) {
        ESP_LOGE(TAG, "beginOTAA failed: %d", state);
        while (true) vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // ---- Restore persisted nonces (prevents DevNonce reuse) ----
    uint8_t nonceBuf[RADIOLIB_LORAWAN_NONCES_BUF_SIZE] = {0};
    if (nvs_load_blob(NVS_KEY_NONCES, nonceBuf, RADIOLIB_LORAWAN_NONCES_BUF_SIZE)) {
        state = node.setBufferNonces(nonceBuf);
        if (state == RADIOLIB_ERR_NONE) {
            ESP_LOGI(TAG, "Nonces restored from NVS");
        }
    }

    // ---- Restore persisted session (skip re-join if valid) ----
    uint8_t sessionBuf[RADIOLIB_LORAWAN_SESSION_BUF_SIZE] = {0};
    if (nvs_load_blob(NVS_KEY_SESSION, sessionBuf, RADIOLIB_LORAWAN_SESSION_BUF_SIZE)) {
        state = node.setBufferSession(sessionBuf);
        if (state == RADIOLIB_ERR_NONE) {
            ESP_LOGI(TAG, "Session restored from NVS");
        }
    }

    // ---- OTAA join loop ----
    while (true) {
        ESP_LOGI(TAG, "Joining LoRaWAN network (OTAA)...");
        state = node.activateOTAA();

        if (state == RADIOLIB_LORAWAN_SESSION_RESTORED) {
            ESP_LOGI(TAG, "Session restored — skipping join");
            break;
        }

        // DevNonce was incremented by activateOTAA — save it now regardless of
        // outcome so a reboot cannot replay the same nonce and get rejected.
        nvs_save_blob(NVS_KEY_NONCES, node.getBufferNonces(), RADIOLIB_LORAWAN_NONCES_BUF_SIZE);

        if (state == RADIOLIB_LORAWAN_NEW_SESSION) {
            ESP_LOGI(TAG, "Joined! New session established");
            nvs_save_blob(NVS_KEY_SESSION, node.getBufferSession(), RADIOLIB_LORAWAN_SESSION_BUF_SIZE);
            break;
        }
        if (state == RADIOLIB_ERR_NO_JOIN_ACCEPT) {
            ESP_LOGW(TAG, "No Join Accept received — retrying in 5s");
        } else {
            ESP_LOGE(TAG, "activateOTAA error: %d — retrying in 5s", state);
        }
        vTaskDelay(pdMS_TO_TICKS(5000));
    }

    // ---- Uplink loop ----
    uint32_t counter = 0;
    uint8_t downBuf[RADIOLIB_LORAWAN_MAX_DOWNLINK_SIZE + 1] = {0};
    size_t  downLen = 0;
    char msg[64];

    while (true) {
        snprintf(msg, sizeof(msg), "Hello #%lu", (unsigned long)counter++);
        ESP_LOGI(TAG, "Uplink #%lu: %s", (unsigned long)(counter - 1), msg);

        downLen = sizeof(downBuf) - 1;
        state = node.sendReceive(
            reinterpret_cast<uint8_t*>(msg), strlen(msg),
            /*fPort=*/1,
            downBuf, &downLen);

        if (state > 0) {
            // state = Rx window number (1=RX1, 2=RX2) when a downlink was received
            ESP_LOGI(TAG, "Uplink sent OK, downlink received (%u bytes)", (unsigned)downLen);
            downBuf[downLen] = '\0';
            ESP_LOGI(TAG, "Downlink: %s", (char*)downBuf);
        } else if (state == RADIOLIB_ERR_NONE) {
            // state = 0 means uplink sent OK, no downlink
            ESP_LOGI(TAG, "Uplink sent OK, no downlink");
        } else {
            ESP_LOGE(TAG, "sendReceive error: %d", state);
        }

        // Persist updated frame counters after every uplink
        nvs_save_blob(NVS_KEY_NONCES,  node.getBufferNonces(),  RADIOLIB_LORAWAN_NONCES_BUF_SIZE);
        nvs_save_blob(NVS_KEY_SESSION, node.getBufferSession(), RADIOLIB_LORAWAN_SESSION_BUF_SIZE);

        vTaskDelay(pdMS_TO_TICKS(UPLINK_INTERVAL_MS));
    }
}
