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
   Class to implement the NEC protocol of IR remote controllers.

   The original code is taken from the GitHub repository https://github.com/espressif/esp-idf.git
   from file esp-idf/examples/peripherals/rmt/ir_nec_transceiver.
*/

class PanasonicProtocol
{
    public:
        PanasonicProtocol();

        bool panasonic_check_in_range(uint32_t signal_duration, uint32_t spec_duration);
        bool panasonic_parse_logic0(rmt_symbol_word_t *rmt_panasonic_symbols);
        bool panasonic_parse_logic1(rmt_symbol_word_t *rmt_panasonic_symbols);
        bool panasonic_parse_frame(rmt_symbol_word_t *rmt_panasonic_symbols);
        bool panasonic_parse_frame_repeat(rmt_symbol_word_t *rmt_panasonic_symbols);
        void example_parse_panasonic_frame(rmt_symbol_word_t *rmt_panasonic_symbols, size_t symbol_num);

        void transmitPanasonicCommandFrame(rmt_channel_handle_t tx_channel, uint16_t address, uint16_t code);
        void transmitPanasonicRepeatFrame(rmt_channel_handle_t tx_channel);
        void receivePanasonicFrame(rmt_channel_handle_t rx_channel, QueueHandle_t receive_queue);

    private:
        std::string tag = "PanasonicProtocol";
        rmt_encoder_handle_t panasonic_encoder = NULL;

        /**
         * @brief Saving RMT decode results
         */
        uint16_t s_panasonic_code_address;
        uint16_t s_panasonic_code_command;
};

#endif /* PANASONIC_PROTOCOL_HPP_ */
