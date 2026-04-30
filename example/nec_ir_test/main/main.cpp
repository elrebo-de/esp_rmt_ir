/*
 * Example program to use nec_ir functionality with elrebo-de/esp_nec_ir
 */

#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "nec_ir.hpp"

static const char *tag = "nec ir test";

extern "C" void app_main(void)
{
    // short delay to reconnect logging
    vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds

    ESP_LOGI(tag, "Example Program");

    /* Initialize NecIr class */
    ESP_LOGI(tag, "NecIr");
    NecIr* necIr = &necIr->getInstance(); // get the Singleton instance
    necIr->setGpioPins(12,26); // set the GPIO pins
    necIr->initialize(); // initialize NEC IR RMT


    // transmitter test
    ESP_LOGI(tag, "transmitNecCommandFrame");
    necIr->transmitNecCommandFrame(0x857a, 0x7c03); // "TV Scene"
    vTaskDelay(pdMS_TO_TICKS(1000)); // delay 1 second
    necIr->transmitNecRepeatFrame();

    vTaskDelay(pdMS_TO_TICKS(30000)); // delay 30 seconds

    necIr->transmitNecCommandFrame(0x817e, 0xd52a); // "Power 0/1"
    vTaskDelay(pdMS_TO_TICKS(1000)); // delay 1 second
    necIr->transmitNecRepeatFrame();


    // receiver test
    while(1) {
        ESP_LOGI(tag, "receiveNecFrame");
        necIr->receiveNecFrame();
    }

}
