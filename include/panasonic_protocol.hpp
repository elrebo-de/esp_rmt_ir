/*
 * panasonic_protocol.hpp
 *
 *      Author: christophoberle
 *
 * this work is licenced under the Apache 2.0 licence
 */

#ifndef PANASONIC_PROTOCOL_HPP_
#define PANASONIC_PROTOCOL_HPP_

#include <string>
#include "driver/rmt_tx.h"
#include "driver/rmt_rx.h"

/* class PanasonicProtocol
   Class to implement the PANASONIC protocol of IR remote controllers for TVs.

   The original code is taken from the GitHub repository https://github.com/espressif/esp-idf.git
   from file esp-idf/examples/peripherals/rmt/ir_nec_transceiver and tweaked to the PANASONIC protocol.

   From different sources I found the structure of a PANASONIC frame like this:
   - non_saving_bits_1      16 bits (fixed for the remote control)
   - system_code             8 bits
   - address                 8 bits
   - command                 8 bits
   - checksum                8 bits (checksum is calculated as system_code XOR address XOR command)


*/

class PanasonicProtocol
{
    public:
        PanasonicProtocol();

        bool panasonic_check_in_range(uint32_t signal_duration, uint32_t spec_duration);
        bool panasonic_parse_logic0(rmt_symbol_word_t *rmt_panasonic_symbols);
        bool panasonic_parse_logic1(rmt_symbol_word_t *rmt_panasonic_symbols);
        bool panasonic_parse_frame(rmt_symbol_word_t *rmt_panasonic_symbols);
        void example_parse_panasonic_frame(rmt_symbol_word_t *rmt_panasonic_symbols, size_t symbol_num);

        void transmitPanasonicCommandFrame(rmt_channel_handle_t tx_channel, uint16_t non_saving_bits_1, uint8_t system_code, uint8_t address, uint8_t command);
        void receivePanasonicFrame(rmt_channel_handle_t rx_channel, QueueHandle_t receive_queue);

    private:
        std::string tag = "PanasonicProtocol";
        rmt_encoder_handle_t panasonic_encoder = NULL;

        /**
         * @brief Saving RMT decode results
         */
        uint16_t s_panasonic_code_non_saving_bits_1;
        uint8_t s_panasonic_code_system_code;
        uint8_t s_panasonic_code_address;
        uint8_t s_panasonic_code_command;
        uint8_t s_panasonic_code_checksum;
};

#endif /* PANASONIC_PROTOCOL_HPP_ */
