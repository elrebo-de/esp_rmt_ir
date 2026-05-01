/*
 * rmt_ir.cpp
 *
 *      Author: christophoberle
 *
 * this work is licenced under the Apache 2.0 licence
 */

#include <string>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "sdkconfig.h"
#include "esp_log.h"

#include "ir_nec_encoder.h"

#define EXAMPLE_IR_RESOLUTION_HZ     1000000 // 1MHz resolution, 1 tick = 1us
#define EXAMPLE_IR_TX_GPIO_NUM       12
#define EXAMPLE_IR_RX_GPIO_NUM       26
#define EXAMPLE_IR_NEC_DECODE_MARGIN 200     // Tolerance for parsing RMT symbols into bit stream

/**
 * @brief NEC timing spec
 */
#define NEC_LEADING_CODE_DURATION_0  9000
#define NEC_LEADING_CODE_DURATION_1  4500
#define NEC_PAYLOAD_ZERO_DURATION_0  560
#define NEC_PAYLOAD_ZERO_DURATION_1  560
#define NEC_PAYLOAD_ONE_DURATION_0   560
#define NEC_PAYLOAD_ONE_DURATION_1   1690
#define NEC_REPEAT_CODE_DURATION_0   9000
#define NEC_REPEAT_CODE_DURATION_1   2250


#include "rmt_ir.hpp"

RmtIr& RmtIr::getInstance()
{
    static RmtIr instance; // Guaranteed to be destroyed. Instantiated on first use.
    return instance;
}

// Function to set GPIO pins
void RmtIr::setGpioPins( uint16_t txPin,
                         uint16_t rxPin)
{
    ESP_LOGI(tag.c_str(), "Set GPIO pins");
    this->txPin = txPin;
    this->rxPin = rxPin;
}

extern "C"
// static
bool example_rmt_rx_done_callback(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_wakeup = pdFALSE;
    QueueHandle_t receive_queue = (QueueHandle_t)user_data;
    // send the received RMT symbols to the parser task
    xQueueSendFromISR(receive_queue, edata, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

// Function to initialize the NEC IR RMT
void RmtIr::initialize()
{
    ESP_LOGI(tag.c_str(), "Initializing NEC IR RMT");

    ESP_LOGI(tag.c_str(), "create RMT RX channel");
    rmt_rx_channel_config_t rx_channel_cfg = {
        .gpio_num = (gpio_num_t) this->rxPin,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = EXAMPLE_IR_RESOLUTION_HZ,
        .mem_block_symbols = 64, // amount of RMT symbols that the channel can store at a time
        .intr_priority = 0,
        .flags = {
            .invert_in = 0,
            .with_dma = 0, // ESP32: DMA not supported
            .allow_pd = 0, // ESP32: not able to power down in light sleep
        }
    };
    ESP_ERROR_CHECK(rmt_new_rx_channel(&rx_channel_cfg, &rx_channel));

    ESP_LOGI(tag.c_str(), "register RX done callback");
    receive_queue = xQueueCreate(1, sizeof(rmt_rx_done_event_data_t));
    assert(receive_queue);
    rmt_rx_event_callbacks_t cbs = {
        .on_recv_done = example_rmt_rx_done_callback,
    };
    ESP_ERROR_CHECK(rmt_rx_register_event_callbacks(rx_channel, &cbs, receive_queue));

    ESP_LOGI(tag.c_str(), "create RMT TX channel");
    rmt_tx_channel_config_t tx_channel_cfg = {
        .gpio_num = (gpio_num_t) this->txPin,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = EXAMPLE_IR_RESOLUTION_HZ,
        .mem_block_symbols = 64, // amount of RMT symbols that the channel can store at a time
        .trans_queue_depth = 4,  // number of transactions that allowed to pending in the background, this example won't queue multiple transactions, so queue depth > 1 is sufficient
        .intr_priority = 0,
        .flags = {
            .invert_out = 1,
            .with_dma = 0, // ESP32: DMA not supported
            .allow_pd = 0, // ESP32: not able to power down in light sleep
            .init_level = 1,
        }
    };
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_channel_cfg, &tx_channel));

    ESP_LOGI(tag.c_str(), "modulate carrier to TX channel");
    rmt_carrier_config_t carrier_cfg = {
        .frequency_hz = 38000, // 38KHz
        .duty_cycle = 0.33,
        .flags = {
            .polarity_active_low = 1,
            .always_on = 1,
        }
    };
    ESP_ERROR_CHECK(rmt_apply_carrier(tx_channel, &carrier_cfg));

    ESP_LOGI(tag.c_str(), "enable RMT TX and RX channels");
    ESP_ERROR_CHECK(rmt_enable(tx_channel));
    ESP_ERROR_CHECK(rmt_enable(rx_channel));

    necProtocol = new NecProtocol();
}

    void RmtIr::transmitNecCommandFrame(uint16_t address, uint16_t code)
    {
        this->necProtocol->transmitNecCommandFrame(tx_channel, address, code);
    }

    void RmtIr::transmitNecRepeatFrame()
    {
        this->necProtocol->transmitNecRepeatFrame(tx_channel);
    }

    void RmtIr::receiveNecFrame()
    {
        this->necProtocol->receiveNecFrame(rx_channel, receive_queue);
    }
