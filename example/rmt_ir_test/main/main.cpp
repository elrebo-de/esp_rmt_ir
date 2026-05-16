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

void switchAllOff(RmtIr* rmtIr) {
        // YAMAHA Receiver
        rmtIr->transmitNecCommandFrame((uint8_t)0x7a, (uint8_t)0x1e); // "STANDBY"
        vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds
        // Panasonic TV
        rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x00, 0xfc); // "Power Off"
        vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds
        // Pioneer DVD Player
        rmtIr->transmitPioneerCommandFrame((uint8_t)0xa3, (uint8_t)0x99, (uint8_t)0xaf, (uint8_t)0xbb); // "Shift"
}

// Callback function for BUTTON_SINGLE_CLICK event from onBoardButton
extern "C" void callback_onBoardButton_BUTTON_SINGLE_CLICK(void *arg, void *data)
{
    ESP_LOGI("onBoardButton Callback", "for Event BUTTON_SINGLE_CLICK called!");

    iot_button_print_event((button_handle_t)arg);

    // bei jedem BUTTON_SINGLE_CLICK wird der Fernseher ein-/ausgeschaltet
    state = !state;
    ESP_LOGI(tag, "state = %d", state);
    RmtIr* rmtIr = &rmtIr->getInstance(); // get the Singleton instance
    if (!state) {
        switchAllOff(rmtIr);
    }
    else {
        // YAMAHA receiver
        rmtIr->transmitNecCommandFrame((uint16_t)0x7a85, (uint16_t)0x037c); // "TV Scene"
        vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds
        // Panasonic TV
        rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x00, 0x7c); // "Power On"
        vTaskDelay(pdMS_TO_TICKS(4000)); // delay 4 seconds
        rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x40, 0x0c); // "TV"
    }
}

// Callback function for BUTTON_DOUBLE_CLICK event from onBoardButton
extern "C" void callback_onBoardButton_BUTTON_DOUBLE_CLICK(void *arg, void *data)
{
    ESP_LOGI("onBoardButton Callback", "for Event BUTTON_DOUBLE_CLICK called!");

    iot_button_print_event((button_handle_t)arg);

    // bei jedem BUTTON_DOUBLE_CLICK wird der AppleTV ein-/ausgeschaltet
    state = !state;
    ESP_LOGI(tag, "state = %d", state);
    RmtIr* rmtIr = &rmtIr->getInstance(); // get the Singleton instance
    if (!state) {
        switchAllOff(rmtIr);
    }
    else {
        // YAMAHA receiver
        rmtIr->transmitNecCommandFrame((uint16_t)0x7a85, (uint16_t)0x0976); // "TV Scene"
        vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds
        // Panasonic TV
        rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x00, 0x7c); // "Power On"
        vTaskDelay(pdMS_TO_TICKS(2000)); // delay 2 seconds
        rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x00, 0xa0); // "AV" (HDMI1)
        vTaskDelay(pdMS_TO_TICKS(2000)); // delay 2 seconds
        rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x00, 0x92); // "OK"
    }
}

// Callback function for BUTTON_MULTIPLE_CLICK event from onBoardButton
extern "C" void callback_onBoardButton_BUTTON_MULTIPLE_CLICK_3(void *arg, void *data)
{
    ESP_LOGI("Button Callback", "for Event BUTTON_MULTIPLE_CLICK_3 called!");

    iot_button_print_event((button_handle_t)arg);

    // bei jedem BUTTON_MULTIPLE_CLICK_3 wird der DVD-Player ein-/ausgeschaltet
    state = !state;
    ESP_LOGI(tag, "state = %d", state);
    RmtIr* rmtIr = &rmtIr->getInstance(); // get the Singleton instance
    if (!state) {
        switchAllOff(rmtIr);
    }
    else {
        // YAMAHA Receiver
        rmtIr->transmitNecCommandFrame((uint16_t)0x7a85, (uint16_t)0x0976); // "BD/DVD Scene"
        vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds
        // Pioneer DVD Player
        rmtIr->transmitPioneerCommandFrame((uint8_t)0xa3, (uint8_t)0x99, (uint8_t)0xaf, (uint8_t)0xba); // "ON"
        vTaskDelay(pdMS_TO_TICKS(200)); // delay 0.05 seconds
        rmtIr->transmitPioneerCommandFrame((uint8_t)0xa3, (uint8_t)0x99, (uint8_t)0xaf, (uint8_t)0xb6); // "OPEN/CLOSE"
        vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds
        // Panasonic TV
        rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x00, 0x7c); // "Power On"
        vTaskDelay(pdMS_TO_TICKS(2000)); // delay 2 seconds
        rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x00, 0xa0); // "AV" (HDMI1)
        vTaskDelay(pdMS_TO_TICKS(2000)); // delay 2 seconds
        rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x00, 0x92); // "OK"
    }
}

// Callback function for BUTTON_LONG_PRESS_START event from onBoardButton
extern "C" void callback_onBoardButton_BUTTON_LONG_PRESS_START_1000(void *arg, void *data)
{
    ESP_LOGI("onBoardButton Callback", "for Event BUTTON_LONG_PRESS_START_1000 called!");

    iot_button_print_event((button_handle_t)arg);

    // bei jedem BUTTON_LONG_PRESS_START wird der state umgeschaltet
    state = !state;
    ESP_LOGI(tag, "state = %d", state);
    RmtIr* rmtIr = &rmtIr->getInstance(); // get the Singleton instance

    if (!state) {
        switchAllOff(rmtIr);
    }
    else {
        rmtIr->transmitNecCommandFrame((uint16_t)0x7e81, (uint16_t)0x2ad5); // "Power 0/1"
        vTaskDelay(pdMS_TO_TICKS(2000)); // delay 2 seconds
        rmtIr->transmitNecCommandFrame((uint16_t)0x7a85, (uint16_t)0x16e9); // "Tuner"
    }
}

extern "C" void app_main(void)
{
    // short delay to reconnect logging

    vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds

    ESP_LOGI(tag, "Example Program");

    /* Initialize RmtIr class */
    ESP_LOGI(tag, "RmtIr");
    RmtIr* rmtIr = &rmtIr->getInstance(); // get the Singleton instance
    rmtIr->setGpioPins(32,0); // set the GPIO pins: external IR emitter, no IR receiver
    //rmtIr->setGpioPins(12,26); // set the GPIO pins: internal IR emitter, external IR receiver
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
    button_event_args_t press_time = {
       { // long_press
           1000, // press_time
       }
    };
    onBoardButton.RegisterCallbackForEvent(BUTTON_LONG_PRESS_START, &press_time, callback_onBoardButton_BUTTON_LONG_PRESS_START_1000);
    button_event_args_t clicks = {
       { // multiple_clicks
           3, // clicks
       }
    };
    onBoardButton.RegisterCallbackForEvent(BUTTON_MULTIPLE_CLICK, &clicks, callback_onBoardButton_BUTTON_MULTIPLE_CLICK_3);

    // receiver test
    while(1) {
        ESP_LOGI(tag, "receiveRmtFrame");
        //rmtIr->receiveRmtFrame();
        vTaskDelay(pdMS_TO_TICKS(300000)); // delay 300 seconds
    }

}
