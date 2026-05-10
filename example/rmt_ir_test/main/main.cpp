/*
 * Example program to use rmt_ir functionality with elrebo-de/esp_rmt_ir
 */

#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "rmt_ir.hpp"
#include "generic_button.hpp"
#include "generic_nvsflash.hpp"
#include "wifi_manager.hpp"
#include "shelly_plug.hpp"
ShellyPlug *shellyPlug = NULL;
#include <iot_button.h>


static const char *tag = "rmt ir test";

bool state = false;

// update value of key in NvsFlash
esp_err_t updateNvsFlash(std::string tag, std::string space, std::string key, uint8_t value);
{
    GenericNvsFlash nvsRmt(tag, space, NVS_READWRITE);
    return nvsRmt.SetU8(key, value);
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
        // turn off shelly plug
        shellyPlug->Switch(false);
        ESP_LOGI(tag, "Response: %s", shellyPlug->ReadResponse().c_str()); // ReadResponse needs 1000 msec
    }
    else {
        // turn on shelly plug
        shellyPlug->Switch(true);
        ESP_LOGI(tag, "Response: %s", shellyPlug->ReadResponse().c_str()); // ReadResponse needs 1000 msec

        vTaskDelay(pdMS_TO_TICKS(2000)); // delay 2 seconds
        rmtIr->transmitNecCommandFrame(0x857a, 0x7c03); // "TV Scene"
        vTaskDelay(pdMS_TO_TICKS(10000)); // delay 10 seconds
        //rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x00, 0xbc); // "Power 0/1"
        //vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds
        rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x40, 0x0c); // "TV"
    }

    // update state in nvsFlash
    updateNvsFlash(std::string("nvsRmt"), std::string("rmt"), std::string("state"), uint8_t state);
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
    if (state) {
        // turn on shelly plug
        shellyPlug->Switch(true);
        ESP_LOGI(tag, "Response: %s", shellyPlug->ReadResponse().c_str()); // ReadResponse needs 1000 msec

        //vTaskDelay(pdMS_TO_TICKS(10000)); // delay 2 seconds
        //rmtIr->transmitNecCommandFrame(0x817e, 0xd52a); // "Power 0/1"
        vTaskDelay(pdMS_TO_TICKS(2000)); // delay 0.5 seconds
        rmtIr->transmitNecCommandFrame(0x857a, 0x7609); // "RADIO Scene" (AppleTV)
        vTaskDelay(pdMS_TO_TICKS(10000)); // delay 0.5 seconds
        //rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x00, 0xbc); // "Power 0/1"
        //vTaskDelay(pdMS_TO_TICKS(2000)); // delay 0.5 seconds
        rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x00, 0xa0); // "AV" (HDMI1)
        vTaskDelay(pdMS_TO_TICKS(5000)); // delay 0.5 seconds
        rmtIr->transmitPanasonicCommandFrame(0x4004, 0x01, 0x00, 0x92); // "OK"
    }
    else {
        // turn off shelly plug
        shellyPlug->Switch(false);
        ESP_LOGI(tag, "Response: %s", shellyPlug->ReadResponse().c_str()); // ReadResponse needs 1000 msec
    }

    // update state in nvsFlash
    updateNvsFlash(std::string("nvsRmt"), std::string("rmt"), std::string("state"), uint8_t state);
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

    if (state) {
        // turn on shelly plug
        shellyPlug->Switch(true);
        ESP_LOGI(tag, "Response: %s", shellyPlug->ReadResponse().c_str()); // ReadResponse needs 1000 msec

        vTaskDelay(pdMS_TO_TICKS(2000)); // delay 2 seconds
        rmtIr->transmitNecCommandFrame(0x817e, 0xd52a); // "Power 0/1"
        vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds
        rmtIr->transmitNecCommandFrame(0x857a, 0xe916); // "Tuner"
    }
    else {
        // turn off shelly plug
        shellyPlug->Switch(false);
        ESP_LOGI(tag, "Response: %s", shellyPlug->ReadResponse().c_str()); // ReadResponse needs 1000 msec
    }

    // update state in nvsFlash
    updateNvsFlash(std::string("nvsRmt"), std::string("rmt"), std::string("state"), uint8_t state);
}

extern "C" void app_main(void)
{
    // short delay to reconnect logging
    vTaskDelay(pdMS_TO_TICKS(500)); // delay 0.5 seconds

    ESP_LOGI(tag, "Example Program");

    /* Initialize RmtIr class */
    ESP_LOGI(tag, "RmtIr");
    RmtIr* rmtIr = &rmtIr->getInstance(); // get the Singleton instance
    rmtIr->setGpioPins(4,0); // set the GPIO pins
    rmtIr->initialize(); // initialize RMT IR

    GenericButton onBoardButton(
	    std::string("onBoardButton"),
	    /* M5 Atom Lite */
	    (gpio_num_t) 9, // GPIO
	    0, // active = DOWN
	    true, // pull disabled - M5 Atom does not support internal PU/PD on this gpio
	    std::string("GPIO")
	);

    onBoardButton.RegisterCallbackForEvent(BUTTON_SINGLE_CLICK, callback_onBoardButton_BUTTON_SINGLE_CLICK);
    onBoardButton.RegisterCallbackForEvent(BUTTON_DOUBLE_CLICK, callback_onBoardButton_BUTTON_DOUBLE_CLICK);
    button_event_args_t args = {
       { // long_press
           1000, // press_time
       }
    };
    onBoardButton.RegisterCallbackForEvent(BUTTON_LONG_PRESS_START, &args, callback_onBoardButton_BUTTON_LONG_PRESS_START_1000);

    /* Initialize WifiManager class */
    ESP_LOGI(tag, "WifiManager");
    Wifi wifi(
		std::string("WifiManager"), // tag
		std::string("ESP32"), // ssid_prefix
		std::string("de-DE") // language
    );

    ESP_LOGI(tag, "Wifi is %s", wifi.IsConnected() ? "connected" : "not connected");

    ESP_LOGI(tag, "Ssid: %s", wifi.GetSsid().c_str());
    ESP_LOGI(tag, "IpAddress: %s", wifi.GetIpAddress().c_str());
    ESP_LOGI(tag, "Rssi: %i", wifi.GetRssi());
    ESP_LOGI(tag, "Channel: %i", wifi.GetChannel());
    ESP_LOGI(tag, "MacAddress: %s", wifi.GetMacAddress().c_str());

    /* Configure the ShellyPlug */
    std::string ipAddrShellyPlug = std::string("192.168.178.102");

    shellyPlug = new ShellyPlug(std::string("shellyPlug"), ipAddrShellyPlug);

    if (shellyPlug->GetLastHttpCode() != HTTP_CODE_OK) {
        ESP_LOGI(tag, "No Shelly Plug at IPAddr %s", ipAddrShellyPlug.c_str());
        return;
    }

    // read state from nvs_flash
    {
        /* Open NVS flash Namespace "rmt" for read operations */
        GenericNvsFlash nvsRmt(std::string("nvsRmt"), std::string("rmt"), NVS_READONLY);

        esp_err_t ret = ESP_OK;

        /* Read value for key "state" from Namespace "rmt" */
        uint8_t nvsState = nvsRmt.GetU8(std::string("state"), &ret);

        if (ret == ESP_OK) {
            state = (bool) nvsState;
        }

        ESP_LOGI(tag, "state = %d", state);

        if (!state) {
            // turn off shelly plug
            shellyPlug->Switch(false);
            ESP_LOGI(tag, "Response: %s", shellyPlug->ReadResponse().c_str()); // ReadResponse needs 1000 msec

            // update state in nvsFlash
            updateNvsFlash(std::string("nvsRmt"), std::string("rmt"), std::string("state"), uint8_t state);
        }
    }

    // receiver test
    while(1) {
        //ESP_LOGI(tag, "receiveNecOrPanasonicFrame");
        //rmtIr->receiveNecOrPanasonicFrame();
        vTaskDelay(pdMS_TO_TICKS(30000)); // delay 30 seconds
    }

}
