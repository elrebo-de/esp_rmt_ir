/*
 * pioneer_protocol.hpp
 *
 *      Author: christophoberle
 *
 * this work is licenced under the Apache 2.0 licence
 */

#ifndef PIONEER_PROTOCOL_HPP_
#define PIONEER_PROTOCOL_HPP_

#include <string>
#include "driver/rmt_tx.h"
#include "driver/rmt_rx.h"

/* class PioneerProtocol
   Class to implement the Pioneer protocol of IR remote controllers.

   The original code is taken from the GitHub repository https://github.com/espressif/esp-idf.git
   from file esp-idf/examples/peripherals/rmt/ir_nec_transceiver.
*/

class PioneerProtocol
{
    public:
        PioneerProtocol();

        bool pioneer_check_in_range(uint32_t signal_duration, uint32_t spec_duration);
        bool pioneer_parse_logic0(rmt_symbol_word_t *rmt_pioneer_symbols);
        bool pioneer_parse_logic1(rmt_symbol_word_t *rmt_pioneer_symbols);
        bool pioneer_parse_frame(rmt_symbol_word_t *rmt_pioneer_symbols);
        bool pioneer_parse_frame_repeat(rmt_symbol_word_t *rmt_pioneer_symbols);
        void example_parse_pioneer_frame(rmt_symbol_word_t *rmt_pioneer_symbols, size_t symbol_num);

        void transmitPioneerCommandFrame(rmt_channel_handle_t tx_channel, uint8_t address1, uint8_t command1, uint8_t address2, uint8_t command2);
        void transmitPioneerCommandFrame(rmt_channel_handle_t tx_channel, uint16_t address1, uint16_t command1, uint16_t address2, uint16_t command2);
        void receivePioneerFrame(rmt_channel_handle_t rx_channel, QueueHandle_t receive_queue);

    private:
        std::string tag = "PioneerProtocol";
        rmt_encoder_handle_t pioneer_encoder = NULL;

        /**
         * @brief Saving RMT decode results
         */
        uint16_t s_pioneer_code_address1;
        uint16_t s_pioneer_code_command1;
        uint16_t s_pioneer_code_address2;
        uint16_t s_pioneer_code_command2;
};

#endif /* PIONEER_PROTOCOL_HPP_ */
