/*
 * Example program to use rmt_ir functionality with elrebo-de/esp_rmt_ir
 */

#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "rmt_ir.hpp"
#include "generic_button.hpp"
#include <iot_button.h>


static const char *tag = "rmt ir test";

bool state = false;

// Callback function for BUTTON_SINGLE_CLICK event from onBoardButton
extern "C" void callback_onBoardButton_BUTTON_SINGLE_CLICK(void *arg, void *data)
{
    ESP_LOGI("onBoardButton Callback", "for Event BUTTON_SINGLE_CLICK called!");

    iot_button_print_event((button_handle_t)arg);

    // bei jedem BUTTON_SINGLE_CLICK wird der state umgeschaltet
    state = !state;
    RmtIr* rmtIr = &rmtIr->getInstance(); // get the Singleton instance
    if (!state) {
        rmtIr->transmitNecCommandFrame(0x857a, 0x7c03); // "TV Scene"
    }
    else {
        rmtIr->transmitNecCommandFrame(0x817e, 0xd52a); // "Power 0/1"
    }
}

// Callback function for BUTTON_DOUBLE_CLICK event from onBoardButton
extern "C" void callback_onBoardButton_BUTTON_DOUBLE_CLICK(void *arg, void *data)
{
    ESP_LOGI("onBoardButton Callback", "for Event BUTTON_DOUBLE_CLICK called!");

    iot_button_print_event((button_handle_t)arg);

    // bei jedem BUTTON_DOUBLE_CLICK wird der state umgeschaltet
    state = !state;
    RmtIr* rmtIr = &rmtIr->getInstance(); // get the Singleton instance
    rmtIr->transmitNecCommandFrame(0x817e, 0xd52a); // "Power 0/1"
    vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds
    rmtIr->transmitNecCommandFrame(0x857a, 0xe916); // "Tuner"
}

extern "C" void app_main(void)
{
    // short delay to reconnect logging
    vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds

    ESP_LOGI(tag, "Example Program");

    /* Initialize NecIr class */
    ESP_LOGI(tag, "NecIr");
    RmtIr* rmtIr = &rmtIr->getInstance(); // get the Singleton instance
    rmtIr->setGpioPins(12,26); // set the GPIO pins
    rmtIr->initialize(); // initialize RMT IR

    GenericButton onBoardButton(
	    std::string("onBoardButton"),
	    /* M5 Atom Lite */
	    (gpio_num_t) 39, // GPIO
	    0, // active = DOWN
	    true, // pull disabled - M5 Atom does not support internal PU/PD on this gpio
	    std::string("GPIO")
	);

    onBoardButton.RegisterCallbackForEvent(BUTTON_SINGLE_CLICK, callback_onBoardButton_BUTTON_SINGLE_CLICK);
    onBoardButton.RegisterCallbackForEvent(BUTTON_DOUBLE_CLICK, callback_onBoardButton_BUTTON_DOUBLE_CLICK);

    // transmitter test
    //ESP_LOGI(tag, "transmitNecCommandFrame");
    //rmtIr->transmitNecCommandFrame(0x857a, 0x7c03); // "TV Scene"
    //vTaskDelay(pdMS_TO_TICKS(1000)); // delay 1 second
    //rmtIr->transmitNecRepeatFrame();

    //vTaskDelay(pdMS_TO_TICKS(30000)); // delay 30 seconds

    //rmtIr->transmitNecCommandFrame(0x817e, 0xd52a); // "Power 0/1"
    //vTaskDelay(pdMS_TO_TICKS(1000)); // delay 1 second
    //rmtIr->transmitNecRepeatFrame();

    // receiver test
    while(1) {
        ESP_LOGI(tag, "receiveNecOrPanasonicFrame");
        rmtIr->receiveNecOrPanasonicFrame();
    }

}
