/*
 * panasonic_protocol.cpp
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

#include "ir_panasonic_encoder.h"

#define EXAMPLE_IR_RESOLUTION_HZ     1000000 // 1MHz resolution, 1 tick = 1us
#define EXAMPLE_IR_TX_GPIO_NUM       12
#define EXAMPLE_IR_RX_GPIO_NUM       26
#define EXAMPLE_IR_PANASONIC_DECODE_MARGIN 256     // Tolerance for parsing RMT symbols into bit stream

/**
 * @brief PANASONIC timing spec
 */
#define PANASONIC_LEADING_CODE_DURATION_0  3380
#define PANASONIC_LEADING_CODE_DURATION_1  1690
#define PANASONIC_PAYLOAD_ZERO_DURATION_0  420
#define PANASONIC_PAYLOAD_ZERO_DURATION_1  420
#define PANASONIC_PAYLOAD_ONE_DURATION_0   420
#define PANASONIC_PAYLOAD_ONE_DURATION_1   1270
//#define PANASONIC_REPEAT_CODE_DURATION_0   9000
//#define PANASONIC_REPEAT_CODE_DURATION_1   2250


#include "rmt_ir.hpp"

PanasonicProtocol::PanasonicProtocol()
{
    ESP_LOGI(tag.c_str(), "install IR PANASONIC encoder");
    ir_panasonic_encoder_config_t panasonic_encoder_cfg = {
        .resolution = EXAMPLE_IR_RESOLUTION_HZ,
        .invert_out = true,
    };

    ESP_ERROR_CHECK(rmt_new_ir_panasonic_encoder(&panasonic_encoder_cfg, &panasonic_encoder));
}

/**
 * @brief Check whether a duration is within expected range
 */
// static inline
bool PanasonicProtocol::panasonic_check_in_range(uint32_t signal_duration, uint32_t spec_duration)
{
    return (signal_duration < (spec_duration + EXAMPLE_IR_PANASONIC_DECODE_MARGIN)) &&
           (signal_duration > (spec_duration - EXAMPLE_IR_PANASONIC_DECODE_MARGIN));
}

/**
 * @brief Check whether a RMT symbol represents PANASONIC logic zero
 */
// static
bool PanasonicProtocol::panasonic_parse_logic0(rmt_symbol_word_t *rmt_panasonic_symbols)
{
    return panasonic_check_in_range(rmt_panasonic_symbols->duration0, PANASONIC_PAYLOAD_ZERO_DURATION_0) &&
           panasonic_check_in_range(rmt_panasonic_symbols->duration1, PANASONIC_PAYLOAD_ZERO_DURATION_1);
}

/**
 * @brief Check whether a RMT symbol represents PANASONIC logic one
 */
// static
bool PanasonicProtocol::panasonic_parse_logic1(rmt_symbol_word_t *rmt_panasonic_symbols)
{
    return panasonic_check_in_range(rmt_panasonic_symbols->duration0, PANASONIC_PAYLOAD_ONE_DURATION_0) &&
           panasonic_check_in_range(rmt_panasonic_symbols->duration1, PANASONIC_PAYLOAD_ONE_DURATION_1);
}

/**
 * @brief Decode RMT symbols into PANASONIC system_code, address and command
 */
//static
bool PanasonicProtocol::panasonic_parse_frame(rmt_symbol_word_t *rmt_panasonic_symbols)
{
    rmt_symbol_word_t *cur = rmt_panasonic_symbols;
    uint16_t non_saving_bits_1 = 0; // first 16 of 20 non saving bits
    uint8_t system_code = 0; // last 4 of 20 non saving bits and 4 bit system code
    uint8_t address = 0; // 2 non saving bits and 6 bit address code
    uint8_t command = 0; // 8 bit command code
    uint8_t non_saving_bits_2 = 0; // 8 non saving bits
    // and the final pulse

    bool valid_leading_code = panasonic_check_in_range(cur->duration0, PANASONIC_LEADING_CODE_DURATION_0) &&
                              panasonic_check_in_range(cur->duration1, PANASONIC_LEADING_CODE_DURATION_1);
    if (!valid_leading_code) {
        return false;
    }
    cur++;
    for (int i = 0; i < 16; i++) {
        if (panasonic_parse_logic1(cur)) {
            non_saving_bits_1 |= 1 << i;
        } else if (panasonic_parse_logic0(cur)) {
            non_saving_bits_1 &= ~(1 << i);
        } else {
            return false;
        }
        cur++;
    }
    for (int i = 0; i < 8; i++) {
        if (panasonic_parse_logic1(cur)) {
            system_code |= 1 << i;
        } else if (panasonic_parse_logic0(cur)) {
            system_code &= ~(1 << i);
        } else {
            return false;
        }
        cur++;
    }
    for (int i = 0; i < 8; i++) {
        if (panasonic_parse_logic1(cur)) {
            address |= 1 << i;
        } else if (panasonic_parse_logic0(cur)) {
            address &= ~(1 << i);
        } else {
            return false;
        }
        cur++;
    }
    for (int i = 0; i < 8; i++) {
        if (panasonic_parse_logic1(cur)) {
            command |= 1 << i;
        } else if (panasonic_parse_logic0(cur)) {
            command &= ~(1 << i);
        } else {
            return false;
        }
        cur++;
    }
    // save panasonic code data
    s_panasonic_code_non_saving_bits_1 = non_saving_bits_1;
    s_panasonic_code_system_code = system_code;
    s_panasonic_code_address = address;
    s_panasonic_code_command = command;
    s_panasonic_code_non_saving_bits_2 = non_saving_bits_2;
    return true;
}

/**
 * @brief Check whether the RMT symbols represent PANASONIC repeat code
 * I THINK THERE IS NO PANASONIC REPEAT CODE
 * /
// static
bool PanasonicProtocol::panasonic_parse_frame_repeat(rmt_symbol_word_t *rmt_panasonic_symbols)
{
    return panasonic_check_in_range(rmt_panasonic_symbols->duration0, PANASONIC_REPEAT_CODE_DURATION_0) &&
           panasonic_check_in_range(rmt_panasonic_symbols->duration1, PANASONIC_REPEAT_CODE_DURATION_1);
}
*/

/**
 * @brief Decode RMT symbols into PANASONIC scan code and print the result
 */
// static
void PanasonicProtocol::example_parse_panasonic_frame(rmt_symbol_word_t *rmt_panasonic_symbols, size_t symbol_num)
{
    printf("PANASONIC frame start---\r\n");
    for (size_t i = 0; i < symbol_num; i++) {
        printf("{%d:%d},{%d:%d}\r\n", rmt_panasonic_symbols[i].level0, rmt_panasonic_symbols[i].duration0,
               rmt_panasonic_symbols[i].level1, rmt_panasonic_symbols[i].duration1);
    }
    printf("---PANASONIC frame end: ");
    // decode RMT symbols
    printf("symbol_num=%i ", symbol_num);
    switch (symbol_num) {
    case 50: // PANASONIC normal frame
        if (panasonic_parse_frame(rmt_panasonic_symbols)) {
            printf("PANASONIC non_saving_bits_1=%04X, system_code=%02X, Address=%02X, Command=%02X, non_saving_bits_2=%02X\r\n\r\n", s_panasonic_code_non_saving_bits_1, s_panasonic_code_system_code, s_panasonic_code_address, s_panasonic_code_command, s_panasonic_code_non_saving_bits_2);
        }
        break;
    //case 2: // PANASONIC repeat frame
    //    if (panasonic_parse_frame_repeat(rmt_panasonic_symbols)) {
    //        printf("PANASONIC Address=%04X, Command=%04X, repeat\r\n\r\n", s_panasonic_code_address, s_panasonic_code_command);
    //    }
    //    break;
    default:
        printf("Unknown PANASONIC frame\r\n\r\n");
        break;
    }
}

void PanasonicProtocol::receivePanasonicFrame(
         rmt_channel_handle_t rx_channel,
         QueueHandle_t receive_queue)
{
    // the following timing requirement is based on PANASONIC protocol
    rmt_receive_config_t receive_config = {
        .signal_range_min_ns = 1250,     // the shortest duration for PANASONIC signal is 560us, 1250ns < 560us, valid signal won't be treated as noise
        .signal_range_max_ns = 12000000, // the longest duration for PANASONIC signal is 9000us, 12000000ns > 9000us, the receive won't stop early
        .flags = {
            .en_partial_rx = 0, // ESP32: partial receive not supported
        }
    };

    // save the received RMT symbols
    rmt_symbol_word_t raw_symbols[64]; // 64 symbols should be sufficient for a standard PANASONIC frame
    rmt_rx_done_event_data_t rx_data;
    // ready to receive
    ESP_ERROR_CHECK(rmt_receive(rx_channel, raw_symbols, sizeof(raw_symbols), &receive_config));

    // wait for RX done signal
    while (xQueueReceive(receive_queue, &rx_data, pdMS_TO_TICKS(1000)) != pdPASS) {
       ESP_LOGI(this->tag.c_str(), "wait for RX done signal");
    }

    // parse the receive symbols and print the result
    example_parse_panasonic_frame(rx_data.received_symbols, rx_data.num_symbols);
}

// Function to transmit a PANASONIC command frame
void PanasonicProtocol::transmitPanasonicCommandFrame(
         rmt_channel_handle_t tx_channel,
         uint16_t address,
         uint16_t code)
{
    ESP_LOGI(tag.c_str(), "Transmit a PANASONIC command frame address=%04X, code=%04X", address, code);

    // this example won't send PANASONIC frames in a loop
    rmt_transmit_config_t transmit_config = {
        .loop_count = 0, // no loop
        .flags = {
            .eot_level = 1,
            .queue_nonblocking = 1,
        }
    };

    const ir_panasonic_scan_code_t scan_code = {
        .address = address,
        .command = code,
    };
    ESP_ERROR_CHECK(rmt_transmit(tx_channel, panasonic_encoder, &scan_code, sizeof(scan_code), &transmit_config));
}

// Function to transmit a PANASONIC repeat frame
void PanasonicProtocol::transmitPanasonicRepeatFrame(
         rmt_channel_handle_t tx_channel)
{
    ESP_LOGI(tag.c_str(), "Transmit a PANASONIC repeat frame (not yet implemented!)");
}


