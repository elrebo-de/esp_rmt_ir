/*
 * pioneer_protocol.cpp
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
#include "lwip/sockets.h" // Wichtig: Für htonl/htons im ESP-IDF

#include "ir_pioneer_encoder.h"

#define EXAMPLE_IR_RESOLUTION_HZ     1000000 // 1MHz resolution, 1 tick = 1us
#define EXAMPLE_IR_TX_GPIO_NUM       12
#define EXAMPLE_IR_RX_GPIO_NUM       26
#define EXAMPLE_IR_PIONEER_DECODE_MARGIN 350     // Tolerance for parsing RMT symbols into bit stream

/**
 * @brief Pioneer timing spec
 */
#define PIONEER_LEADING_CODE_DURATION_0  8250
#define PIONEER_LEADING_CODE_DURATION_1  4200
#define PIONEER_PAYLOAD_ZERO_DURATION_0  500
#define PIONEER_PAYLOAD_ZERO_DURATION_1  500
#define PIONEER_PAYLOAD_ONE_DURATION_0   500
#define PIONEER_PAYLOAD_ONE_DURATION_1   1500


#include "rmt_ir.hpp"

PioneerProtocol::PioneerProtocol()
{
    ESP_LOGI(tag.c_str(), "install IR PIONEER encoder");
    ir_pioneer_encoder_config_t pioneer_encoder_cfg = {
        .resolution = EXAMPLE_IR_RESOLUTION_HZ,
        .invert_out = true,
    };

    ESP_ERROR_CHECK(rmt_new_ir_pioneer_encoder(&pioneer_encoder_cfg, &pioneer_encoder));
}

/**
 * @brief Check whether a duration is within expected range
 */
// static inline
bool PioneerProtocol::pioneer_check_in_range(uint32_t signal_duration, uint32_t spec_duration)
{
    return (signal_duration < (spec_duration + EXAMPLE_IR_PIONEER_DECODE_MARGIN)) &&
           (signal_duration > (spec_duration - EXAMPLE_IR_PIONEER_DECODE_MARGIN));
}

/**
 * @brief Check whether a RMT symbol represents PIONEER logic zero
 */
// static
bool PioneerProtocol::pioneer_parse_logic0(rmt_symbol_word_t *rmt_pioneer_symbols)
{
    return pioneer_check_in_range(rmt_pioneer_symbols->duration0, PIONEER_PAYLOAD_ZERO_DURATION_0) &&
           pioneer_check_in_range(rmt_pioneer_symbols->duration1, PIONEER_PAYLOAD_ZERO_DURATION_1);
}

/**
 * @brief Check whether a RMT symbol represents PIONEER logic one
 */
// static
bool PioneerProtocol::pioneer_parse_logic1(rmt_symbol_word_t *rmt_pioneer_symbols)
{
    return pioneer_check_in_range(rmt_pioneer_symbols->duration0, PIONEER_PAYLOAD_ONE_DURATION_0) &&
           pioneer_check_in_range(rmt_pioneer_symbols->duration1, PIONEER_PAYLOAD_ONE_DURATION_1);
}

/**
 * @brief Decode RMT symbols into PIONEER address and command
 */
//static
bool PioneerProtocol::pioneer_parse_frame(rmt_symbol_word_t *rmt_pioneer_symbols)
{
    rmt_symbol_word_t *cur = rmt_pioneer_symbols;
    uint16_t address1 = 0;
    uint16_t command1 = 0;
    uint16_t address2 = 0;
    uint16_t command2 = 0;
    bool valid_leading_code = pioneer_check_in_range(cur->duration0, PIONEER_LEADING_CODE_DURATION_0) &&
                              pioneer_check_in_range(cur->duration1, PIONEER_LEADING_CODE_DURATION_1);
    if (!valid_leading_code) {
        ESP_LOGI(tag.c_str(), "leading_code not valid!");
        return false;
    }
    cur++;
    for (int i = 0; i < 16; i++) {
        if (pioneer_parse_logic1(cur)) {
            address1 |= 1 << i;
        } else if (pioneer_parse_logic0(cur)) {
            address1 &= ~(1 << i);
        } else {
            ESP_LOGI(tag.c_str(), "address1 not valid!");
            return false;
        }
        cur++;
    }
    for (int i = 0; i < 16; i++) {
        if (pioneer_parse_logic1(cur)) {
            command1 |= 1 << i;
        } else if (pioneer_parse_logic0(cur)) {
            command1 &= ~(1 << i);
        } else {
            ESP_LOGI(tag.c_str(), "command1 not valid!");
            return false;
        }
        cur++;
    }
    cur = cur + 1; // skip trailing code
    cur = cur + 1; // skip leading code
    for (int i = 0; i < 16; i++) {
        if (pioneer_parse_logic1(cur)) {
            address2 |= 1 << i;
        } else if (pioneer_parse_logic0(cur)) {
            address2 &= ~(1 << i);
        } else {
            ESP_LOGI(tag.c_str(), "address2 not valid!");
            return false;
        }
        cur++;
    }
    for (int i = 0; i < 16; i++) {
        if (pioneer_parse_logic1(cur)) {
            command2 |= 1 << i;
        } else if (pioneer_parse_logic0(cur)) {
            command2 &= ~(1 << i);
        } else {
            ESP_LOGI(tag.c_str(), "command2 not valid!");
            return false;
        }
        cur++;
    }
    // save addresses and commands
    s_pioneer_code_address1 = ntohs(address1);
    s_pioneer_code_command1 = ntohs(command1);
    s_pioneer_code_address2 = ntohs(address2);
    s_pioneer_code_command2 = ntohs(command2);
    return true;
}

/**
 * @brief Decode RMT symbols into PIONEER scan code and print the result
 */
// static
void PioneerProtocol::example_parse_pioneer_frame(rmt_symbol_word_t *rmt_pioneer_symbols, size_t symbol_num)
{
    //       printf("PIONEER frame start---\r\n");
    //       for (size_t i = 0; i < symbol_num; i++) {
    //           printf("{%d:%d},{%d:%d}\r\n", rmt_pioneer_symbols[i].level0, rmt_pioneer_symbols[i].duration0,
    //                  rmt_pioneer_symbols[i].level1, rmt_pioneer_symbols[i].duration1);
    //       }
    //       printf("---PIONEER frame end: ");
    //       // decode RMT symbols
    //       printf("symbol_num=%i ", symbol_num);
    switch (symbol_num) {
    case 68: // PIONEER normal frame
        if (pioneer_parse_frame(rmt_pioneer_symbols)) {
            printf("PIONEER Address1=%04X, Command1=%04X, Address2=%04X, Command2=%04X\r\n\r\n", s_pioneer_code_address1, s_pioneer_code_command1, s_pioneer_code_address2, s_pioneer_code_command2);
        }
        break;
    default:
        printf("Unknown PIONEER frame\r\n\r\n");
        break;
    }
}

void PioneerProtocol::receivePioneerFrame(
         rmt_channel_handle_t rx_channel,
         QueueHandle_t receive_queue)
{
    // the following timing requirement is based on PIONEER protocol
    rmt_receive_config_t receive_config = {
        .signal_range_min_ns = 1250,     // the shortest duration for PIONEER signal is 560us, 1250ns < 560us, valid signal won't be treated as noise
        .signal_range_max_ns = 30000000, // the longest duration for PIONEER signal is 9000us, 12000000ns > 9000us, the receive won't stop early
        .flags = {
            .en_partial_rx = 0, // ESP32: partial receive not supported
        }
    };


    // save the received RMT symbols
    rmt_symbol_word_t raw_symbols[64]; // 64 symbols should be sufficient for a standard PIONEER frame
    rmt_rx_done_event_data_t rx_data;
    // ready to receive
    ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols, sizeof(raw_symbols), &receive_config));

    // wait for RX done signal
    while (xQueueReceive(receive_queue, &rx_data, pdMS_TO_TICKS(1000)) != pdPASS) {
       ESP_LOGI(this->tag.c_str(), "wait for RX done signal");
    }

    // parse the receive symbols and print the result
    example_parse_pioneer_frame(rx_data.received_symbols, rx_data.num_symbols);
}

// Function to transmit a PIONEER command frame
void PioneerProtocol::transmitPioneerCommandFrame(
         rmt_channel_handle_t tx_channel,
         uint8_t address1,
         uint8_t command1,
         uint8_t address2,
         uint8_t command2)
{
    ESP_LOGI(tag.c_str(), "Prepare a PIONEER command frame address1=%02X, command1=%02X, address2=%02X, command2=%02X", address1, command1, address2, command2);

    uint8_t address1_inverted = ~address1;
    uint16_t address1_16 = (((uint16_t) address1) << 8) + (uint16_t) address1_inverted;

    uint8_t command1_inverted = ~command1;
    uint16_t command1_16 = (((uint16_t) command1) << 8) + (uint16_t) command1_inverted;

    uint8_t address2_inverted = ~address2;
    uint16_t address2_16 = (((uint16_t) address2) << 8) + (uint16_t) address2_inverted;

    uint8_t command2_inverted = ~command2;
    uint16_t command2_16 = (((uint16_t) command2) << 8) + (uint16_t) command2_inverted;

    this->transmitPioneerCommandFrame(tx_channel, address1_16, command1_16, address2_16, command2_16);
}

// Function to transmit a PIONEER command frame
void PioneerProtocol::transmitPioneerCommandFrame(
         rmt_channel_handle_t tx_channel,
         uint16_t address1,
         uint16_t command1,
         uint16_t address2,
         uint16_t command2)
{
    ESP_LOGI(tag.c_str(), "Transmit a PIONEER command frame address1=%04X, command1=%04X, address2=%04X, command2=%04X", address1, command1, address2, command2);

    // this example won't send PIONEER frames in a loop
    rmt_transmit_config_t transmit_config = {
        .loop_count = 0, // no loop
        .flags = {
            .eot_level = 1,
            .queue_nonblocking = 1,
        }
    };

    const ir_pioneer_scan_code_t scan_code = {
        .address1 = address1,
        .command1 = command1,
        .address2 = address2,
        .command2 = command2,
    };

    ESP_LOGI(tag.c_str(), "apply 40kHz carrier to TX channel");
    rmt_carrier_config_t carrier_cfg = {
        .frequency_hz = 40000, // 40KHz
        .duty_cycle = 0.33,
        .flags = {
            .polarity_active_low = 1,
            .always_on = 1,
        }
    };
    ESP_ERROR_CHECK(rmt_disable(tx_channel));
    ESP_ERROR_CHECK(rmt_apply_carrier(tx_channel, &carrier_cfg));
    ESP_ERROR_CHECK(rmt_enable(tx_channel));
    ESP_ERROR_CHECK(rmt_transmit(tx_channel, pioneer_encoder, &scan_code, sizeof(scan_code), &transmit_config));
    //vTaskDelay(pdMS_TO_TICKS(1000)); // delay 1 seconds
}

