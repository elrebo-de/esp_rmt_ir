/*
 * nec_protocol.hpp
 *
 *      Author: christophoberle
 *
 * this work is licenced under the Apache 2.0 licence
 */

#ifndef NEC_PROTOCOL_HPP_
#define NEC_PROTOCOL_HPP_

#include <string>
#include "driver/rmt_tx.h"
#include "driver/rmt_rx.h"

/* class NecProtocol
   Class to implement the NEC protocol of IR remote controllers.

   The original code is taken from the GitHub repository https://github.com/espressif/esp-idf.git
   from file esp-idf/examples/peripherals/rmt/ir_nec_transceiver.
*/

class NecProtocol
{
    public:
        NecProtocol();

        bool nec_check_in_range(uint32_t signal_duration, uint32_t spec_duration);
        bool nec_parse_logic0(rmt_symbol_word_t *rmt_nec_symbols);
        bool nec_parse_logic1(rmt_symbol_word_t *rmt_nec_symbols);
        bool nec_parse_frame(rmt_symbol_word_t *rmt_nec_symbols);
        bool nec_parse_frame_repeat(rmt_symbol_word_t *rmt_nec_symbols);
        void example_parse_nec_frame(rmt_symbol_word_t *rmt_nec_symbols, size_t symbol_num);

        void transmitNecCommandFrame(rmt_channel_handle_t tx_channel, uint8_t address, uint8_t command);
        void transmitNecCommandFrame(rmt_channel_handle_t tx_channel, uint16_t address, uint8_t command);
        void transmitNecCommandFrame(rmt_channel_handle_t tx_channel, uint16_t address, uint16_t command);
        void transmitNecRepeatFrame(rmt_channel_handle_t tx_channel);
        void receiveNecFrame(rmt_channel_handle_t rx_channel, QueueHandle_t receive_queue);

    private:
        std::string tag = "NecProtocol";
        rmt_encoder_handle_t nec_encoder = NULL;

        /**
         * @brief Saving RMT decode results
         */
        uint16_t s_nec_code_address;
        uint16_t s_nec_code_command;
};

#endif /* NEC_PROTOCOL_HPP_ */
