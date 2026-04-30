/*
 * Example program to use nec_ir functionality with elrebo-de/esp_nec_ir
 */

#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "nec_ir.hpp"
#include "generic_button.hpp"
#include <iot_button.h>


static const char *tag = "nec ir test";

bool state = false;

// Callback function for BUTTON_SINGLE_CLICK event from onBoardButton
extern "C" void callback_onBoardButton_BUTTON_SINGLE_CLICK(void *arg, void *data)
{
    ESP_LOGI("onBoardButton Callback", "for Event BUTTON_SINGLE_CLICK called!");

    iot_button_print_event((button_handle_t)arg);

    // bei jedem BUTTON_SINGLE_CLICK wird der state umgeschaltet
    state = !state;
    NecIr* necIr = &necIr->getInstance(); // get the Singleton instance
    if (!state) {
        necIr->transmitNecCommandFrame(0x857a, 0x7c03); // "TV Scene"
    }
    else {
        necIr->transmitNecCommandFrame(0x817e, 0xd52a); // "Power 0/1"
    }
}

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

    GenericButton onBoardButton(
	    std::string("onBoardButton"),
	    /* M5 Atom Lite */
	    (gpio_num_t) 39, // GPIO
	    0, // active = DOWN
	    true, // pull disabled - M5 Atom does not support internal PU/PD on this gpio
	    std::string("GPIO")
	);

    onBoardButton.RegisterCallbackForEvent(BUTTON_SINGLE_CLICK, callback_onBoardButton_BUTTON_SINGLE_CLICK);

    // transmitter test
    //ESP_LOGI(tag, "transmitNecCommandFrame");
    //necIr->transmitNecCommandFrame(0x857a, 0x7c03); // "TV Scene"
    //vTaskDelay(pdMS_TO_TICKS(1000)); // delay 1 second
    //necIr->transmitNecRepeatFrame();

    //vTaskDelay(pdMS_TO_TICKS(30000)); // delay 30 seconds

    //necIr->transmitNecCommandFrame(0x817e, 0xd52a); // "Power 0/1"
    //vTaskDelay(pdMS_TO_TICKS(1000)); // delay 1 second
    //necIr->transmitNecRepeatFrame();

    // receiver test
    //while(1) {
    //    ESP_LOGI(tag, "receiveNecFrame");
    //    necIr->receiveNecFrame();

    while(1) {
        vTaskDelay(pdMS_TO_TICKS(3000)); // delay 30 second
    }

}
