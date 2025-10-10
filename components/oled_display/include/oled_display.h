// #pragma once

// #include "esp_err.h"

// #ifdef __cplusplus
// extern "C" {
// #endif

// /**
//  * @brief Initialize the OLED display and its I²C bus.
//  *
//  * Powers up Vext, resets the OLED, initializes I²C and sends the SSD1306 init sequence.
//  *
//  * @return ESP_OK on success, or an error code on failure.
//  */
// esp_err_t oled_init(void);

// /**
//  * @brief Clear the OLED display.
//  *
//  * Fills the display with zeros.
//  *
//  * @return ESP_OK on success.
//  */
// esp_err_t oled_clear(void);

// /**
//  * @brief Turn on external power (Vext).
//  */
// void oled_vext_on(void);

// /**
//  * @brief Turn off external power (Vext).
//  */
// void oled_vext_off(void);

// /**
//  * @brief (Optional) Scan I²C bus and print detected devices to log.
//  *
//  * Use only for debugging. Not required in production.
//  */
// void oled_i2c_scan(void);

// #ifdef __cplusplus
// }
// #endif



#pragma once
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

// Initialize OLED (powers Vext, resets panel, sets up I2C, sends SSD1306 init).
esp_err_t oled_init(void);

// Clear screen (fills 128x64 with zeros).
esp_err_t oled_clear(void);

// Optional: quick I2C bus scan (logs found addresses).
void oled_i2c_scan(void);

// Vext control (LOW = ON, HIGH = OFF on Heltec WiFi LoRa V3).
void oled_vext_on(void);
void oled_vext_off(void);


/**
 * @brief Draw ASCII text at (x, page)
 *
 * @param x       Horizontal pixel position (0–127)
 * @param page    Page number (0–7), each page is 8 pixels tall
 * @param text    Null-terminated string to draw
 * @return ESP_OK on success
 */
esp_err_t oled_draw_text(uint8_t x, uint8_t page, const char *text);
#ifdef __cplusplus
}
#endif
