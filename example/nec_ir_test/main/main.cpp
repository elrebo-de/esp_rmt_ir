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

    timeSync->setSyncIntervalMs(30000); // syncIntervalMs set to 30 sec, default for normal operation is 5 mins

    xTaskCreate(timeTask, "timeTask", 4096, NULL, 5, NULL);

    while(!timeSync->isSynchronized()) {
        ESP_LOGI(tag, "time is not yet synchronized");
        vTaskDelay(pdMS_TO_TICKS(1000)); // delay 1 second
    }

    while(1) {
        ESP_LOGI(tag, "wait 10 seconds");
        vTaskDelay(pdMS_TO_TICKS(10000)); // delay 10 seconds
    }
}
