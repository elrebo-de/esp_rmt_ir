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
#include "ir_panasonic_encoder.h"

#define EXAMPLE_IR_RESOLUTION_HZ     1000000 // 1MHz resolution, 1 tick = 1us
#define EXAMPLE_IR_TX_GPIO_NUM       12
#define EXAMPLE_IR_RX_GPIO_NUM       26
#define EXAMPLE_IR_NEC_DECODE_MARGIN 200     // Tolerance for parsing RMT symbols into bit stream

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
        .mem_block_symbols = 200, // amount of RMT symbols that the channel can store at a time
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
        .mem_block_symbols = 200, // amount of RMT symbols that the channel can store at a time
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
    panasonicProtocol = new PanasonicProtocol();
}

    void RmtIr::transmitNecCommandFrame(uint16_t address, uint16_t code)
    {
        this->necProtocol->transmitNecCommandFrame(tx_channel, address, code);
    }

    void RmtIr::transmitNecRepeatFrame()
    {
        this->necProtocol->transmitNecRepeatFrame(tx_channel);
    }

    void RmtIr::transmitPanasonicCommandFrame(uint16_t non_saving_bits_1, uint8_t system_code, uint8_t address, uint8_t command)
    {
        this->panasonicProtocol->transmitPanasonicCommandFrame(tx_channel, non_saving_bits_1, system_code, address, command);
    }

    void RmtIr::receiveNecOrPanasonicFrame()
    {
    // the following timing requirement is based on NEC protocol, also usable for PANASONIC protocol
    rmt_receive_config_t receive_config = {
        .signal_range_min_ns = 1250,     // the shortest duration for NEC signal is 560us, 1250ns < 560us, valid signal won't be treated as noise
        .signal_range_max_ns = 20000000, // the longest duration for NEC signal is 9000us, 12000000ns > 9000us, the receive won't stop early
        .flags = {
            .en_partial_rx = 0, // ESP32: partial receive not supported
        }
    };

    // save the received RMT symbols
    rmt_symbol_word_t raw_symbols[200]; // 64 symbols should be sufficient for a standard NEC frame and for a standard PANASONIC frame
    rmt_rx_done_event_data_t rx_data;
    // ready to receive
    ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols, sizeof(raw_symbols), &receive_config));

    // wait for RX done signal
    while (xQueueReceive(receive_queue, &rx_data, pdMS_TO_TICKS(1000)) != pdPASS) {
       ESP_LOGI(this->tag.c_str(), "wait for RX done signal");
    }

    printf("NEC or PANASONIC frame start---\r\n");
    for (size_t i = 0; i < rx_data.num_symbols; i++) {
        printf("{%d:%d},{%d:%d}\r\n", rx_data.received_symbols[i].level0, rx_data.received_symbols[i].duration0,
               rx_data.received_symbols[i].level1, rx_data.received_symbols[i].duration1);
    }
    printf("---NEC or PANASONIC frame end: ");
    // decode RMT symbols
    printf("symbol_num=%i\r\n", rx_data.num_symbols);

    // parse the receive symbols and print the result
    if (rx_data.num_symbols == 34 || rx_data.num_symbols == 2)
    {
        this->necProtocol->example_parse_nec_frame(rx_data.received_symbols, rx_data.num_symbols);
    }
    else if (rx_data.num_symbols == 50)
    {
        this->panasonicProtocol->example_parse_panasonic_frame(rx_data.received_symbols, rx_data.num_symbols);
    }
    else
    {
        printf("Unknown NEC or PANASONIC frame\r\n\r\n");
    }
}
